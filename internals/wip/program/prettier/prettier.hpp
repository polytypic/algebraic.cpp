#pragma once

#include "core/types.hpp"

#include "types/lhs_rhs.hpp"
#include "types/list_strict.hpp"

#include <functional>
#include <string>

using doc_t = fix_t<tycon_t<
    seq_t<Rec>,
    shared_t<sum_t<
        the_t<L(Lazy), lazy_t<Rec>>, the_t<L(Line), Rec>,
        the_t<L(Join), lhs_rhs_t<Rec>>,
        the_t<L(Nest), product_t<the_t<L(Prefix), std::string>, the_t<Rec>>>,
        the_t<L(String), std::string>, the_t<L(Literal), const char *>,
        the_t<L(Char), char>,
        the_t<L(Choice), product_t<the_t<L(Wide), Rec>, the_t<L(Narrow), Rec>>>,
        the_t<L(With), std::function<Rec(int, const std::string &)>>>>>>;

extern doc_t txt(char txt);
extern doc_t txt(const char *txt);
extern doc_t txt(const std::string &txt);

extern doc_t operator+(const doc_t &lhs, const doc_t &rhs);

extern doc_t &space();
extern doc_t &colon();
extern doc_t &comma();

extern doc_t &dquote();
extern doc_t &squote();

extern doc_t &lparen();
extern doc_t &rparen();
extern doc_t &lbrace();
extern doc_t &rbrace();
extern doc_t &lbracket();
extern doc_t &rbracket();

extern doc_t &empty();
extern doc_t &line();
extern doc_t &linebreak();
extern doc_t &softline();
extern doc_t &softbreak();

extern doc_t &commaline();
extern doc_t &colonline();

extern lhs_rhs_t<doc_t> &parens();
extern lhs_rhs_t<doc_t> &braces();
extern lhs_rhs_t<doc_t> &brackets();

extern doc_t delay(const std::function<doc_t()> &th);
extern doc_t nest(int n, const doc_t &doc);
extern doc_t nestBy(const std::string &prefix, const doc_t &doc);
extern doc_t choice(const doc_t &wide, const doc_t &narrow);
extern doc_t group(const doc_t &doc);
extern doc_t gnest(int n, const doc_t &doc);
extern doc_t column(const std::function<doc_t(int)> &fn);
extern doc_t nesting(const std::function<doc_t(int)> &fn);
extern doc_t indent(int n, const doc_t &doc);
extern doc_t hang(int n, const doc_t &doc);
extern doc_t align(const doc_t &doc);
extern doc_t width(const doc_t &doc, const std::function<doc_t(int)> &fn);
extern doc_t vsep(const list_t<doc_t> &ds);
extern doc_t cat(const list_t<doc_t> &ds);

extern std::string render(int max_cols, const doc_t &doc);
