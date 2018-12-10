#include "prettier.hpp"

#include "types/list_lazy.hpp"
#include "types/list_strict.hpp"

#include <cstring>

//

static std::string spaces(int n) { return std::string(n, ' '); }

//

doc_t operator+(const doc_t &lhs, const doc_t &rhs) {
  return the<L(Join)>(product(the<L(Lhs)>(lhs), the<L(Rhs)>(rhs)));
}

//

doc_t flatten(const doc_t &doc) {
  return delay([=]() {
    if (auto lazy = as<L(Lazy)>(doc))
      return flatten(it(lazy));
    if (auto join = as<L(Join)>(doc))
      return flatten(lhs(join)) + flatten(rhs(join));
    if (auto nest = as<L(Nest)>(doc))
      return nestBy(get<L(Prefix)>(nest), flatten(get<doc_t>(nest)));
    if (auto line = as<L(Line)>(doc))
      return it(line);
    if (auto choice = as<L(Choice)>(doc))
      return get<L(Wide)>(choice);
    if (auto with = as<L(With)>(doc)) {
      auto fn = it(with);
      return doc_t(the<L(With)>([=](int column, auto &prefix) {
        return flatten(fn(column, prefix));
      }));
    }
    return doc;
  });
}

//

doc_t txt(char txt) { return the<L(Char)>(txt); }
doc_t txt(const std::string &txt) { return the<L(String)>(txt); }
doc_t txt(const char *txt) { return the<L(Literal)>(txt); }

doc_t choice(const doc_t &wide, const doc_t &narrow) {
  return the<L(Choice)>(
      product(the<L(Wide)>(flatten(wide)), the<L(Narrow)>(narrow)));
}

doc_t &squote() {
  static auto d = txt('\"');
  return d;
}
doc_t &dquote() {
  static auto d = txt('\'');
  return d;
}

doc_t &lparen() {
  static auto d = txt('(');
  return d;
}
doc_t &rparen() {
  static auto d = txt(')');
  return d;
}
doc_t &lbrace() {
  static auto d = txt('{');
  return d;
}
doc_t &rbrace() {
  static auto d = txt('}');
  return d;
}
doc_t &lbracket() {
  static auto d = txt('[');
  return d;
}
doc_t &rbracket() {
  static auto d = txt(']');
  return d;
}
doc_t &colon() {
  static auto d = txt(':');
  return d;
}
doc_t &comma() {
  static auto d = txt(',');
  return d;
}

doc_t &empty() {
  static auto d = txt("");
  return d;
}
doc_t &space() {
  static auto d = txt(' ');
  return d;
}
doc_t &line() {
  static doc_t d = the<L(Line)>(space());
  return d;
}
doc_t &softline() {
  static doc_t d = choice(space(), line());
  return d;
}
doc_t &linebreak() {
  static doc_t d = the<L(Line)>(empty());
  return d;
}
doc_t &softbreak() {
  static auto d = choice(empty(), linebreak());
  return d;
}

doc_t &commaline() {
  static auto d = comma() + line();
  return d;
}
doc_t &colonline() {
  static auto d = colon() + line();
  return d;
}

lhs_rhs_t<doc_t> &parens() {
  static auto d = product(the<L(Lhs)>(lparen()), the<L(Rhs)>(rparen()));
  return d;
}

lhs_rhs_t<doc_t> &braces() {
  static auto d = product(the<L(Lhs)>(lbrace()), the<L(Rhs)>(rbrace()));
  return d;
}

lhs_rhs_t<doc_t> &brackets() {
  static auto d = product(the<L(Lhs)>(lbracket()), the<L(Rhs)>(rbracket()));
  return d;
}

doc_t delay(const std::function<doc_t()> &th) {
  return the<L(Lazy)>(thunk(th));
}

doc_t nestBy(const std::string &prefix, const doc_t &doc) {
  return the<L(Nest)>(product(the<L(Prefix)>(prefix), the(doc)));
}

doc_t nest(int n, const doc_t &doc) { return nestBy(spaces(n), doc); }

doc_t group(const doc_t &doc) { return choice(doc, doc); }

doc_t gnest(int n, const doc_t &doc) { return group(nest(n, doc)); }

doc_t column(const std::function<doc_t(int)> &fn) {
  return the<L(With)>([=](int column, auto &) { return fn(column); });
}

doc_t nesting(const std::function<doc_t(int)> &fn) {
  return the<L(With)>([=](int, auto &prefix) { return fn(prefix.size()); });
}

