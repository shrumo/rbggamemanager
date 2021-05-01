//
// Created by shrum on 11/07/19.
//

#ifndef RBGGAMEMANAGER_RANGE_H
#define RBGGAMEMANAGER_RANGE_H

#include <unordered_map>

namespace std_ext {
template <typename Iterator> class range {
public:
  range(Iterator begin, Iterator end, std::size_t size)
      : begin_(begin), end_(end), size_(size) {}

  range(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    size_ = end - begin;
  }

  Iterator begin() { return begin_; }

  Iterator end() { return end_; }

  size_t size() const { return size_; }

private:
  Iterator begin_, end_;
  std::size_t size_;
};
} // namespace std_ext

#endif // RBGGAMEMANAGER_RANGE_H
