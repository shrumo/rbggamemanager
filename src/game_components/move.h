//
// Created by shrum on 21/02/18.
//

#ifndef RBGGAMEMANAGER_MOVE_H
#define RBGGAMEMANAGER_MOVE_H

#include <vector>

class BlockApplication {
public:
  BlockApplication(std::size_t applied_x, std::size_t applied_y,
                   std::size_t block_id)
      : applied_x_(applied_x), applied_y_(applied_y),
        block_id_(block_id) {}

  std::size_t x() const {
    return applied_x_;
  }

  std::size_t y() const {
    return applied_y_;
  }

  std::size_t id() const {
    return block_id_;
  }

  bool operator!=(const BlockApplication &b) const {
    return x() != b.x() || y() != b.y() || block_id_ != b.block_id_;
  }

  bool operator==(const BlockApplication &b) const {
    return x() == b.x() && y() == b.y() && block_id_ == b.block_id_;
  }

private:
  std::size_t applied_x_, applied_y_;
  std::size_t block_id_;
};

class Move {
public:
  Move() = default;

  template<typename ...Args>
  void AddBlock(Args &&... args) {
    blocks_.emplace_back(std::forward<Args>(args)...);
  }

  void PopBlock() {
    blocks_.pop_back();
  }

  const std::vector<BlockApplication> &blocks() const {
    return blocks_;
  }

  bool operator==(const Move &b) const {
    const auto &a_blocks = blocks();
    const auto &b_blocks = b.blocks();
    if (a_blocks.size() != b_blocks.size())
      return false;
    for (std::size_t i = 0; i < a_blocks.size(); i++) {
      if (a_blocks[i] != b_blocks[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Move &b) const {
    return !(*this == b);
  }

private:
  std::vector<BlockApplication> blocks_;
};

namespace std {
  template<>
  struct hash<Move> {
    std::size_t operator()(const Move &m) const {
      std::size_t result = 0;
      for (const auto &block : m.blocks()) {
        result *= 2579;
        result += (block.y() * 31 + block.x()) * 83 + block.id();
      }
      return result;
    }
  };
}

#endif //RBGGAMEMANAGER_MOVE_H
