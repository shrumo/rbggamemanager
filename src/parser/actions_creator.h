//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_ACTIONS_CREATOR_H
#define RBGGAMEMANAGER_ACTIONS_CREATOR_H

#include <memory>
#include <actions/actions.h>
#include <rbgParser/src/game_move.hpp>

std::unique_ptr<rbg::Action> CreateAction(const rbg_parser::game_move &move, uint id);

#endif //RBGGAMEMANAGER_ACTIONS_CREATOR_H
