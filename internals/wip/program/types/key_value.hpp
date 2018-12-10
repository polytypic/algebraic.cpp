#pragma once

#include "core/types.hpp"

template <class Key, class Value>
using key_value_t = product_t<the_t<L(Key), Key>, the_t<L(Value), Value>>;

template <class KV> auto &key(const KV &kv) { return get<L(Key)>(kv); }

template <class KV> auto &value(const KV &kv) { return get<L(Value)>(kv); }

template <class K, class V> auto key_value(K &&k, V &&v) {
  return product(the<L(Key)>(std::forward<K>(k)),
                 the<L(Value)>(std::forward<V>(v)));
}
