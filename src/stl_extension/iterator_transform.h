//
// Created by shrum on 11/07/19.
//

#ifndef RBGGAMEMANAGER_ITERATOR_TRANSFORM_H
#define RBGGAMEMANAGER_ITERATOR_TRANSFORM_H

#include<iterator>

namespace std_ext {
  class identity {
  public:
    template<typename U>
    constexpr auto operator()(U&& v) const noexcept
    -> decltype(std::forward<U>(v))
    {
      return std::forward<U>(v);
    }
  };

  template<typename Iterator, typename UnaryFunction = identity>
  class transform_iterator : public Iterator {
    public:
      transform_iterator(Iterator it, UnaryFunction function = UnaryFunction()) : Iterator(it), function_(function) {}

      typename std::remove_reference<std::result_of_t<UnaryFunction(typename std::iterator_traits<Iterator>::value_type&)>>::type *operator->() { return &function_(Iterator::operator*()); }
      typename std::result_of<UnaryFunction(typename std::iterator_traits<Iterator>::value_type&)>::type& operator*() { return function_(Iterator::operator*()); }
    private:
      UnaryFunction function_;
    };
}

#endif //RBGGAMEMANAGER_ITERATOR_TRANSFORM_H
