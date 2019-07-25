//
// Created by shrum on 17.06.19.
//

#include "names_resolver.h"

std::ostream &operator<<(std::ostream &o, const rbg::NamesResolver &res) {
  o << "<NamesResolver with mapping {";
  for (rbg::name_id_t i = 0; i < res.size() - 1; i++) {
    o << i << ": " << res.Name(i) << ", ";
  }
  o << res.size() - 1 << ": " << res.Name(res.size() - 1) << "}>";
  return o;
}
