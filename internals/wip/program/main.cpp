#include "core/types.hpp"
#include "generics/pretty.hpp"
#include "prettier/prettier.hpp"
#include "types/list_strict.hpp"

#include <iostream>

//

template <class Rec>
using application_t =
    product_t<the_t<L(Function), Rec>, the_t<L(Argument), Rec>>;

template <class Rec> using lambda_t = product_t<the_t<L(Body), Rec>>;

template <class Rec> using variable_t = unsigned;

template <class T, class Rec> using literal_t = T;

template <class Rec>
using plus_t = product_t<the_t<L(Lhs), Rec>, the_t<L(Rhs), Rec>>;

using expression_t = fix_t<tycon_t<
    seq_t<Rec>,
    shared_t<sum_t<
        the_t<L(Application), application_t<Rec>>,
        the_t<L(Lambda), lambda_t<Rec>>, the_t<L(Variable), variable_t<Rec>>,
        the_t<L(Int), literal_t<int, Rec>>, the_t<L(Plus), plus_t<Rec>>>>>>;

//

template <class Rec>
using closure_t =
    product_t<the_t<L(Captured), list_t<Rec>>, the_t<L(Body), expression_t>>;

template <class Rec, class T> using primitive_t = T;

using value_t =
    fix_t<tycon_t<seq_t<Rec>, sum_t<the_t<L(Closure), closure_t<Rec>>,
                                    the_t<L(Int), primitive_t<Rec, int>>>>>;

//

value_t eval(const list_t<value_t> &environment,
             const expression_t &expression) {
  if (auto lambda = as<L(Lambda)>(expression))
    return the<L(Closure)>(product(the<L(Captured)>(environment),
                                   the<L(Body)>(get<L(Body)>(lambda))));
  if (auto application = as<L(Application)>(expression)) {
    auto function = eval(environment, get<L(Function)>(application));
    auto rand = eval(environment, get<L(Argument)>(application));
    auto closure = as<L(Closure)>(function);
    if (!closure)
      PANIC("Expected closure");
    auto env = cons(std::move(rand), get<L(Captured)>(closure));
    return eval(env, get<L(Body)>(closure));
  }
  if (auto variable = as<L(Variable)>(expression)) {
    if (auto vvs = as<L(Some)>(drop(it(variable), environment)))
      return head(vvs);
    PANIC("Invalid environment access");
  }
  if (auto literal = as<L(Int)>(expression))
    return the<L(Int)>(it(literal));
  if (auto plus = as<L(Plus)>(expression)) {
    auto lhsV = eval(environment, lhs(plus));
    auto rhsV = eval(environment, rhs(plus));
    auto lhsI = as<L(Int)>(lhsV);
    if (!lhsI)
      PANIC("Expected integer");
    auto rhsI = as<L(Int)>(rhsV);
    if (!rhsI)
      PANIC("Expected integer");
    return the<L(Int)>(it(lhsI) + it(rhsI));
  }
  UNEXPECTED();
}

int main() {
  auto e = expression_t(the<L(Application)>(product(
      the<L(Function)>(expression_t(the<L(Application)>(product(
          the<L(Function)>(expression_t(the<L(Lambda)>(
              expression_t(the<L(Lambda)>(expression_t(the<L(Plus)>(product(
                  the<L(Lhs)>(expression_t(the<L(Variable)>(1))),
                  the<L(Rhs)>(expression_t(the<L(Variable)>(0))))))))))),
          the<L(Argument)>(expression_t(the<L(Int)>(1))))))),
      the<L(Argument)>(expression_t(the<L(Int)>(2))))));

  std::cerr << render(120, pretty(e)) << std::endl;

  auto v = eval(none, e);

  std::cerr << render(120, pretty(v)) << std::endl;

  return 0;
}
