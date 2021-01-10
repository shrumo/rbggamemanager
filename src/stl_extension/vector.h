//
// Created by shrum on 16/07/19.
//

#ifndef RBGGAMEMANAGER_VECTOR_H
#define RBGGAMEMANAGER_VECTOR_H

#include <vector>

namespace std_ext {
template <typename T>
class rigid_vector {
 public:
  static_assert(std::is_trivial<T>::value);

  explicit rigid_vector(std::size_t size) : data_(size), position_(0) {}

  void push_back(T e) {
    data_[position_] = e;
    position_++;
  }

  void pop_back() { position_--; }

  void reserve(std::size_t size) { data_.resize(size); }

  T& operator[](std::size_t i) { return data_[i]; }

  T operator[](std::size_t i) const { return data_[i]; }

  std::size_t size() const { return position_; }

 private:
  std::vector<T> data_;
  std::size_t position_;
};
}  // namespace std_ext

#endif  // RBGGAMEMANAGER_VECTOR_H
