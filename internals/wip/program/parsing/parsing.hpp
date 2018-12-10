#pragma once

#include "core/structures.hpp"
#include "core/types.hpp"

#include "types/list_strict.hpp"
#include "types/option.hpp"

#include <functional>

template <class Stream, class User = none_t> struct parsing {
  using user_t = User;

  using stream_t = typename Stream::stream_t;
  using pos_t = typename Stream::pos_t;
  using elem_t = typename Stream::elem_t;

  using error_t = pos_t;

  using dom_t = product_t<the_t<stream_t>, the_t<user_t>>;

  template <class A>
  using reply_t =
      sum_t<the_t<L(Fail), error_t>,
            the_t<L(Ok), product_t<the_t<L(Result), A>, the_t<L(Dom), dom_t>,
                                   the_t<L(Error), error_t>>>>;

  template <class A>
  using cod_t = sum_t<the_t<L(Taste), std::function<reply_t<A>()>>,
                      the_t<L(Empty), reply_t<A>>>;

  template <class A> using parser_t = std::function<cod_t<A>(const dom_t &)>;

private:
  template <class T> struct parser_of_s;
  template <class T> struct parser_of_s<parser_t<T>> : is_type<T> {};
  template <class T> using parser_of_t = tpnm<parser_of_s<T>>;

  static pos_t pos(const dom_t &dom) { return Stream::pos(get<stream_t>(dom)); }

  static option_t<product_t<the_t<elem_t>, the_t<dom_t>>>
  read(const dom_t &dom) {
    return map<option>(
        [&](auto &r) {
          return product(the(head(r)),
                         the(product(the(tail(r)), the(get<user_t>(dom)))));
        },
        Stream::read(get<stream_t>(dom)));
  }

  template <class A>
  static reply_t<A> ok(const A &a, const dom_t &dom, const error_t &m) {
    return the<L(Ok)>(
        product(the<L(Result)>(a), the<L(Dom)>(dom), the<L(Error)>(m)));
  }

  template <class A> static reply_t<A> merge(error_t m, const reply_t<A> &aR) {
    if (auto okR = as<L(Ok)>(aR))
      return ok(get<L(Result)>(okR), get<L(Dom)>(okR), m);
    return the<L(Fail)>(m);
  }

  template <class A> static reply_t<A> bindSome(error_t m, const cod_t<A> &aC) {
    if (auto emptyC = as<L(Empty)>(aC))
      return merge(m, it(emptyC));
    if (auto tasteC = as<L(Taste)>(aC))
      return it(tasteC)();
    UNEXPECTED();
  }

  template <class A> static cod_t<A> replyNone(error_t m, const cod_t<A> &aC) {
    if (auto emptyC = as<L(Empty)>(aC))
      return the<L(Empty)>(merge(m, it(emptyC)));
    return aC;
  }

  template <class A> static cod_t<A> taste(const reply_t<A> &aR) {
    return the<L(Taste)>(constant(aR));
  }

public:
  template <class A>
  static reply_t<A> run(const parser_t<A> &aP, const stream_t &s,
                        const user_t &u) {
    auto aC = aP(product(the(s), the(u)));
    if (auto emptyC = as<L(Empty)>(aC))
      return it(emptyC);
    if (auto taste = as<L(Taste)>(aC))
      return it(taste)();
    UNEXPECTED();
  }

  //

  template <class X> using tycon_t = parser_t<X>;

  template <class MX> using param_t = parser_of_t<MX>;

  template <class APTH, class A = param_t<std::invoke_result_t<APTH>>>
  static parser_t<A> delay(APTH &&apth) {
    lazy_t<parser_t<A>> apl = thunk(std::forward<APTH>(apth));
    return [=](auto &dom) { return it(apl)(dom); };
  }

  template <class A> static tycon_t<std::remove_cvref_t<A>> pure(A &&a) {
    return [=](auto &dom) { return the<L(Empty)>(ok(a, dom, pos(dom))); };
  }

  template <class A, class A2BP,
            class B = param_t<std::invoke_result_t<A2BP, const A &>>>
  static tycon_t<B> bind(A2BP &&a2bP, const tycon_t<A> &aP) {
    return [=](auto &dom) -> cod_t<B> {
      auto aC = aP(dom);
      if (auto aR = as<L(Empty)>(aC)) {
        if (auto fail = as<L(Fail)>(aR))
          return the<L(Empty)>(the<L(Fail)>(it(fail)));
        if (auto ok = as<L(Ok)>(aR))
          return replyNone(get<L(Error)>(ok),
                           a2bP(get<L(Result)>(ok))(get<L(Dom)>(ok)));
      } else if (auto taste = as<L(Taste)>(aC)) {
        auto th = it(taste);
        return the<L(Taste)>([=]() -> reply_t<B> {
          auto aR = th();
          if (auto fail = as<L(Fail)>(aR))
            return the<L(Fail)>(it(fail));
          if (auto ok = as<L(Ok)>(aR))
            return bindSome(get<L(Error)>(ok),
                            a2bP(get<L(Result)>(ok))(get<L(Dom)>(ok)));
          UNEXPECTED();
        });
      }
      UNEXPECTED();
    };
  }

  template <class A> static parser_t<A> zero() {
    return
        [](const dom_t &dom) { return the<L(Empty)>(the<L(Fail)>(pos(dom))); };
  }

  template <class A>
  static parser_t<A> alt(const parser_t<A> &lhsP, const parser_t<A> &rhsP) {
    return [=](auto &dom) {
      auto lhsC = lhsP(dom);
      if (auto empty = as<L(Empty)>(lhsC))
        if (auto fail = as<L(Fail)>(empty))
          return replyNone(it(fail), rhsP(dom));
      return lhsC;
    };
  }

  template <class A> static parser_t<A> guess(const parser_t<A> &aP) {
    return [=](auto &dom) -> cod_t<A> {
      auto aC = aP(dom);
      if (auto th = as<L(Taste)>(aC)) {
        auto aR = it(th)();
        if (auto fail = as<L(Fail)>(aR))
          return the<L(Empty)>(the<L(Fail)>(pos(dom)));
        return taste(aR);
      }
      return aC;
    };
  }

  static const parser_t<elem_t> &elem() {
    static parser_t<elem_t> p([](auto &dom) -> cod_t<elem_t> {
      auto d = read(dom);
      if (auto some = as<L(Some)>(d))
        return taste(ok(get<elem_t>(some), get<dom_t>(some), pos(dom)));
      return the<L(Empty)>(the<L(Fail)>(pos(dom)));
    });
    return p;
  }

  template <class Predicate>
  static parser_t<elem_t> sat(Predicate &&predicate) {
    return [=](auto &dom) -> cod_t<elem_t> {
      auto d = read(dom);
      if (auto dS = as<L(Some)>(d))
        if (predicate(get<elem_t>(dS)))
          return taste(ok(get<elem_t>(dS), get<dom_t>(dS), pos(dom)));
      return the<L(Empty)>(the<L(Fail)>(pos(dom)));
    };
  }

  template <class A>
  static parser_t<list_t<A>> many_rev_to(const list_t<A> &as,
                                         const parser_t<A> &aP) {
    return alt(bind([=](auto &a) { return many_rev_to(cons(a, as), aP); }, aP),
               pure(as));
  }

  template <class A> static parser_t<list_t<A>> many(const parser_t<A> &aP) {
    return map<parsing>(reverse<A>, many_rev_to(list_t<A>(none), aP));
  }

  template <class A>
  static parser_t<list_t<A>> one_many(const parser_t<A> &hP,
                                      const parser_t<A> &tP) {
    return map<parsing>([](auto &h, auto &t) { return cons(h, t); }, hP,
                        many(tP));
  }

  template <class A> static parser_t<list_t<A>> many1(const parser_t<A> &aP) {
    return one_many(aP, aP);
  }

  template <class L, class R, class X>
  static parser_t<X> between(const parser_t<L> &lP, const parser_t<R> &rP,
                             const parser_t<X> &xP) {
    return map<parsing>([](auto &, auto &x, auto &) { return x; }, lP, xP, rP);
  }

  template <class S, class X>
  static parser_t<list_t<X>> sep_by(const parser_t<S> &sP,
                                    const parser_t<X> &xP) {
    return alt(map<parsing>([](auto &x, auto &xs) { return cons(x, xs); }, xP,
                            many(map<parsing>([](auto &, auto &x) { return x; },
                                              sP, xP))),
               pure(list_t<X>(none)));
  }

  template <class X> static parser_t<none_t> skip_many(const parser_t<X> &xP) {
    return alt(bind([=](auto &) { return skip_many(xP); }, xP), pure(none));
  }
};
