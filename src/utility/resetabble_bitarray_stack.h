//
// Created by shrum on 15.01.18.
//

#ifndef RBGGAMEMANAGER_FAST_RESET_VECTOR_H
#define RBGGAMEMANAGER_FAST_RESET_VECTOR_H

#include <vector>
#include <limits>
#include <unordered_map>
#include <cassert>

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

//    ResettableBitArray(ResettableBitArray&&) = delete;
//    ResettableBitArray(const ResettableBitArray&) = delete;

    bool operator[](const uint index) const {
      return data_[index] == threshold_;
    }

    void set(const uint index) {
      data_[index] = threshold_;
    }

    void reset() {
      if (threshold_ == std::numeric_limits<unsigned int>::max()) {
        std::fill(data_.begin(), data_.end(), 0);
        threshold_ = 1;
      } else
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
    explicit ResettableBitArrayStack(uint bit_array_size)
        : bit_array_size_(bit_array_size), bit_arrays_{}, depth_(0) {
      Push();
    }

    void Push() {
      if (depth_ == bit_arrays_.size()) {
        bit_arrays_.emplace_back(bit_array_size_, false);
      } else {
        bit_arrays_[depth_].reset();
      }
      depth_++;
    }

    void Pop() {
      depth_--;
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

    uint bit_array_size() const {
      return bit_array_size_;
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
    ResetableBitArrayStackChunk(ResettableBitArrayStack *parent, uint begin_index, uint size)
        : parent_(parent), begin_index_(begin_index), size_(size) {
      assert(begin_index + size <= parent_->bit_array_size());
    }

    bool operator[](uint index) const {
      assert(index < size_);
      return parent_->top_array()[begin_index_ + index];
    }

    void set(uint index) {
      assert(index < size_);
      parent_->top_array().set(begin_index_ + index);
    }

    uint size() const {
      return size_;
    }

  private:
    ResettableBitArrayStack *parent_;
    uint begin_index_;
    uint size_;
  };
}


#endif //RBGGAMEMANAGER_FAST_RESET_VECTOR_H
