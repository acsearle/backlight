#pragma once

#include <list>

namespace lift
{

  class singleton_list_default_key {};

  template<typename T, typename K = singleton_list_default_key> class singleton_list
  {
  public:
    typedef typename std::list<T>::iterator iterator;
    typedef typename std::list<T>::reverse_iterator reverse_iterator;
    typedef typename std::list<T>::value_type value_type;
    typedef typename std::list<T>::reference reference;
    typedef typename std::list<T>::const_iterator const_iterator;
    typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;
    typedef typename std::list<T>::const_reference const_reference;
    static bool empty() { return _().empty(); }
    static iterator begin() { return _().begin(); }
    static iterator end() { return _().end(); }
    static reverse_iterator rbegin() { return _().rbegin(); }
    static reverse_iterator rend() { return _().rend(); }
    static reference back() { return _().back(); }
    static void push_back(const value_type& v) { _().push_back(v); }
    static void remove(const value_type& v) { _().remove(v); }
  private:
    static std::list<T>& _() { static std::list<T> _; return _; }
    singleton_list();
  };
  
}
