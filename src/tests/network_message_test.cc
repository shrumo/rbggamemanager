//
// Created by shrum on 02.07.19.
//

#include <networking/message.h>
#include <iostream>
#include <cassert>

using namespace rbg;

int main() {
  Message message("long message");
  assert(message.body_length() == 13);
  assert(message.length() == 13 + rbg::kHeaderLength);
  assert(std::string(message.body_ptr()) == "long message");
  std::cout << message.data_ptr() << std::endl;
}