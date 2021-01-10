//
// Created by shrum on 18/07/19.
//

#ifndef RBGGAMEMANAGER_MODIFYING_APPLICATION_H
#define RBGGAMEMANAGER_MODIFYING_APPLICATION_H

#include <game_description/board.h>
#include <game_description/names_resolver.h>

namespace rbg {
class GameState;
enum class ActionTypeTrait {
  kCheck,
  kApplication,
  kModifier,
  kSwitch,
  kSpecial,
};

class ModifyingApplication {
 public:
  explicit ModifyingApplication(uint index) : index_(index) {}

  virtual name_id_t Apply(GameState *state) const = 0;
  virtual void Revert(GameState *state, name_id_t) const = 0;

  vertex_id_t modified_position(GameState *state) const;

  uint index() const { return index_; }

 public:
  void PushVisitedStackAndAddToApplied(GameState *state) const;
  void PopVisitedStackAndAddToApplied(GameState *state) const;

  void RegisterMove(GameState *state) const;

  uint index_;
};
}  // namespace rbg

#endif  // RBGGAMEMANAGER_MODIFYING_APPLICATION_H
