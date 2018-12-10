#pragma once

#include "std/type_traits.hpp"
#include <utility>

template <class Action> struct finally_t {
  ~finally_t() { m_action(); }
  template <class Act>
  finally_t(Act &&act) : m_action(std::forward<Act>(act)) {}

private:
  Action m_action;
};

template <class Act> auto finally(Act &&act) {
  return finally_t<std::remove_cvref_t<Act>>(std::forward<Act>(act));
}
