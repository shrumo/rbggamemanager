//
// Created by shrum on 15.01.18.
//

#ifndef RBGGAMEMANAGER_FAST_RESET_VECTOR_H
#define RBGGAMEMANAGER_FAST_RESET_VECTOR_H

#include <vector>
#include <limits>
#include <unordered_map>

namespace rbg {
  // This is vector of booleans supporting fast reset function. Reset
  // is done in constant time, independent from the size of the vector.
  // Example:
  //   ResettableBitArray vec(10, false);
  //   vec[5] = true;
  //   vec.reset();
  class ResettableBitArray {
  public:
    ResettableBitArray(uint size, bool value)
        : data_(size, (unsigned int) value) {
      threshold_ = 1;
    }

    bool operator[](const uint index) const {
      return data_[index] >= threshold_;
    }

    void set(const uint index) {
      data_[index] = threshold_;
    }

    void reset() {
      if (threshold_ == std::numeric_limits<unsigned int>::max()) {
        std::fill(data_.begin(), data_.end(), 0);
        threshold_ = 1;
      }
      threshold_++;
    }

    void resize(uint new_size) {
      data_.resize(new_size);
    }

    uint size() const {
      return data_.size();
    }

  private:
    std::vector<unsigned int> data_;
    unsigned int threshold_;
  };

  class ResettableBitArrayStack {
  public:
    ResettableBitArrayStack(uint bit_array_size)
        : bit_array_size_(bit_array_size), bit_arrays_{}, depth_(0) {}

    void Push() {
      if (depth_ == bit_arrays_.size()) {
        bit_arrays_.emplace_back(bit_array_size_, false);
      } else {
        bit_arrays_[depth_].reset();
      }
      depth_++;
    }

    void RevertToDepth(uint depth) {
      depth_ = depth;
    }

    uint depth() const {
      return depth_;
    }

    void reset() {
      depth_ = 0;
      Push();
    }

    ResettableBitArray &top_array() {
      return bit_arrays_[depth_ - 1];
    }

  private:
    uint bit_array_size_;
    std::vector<ResettableBitArray> bit_arrays_;
    uint depth_;
  };

  class ResetableBitArrayStackChunk {
  public:
    ResetableBitArrayStackChunk(ResettableBitArrayStack &parent, uint begin_index, uint end_index)
        : parent_(parent), begin_index_(begin_index), end_index_(end_index) {}

    bool operator[](uint index) const {
      return parent_.top_array()[begin_index_ + index];
    }

    void set(uint index) {
      parent_.top_array().set(begin_index_ + index);
    }

    uint size() const {
      return end_index_ - begin_index_ + 1;
    }

  private:
    ResettableBitArrayStack &parent_;
    uint begin_index_;
    uint end_index_;
  };
}


#endif //RBGGAMEMANAGER_FAST_RESET_VECTOR_H