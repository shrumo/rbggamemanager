//
// Created by shrum on 01/02/18.
//

#ifndef RBGGAMEMANAGER_UTILS_H
#define RBGGAMEMANAGER_UTILS_H

#include <string>
#include <sstream>
#include "../game_components/search_context.h"
#include "../game_components/name_resolver.h"

class message;

std::string move_data_string(const move& move);
move decode_move(const message& message);

#endif //RBGGAMEMANAGER_UTILS_H
