//
// Created by shrum on 11/07/19.
//

#ifndef RBGGAMEMANAGER_UTILITY_H
#define RBGGAMEMANAGER_UTILITY_H

#include <utility>

#ifndef always_inline
#ifdef __GNUC__
#define always_inline __attribute__((always_inline))
#else
#define always_inline inline
#endif
#endif

namespace std_ext {
  template<typename A, typename B>
  class first_of_pair {
  public:
    A& operator()(std::pair<A,B>& a) const {
      return a.first;
    }
    const A& operator()(const std::pair<A,B>& a) const {
      return a.first;
    }
  };

  template<typename A, typename B>
  class second_of_pair {
  public:
    A& operator()(std::pair<A,B>& a) const {
      return a.second;
    }
    const A& operator()(const std::pair<A,B>& a) const {
      return a.second;
    }
  };
}

#endif //RBGGAMEMANAGER_UTILITY_H
