//
// Created by shrum on 10.06.19.
//

#include <game_description/names_resolver.h>
#include <cassert>
#include <iostream>

using namespace rbg;

int main() {
  NamesResolver resolver;
  assert(resolver.size() == 0);
  resolver.AddName("cat");
  assert(resolver.size() == 1);
  assert(resolver.AddName("cat") == 0 &&
         "The indices given to the names should range from 0 to size and be in order of adding.");
  resolver.AddName("dog");
  assert(resolver.size() == 2);
  resolver.AddName("cat");
  assert(resolver.size() == 2);
  assert(resolver.contains("dog"));
  assert(!resolver.contains("mouse"));
  const NamesResolver resolver_const = resolver;
  assert(resolver.AddName("dog") == 1);
  std::cout << resolver << std::endl;
}