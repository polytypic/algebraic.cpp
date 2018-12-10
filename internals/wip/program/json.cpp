#include "json.hpp"
#include "core/structures.hpp"
#include "generics/pretty.hpp"

static auto &null_d() {
  static auto d = txt("null");
  return d;
}
static auto &true_d() {
  static auto d = txt("true");
  return d;
}
static auto &false_d() {
  static auto d = txt("false");
  return d;
}

static auto aggregate(const lhs_rhs_t<doc_t> &lr, const list_t<doc_t> &ds) {
  return group(
      nest(2, lhs(lr) + linebreak() + cat(intersperse(commaline(), ds))) +
      linebreak() + rhs(lr));
}

doc_t pretty(const json_t &json) {
  if (auto object = as<L(Object)>(json))
    return aggregate(braces(), map<append>(
                                   [](auto &kv) {
                                     return gnest(2, pretty(key(kv)) +
                                                         colonline() +
                                                         pretty(value(kv)));
                                   },
                                   it(object)));
  else if (auto array = as<L(Array)>(json))
    return aggregate(
        brackets(),
        map<append>([](auto &json) { return pretty(json); }, it(array)));
  else if (auto string = as<L(String)>(json))
    return pretty(it(string));
  else if (auto number = as<L(Number)>(json))
    return pretty(it(number));
  else if (auto boolean = as<L(Boolean)>(json))
    return it(boolean) ? true_d() : false_d();
  else
    return null_d();
}
