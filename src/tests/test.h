//
// Created by shrum on 21.05.19.
//

#ifndef RBGGAMEMANAGER_TEST_H
#define RBGGAMEMANAGER_TEST_H

#define ASSERT(condition)                             \
{                                                                   \
  if( !( condition ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( __PRETTY_FUNCTION__ )  \
    );                                                              \
  }                                                                 \
}

#endif //RBGGAMEMANAGER_TEST_H
