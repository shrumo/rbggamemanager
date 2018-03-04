//
// Created by shrum on 15.01.18.
//

#ifndef RBGGAMEMANAGER_FAST_RESET_VECTOR_H
#define RBGGAMEMANAGER_FAST_RESET_VECTOR_H

#include <vector>
#include <limits>

class FastBitVector {
public:
  FastBitVector(std::size_t size, bool value)
      : data_(size, (unsigned int) value) {
    threshold_ = 1;
  }

  bool operator[](std::size_t index) const {
    return data_[index] >= threshold_;
  }

  void set(std::size_t index) {
    data_[index] = threshold_;
  }

  void reset() {
    if (threshold_ == std::numeric_limits<unsigned int>::max()) {
      std::fill(data_.begin(), data_.end(), 0);
      threshold_ = 1;
    }
    threshold_++;
  }

  void resize(std::size_t new_size) {
    data_.resize(new_size);
  }

  std::size_t size() const {
    return data_.size();
  }

private:
  std::vector<unsigned int> data_;
  unsigned int threshold_;
};


#endif //RBGGAMEMANAGER_FAST_RESET_VECTOR_H
