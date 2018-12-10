#include "c_str_stream.hpp"
#include "parsing.hpp"

#include "generics/pretty.hpp"

#include "json.hpp"

#include "util/test_runner.hpp"

#include <cctype>
#include <iostream>

using S = c_str_stream_t;
using P = parsing<S>;

static bool is_alpha(char c) { return std::isalpha(c); }
static bool is_alnum(char c) { return std::isalnum(c); }
static bool is_digit(char c) { return std::isdigit(c); }
static bool is_space(char c) { return std::isspace(c); }

static std::string to_string(const list_t<char> &cs) {
  std::string result;
  for_each([&](auto c) { result += c; }, cs);
  return result;
}

static P::parser_t<none_t> spaces = P::skip_many(P::sat(is_space));

template <class X> static P::parser_t<X> token(const P::parser_t<X> &xP) {
  return map<P>([](auto &x, auto &) { return x; }, xP, spaces);
}

static P::parser_t<std::string> id_p =
    token(map<P>(to_string, P::one_many(P::sat(is_alpha), P::sat(is_alnum))));

static P::parser_t<none_t> kw_p(const char *cs) {
  return P::guess(bind<P>(
      [=](auto &s) { return s == cs ? pure<P>(none) : zero<P, none_t>(); },
      id_p));
}

static P::parser_t<char> chr(char c) {
  return P::sat([=](auto x) { return x == c; });
}

static P::parser_t<char> sep_p(char c) { return token(chr(c)); }

static P::parser_t<std::string> string_p = token(
    map<P>([](auto &, auto &cs, auto &) { return to_string(cs); }, chr('"'),
           P::many(P::sat([](auto c) { return c != '"'; })), chr('"')));

static P::parser_t<double> number_p =
    token(map<P>([](auto &cs) { return std::stod(to_string(cs)); },
                 P::many1(P::sat(is_digit))));

static P::parser_t<json_t> json_p = P::delay([]() {
  return alt<P>(
      map<P>(constant<json_t, none_t>(none), kw_p("null")),
      map<P>(constant<json_t, none_t>(the<L(Boolean)>(true)), kw_p("true")),
      map<P>(constant<json_t, none_t>(the<L(Boolean)>(false)), kw_p("false")),
      map<P>([](auto &s) -> json_t { return the<L(String)>(s); }, string_p),
      map<P>([](auto &n) -> json_t { return the<L(Number)>(n); }, number_p),
      map<P>(
          [](auto &, auto &kvs, auto &) -> json_t {
            return the<L(Object)>(kvs);
          },
          sep_p('{'),
          P::sep_by(sep_p(','), map<P>([](auto &k, auto &,
                                          auto &v) { return key_value(k, v); },
                                       string_p, sep_p(':'), json_p)),
          sep_p('}')),
      map<P>(
          [](auto &, auto &xs, auto &) -> json_t { return the<L(Array)>(xs); },
          sep_p('['), P::sep_by(sep_p(','), json_p), sep_p(']')));
});

template <class A>
static void test_parse(const P::parser_t<A> &aP, const char *input) {
  auto r = P::run(aP, S::from(input), none);
  if (auto ok = as<L(Ok)>(r)) {
    auto d = pretty(get<L(Result)>(ok));
    std::cerr << render(80, txt("Ok ") + align(d)) << std::endl;
  } else if (auto fail = as<L(Fail)>(r)) {
    std::cerr << "Error at: " << it(fail) << std::endl;
  }
}

static auto parsing_test = test_runner([]() {
  {
    auto s = S::from(R"({"foo": [1, true, null]})");
    auto p = map<P>([](auto x) { return float(x); }, pure<P>(101));
    auto r = P::run(p, s, none);
    if (auto ok = as<L(Ok)>(r)) {
      std::cerr << "Ok " << get<L(Result)>(ok) << std::endl;
    }
  }

  {
    P::parser_t<int> pi = zero<P, int>();
    P::parser_t<int> pzo = alt<P>(P::guess(zero<P, int>()), pure<P>(101));
  }

  test_parse(P::elem(), "!");

  test_parse(P::many1(P::elem()), "try");

  test_parse(P::delay([]() {
               return map<P>([](auto &c1, auto &c2,
                                auto &c3) { return list(c1, c2, c3); },
                             P::elem(), P::elem(), P::elem());
             }),
             "abc");

  test_parse(json_p,
             R"({"array": [{"numbers": [101, 69]},
                  {"booleans": [true, false]},
                  {"null": null}],
        "string": "foo bar is a string"})");
});
