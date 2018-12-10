#pragma once

#include "util/meta.hpp"
#include "util/panic.hpp"

#include <atomic>
#include <type_traits>
#include <utility>

namespace detail {

struct counted_base {
  std::atomic<int> m_count;
  counted_base() : m_count(1) {}
};

template <class V> struct counted {
  ~counted() {
    if (m_ptr && 0 == --(m_ptr->m_count))
      delete m_ptr;
  }

  explicit counted(V *ptr) : m_ptr(ptr) {}

  counted(counted &&that) : m_ptr(that.m_ptr) { that.m_ptr = nullptr; }
  counted(const counted &that) : m_ptr(that.m_ptr) { m_ptr->m_count++; }

  const V &deref() const { return *m_ptr; }

  counted &operator=(counted &) = delete;
  V *m_ptr;
};

} // namespace detail