doc_t align(const doc_t &doc) {
  return the<L(With)>([=](int c, auto &p) { return nest(c - p.size(), doc); });
}

doc_t hang(int n, const doc_t &doc) { return align(nest(n, doc)); }

doc_t indent(int n, const doc_t &doc) { return hang(n, txt(spaces(n)) + doc); }

doc_t width(const doc_t &doc, const std::function<doc_t(int)> &fn) {
  return column(
      [=](int l) { return doc + column([=](int r) { return fn(r - l); }); });
}

template <class DDD> static doc_t cat_with(DDD &&ddd, const list_t<doc_t> &ds) {
  if (auto dds = as<L(Some)>(ds))
    return fold_left(ddd, head(dds), tail(dds));
  return empty();
}

doc_t vsep(const list_t<doc_t> &ds) {
  return cat_with([](auto &s, auto &d) { return s + line() + d; }, ds);
}

doc_t cat(const list_t<doc_t> &ds) { return cat_with(operator+, ds); }

//

using command_t =
    sum_t<the_t<L(Print), std::string>, the_t<L(Linefeed), std::string>>;

//

void output(const std::function<void()> &output_line,
            const std::function<void(const std::string &)> &output_text,
            const list_t<command_t, lazy_t> &commands) {
  for_each(
      [&](const command_t &command) {
        if (auto print = as<L(Print)>(command)) {
          output_text(it(print).c_str());
        } else if (auto linefeed = as<L(Linefeed)>(command)) {
          output_line();
          output_text(it(linefeed).c_str());
        }
      },
      commands);
}

bool fits(int max_cols, int used_cols,
          const list_t<command_t, lazy_t> &commands) {
  if (max_cols < used_cols)
    return false;
  if (auto ccs = as<L(Some)>(commands))
    if (auto print = as<L(Print)>(head(ccs)))
      return fits(max_cols, used_cols + it(print).size(), tail(ccs));
  return true;
}

list_t<command_t, lazy_t>
layout(int max_cols, int used_cols,
       const list_t<product_t<the_t<std::string>, the_t<doc_t>>> &docs) {
  return thunk([=]() {
    if (auto dds = as<L(Some)>(docs)) {
      auto &first = head(dds);
      auto &prefix = get<std::string>(first);
      auto &body = get<doc_t>(first);
      auto &rest = tail(dds);
      if (auto lazy = as<L(Lazy)>(body))
        return layout(max_cols, used_cols,
                      cons(product(the(prefix), the(it(lazy))), rest));
      if (auto join = as<L(Join)>(body))
        return layout(max_cols, used_cols,
                      cons(product(the(prefix), the(lhs(join))),
                           cons(product(the(prefix), the(rhs(join))), rest)));
      if (auto nest = as<L(Nest)>(body))
        return layout(max_cols, used_cols,
                      cons(product(the(prefix + get<L(Prefix)>(nest)),
                                   the(get<doc_t>(nest))),
                           rest));
      if (auto text = as<L(String)>(body)) {
        auto &str = it(text);
        return cons(command_t(the<L(Print)>(str)),
                    layout(max_cols, used_cols + str.size(), rest));
      }
      if (auto text = as<L(Literal)>(body)) {
        std::string str = it(text);
        return cons(command_t(the<L(Print)>(str)),
                    layout(max_cols, used_cols + str.size(), rest));
      }
      if (auto text = as<L(Char)>(body))
        return cons(command_t(the<L(Print)>(std::string(1, it(text)))),
                    layout(max_cols, used_cols + 1, rest));
      if (as<L(Line)>(body))
        return cons(command_t(the<L(Linefeed)>(prefix)),
                    layout(max_cols, prefix.size(), rest));
      if (auto choice = as<L(Choice)>(body)) {
        auto wide =
            layout(max_cols, used_cols,
                   cons(product(the(prefix), the(get<L(Wide)>(choice))), rest));
        if (max_cols <= 0 || fits(max_cols, used_cols, wide))
          return wide;
        return layout(
            max_cols, used_cols,
            cons(product(the(prefix), the(get<L(Narrow)>(choice))), rest));
      }
      if (auto with = as<L(With)>(body))
        return layout(
            max_cols, used_cols,
            cons(product(the(prefix), the(it(with)(used_cols, prefix))), rest));
      UNEXPECTED();
    }
    return list_t<command_t, lazy_t>(none);
  });
}

std::string render(int max_cols, const doc_t &d) {
  std::string result;
  output([&]() { result += '\n'; }, [&](auto &txt) { result += txt; },
         layout(max_cols, 0, list(product(the(std::string("")), the(d)))));
  return result;
}
