#pragma once

template <class X> const X &identity(const X &x) { return x; }

template <class X, class... Xs> auto constant(X &&x) {
  return [=](Xs...) { return x; };
}
