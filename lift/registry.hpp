#pragma once

#include <boost/function.hpp>
#include <exception>
#include <map>
#include <string>

namespace lift {

template <typename T, typename K = std::string>
class registry {
 public:
  typedef typename std::map<K, boost::function<T*()> >::const_iterator
      const_iterator;
  typedef
      typename std::map<K, boost::function<T*()> >::const_pointer const_pointer;
  typedef typename std::map<K, boost::function<T*()> >::const_reference
      const_reference;
  typedef typename std::map<K, boost::function<T*()> >::const_reverse_iterator
      const_reverse_iterator;
  typedef typename std::map<K, boost::function<T*()> >::difference_type
      difference_type;
  typedef typename std::map<K, boost::function<T*()> >::iterator iterator;
  typedef typename std::map<K, boost::function<T*()> >::key_compare key_compare;
  typedef typename std::map<K, boost::function<T*()> >::key_type key_type;
  typedef typename std::map<K, boost::function<T*()> >::mapped_type mapped_type;
  typedef typename std::map<K, boost::function<T*()> >::pointer pointer;
  typedef typename std::map<K, boost::function<T*()> >::reference reference;
  typedef typename std::map<K, boost::function<T*()> >::reverse_iterator
      reverse_iterator;
  typedef typename std::map<K, boost::function<T*()> >::size_type size_type;
  typedef
      typename std::map<K, boost::function<T*()> >::value_compare value_compare;
  typedef typename std::map<K, boost::function<T*()> >::value_type value_type;
  class undefined : public std::exception {};
  static iterator begin() { return _().begin(); }
  static void clear() { _().clear(); }
  static size_type count(const key_type& key) { return _().count(key); }
  static bool empty() { return _().empty(); }
  static std::pair<iterator, iterator> equal_range(const key_type& key) {
    return _().equal_range(key);
  }
  static iterator end() { return _().end(); }
  static iterator erase(iterator where) { return _().erase(where); }
  static iterator erase(iterator first, iterator last) {
    return _().erase(first, last);
  }
  static size_type erase(const key_type& key) { return _().erase(key); }
  static iterator find(const key_type& key) { return _().find(key); }
  static std::pair<iterator, bool> insert(const value_type& value) {
    return _().insert(value);
  }
  static iterator insert(iterator where, const value_type& value) {
    return _().insert(where, value);
  }
  template <typename I>
  static iterator insert(I first, I last) {
    return _().insert(first, last);
  }
  static key_compare key_comp() { return _().key_comp(); }
  static iterator lower_bound(const key_type& key) {
    return _().lower_bound(key);
  }
  static reverse_iterator rbegin() { return _().rbegin(); }
  static reverse_iterator rend() { return _().rend(); }
  static size_type size() { return _().size(); }
  static iterator upper_bound(const key_type& key) {
    return _().upper_bound(key);
  }
  static value_compare value_comp() { return _().value_comp(); }
  static T* make(const key_type& key) {
    iterator i = find(key);
    if (i != end())
      return i->second();
    else
      throw undefined();
  }
  static bool can_make(const key_type& key) { return find(key) != end(); }

 private:
  ~registry();
  static std::map<K, boost::function<T*()> >& _() {
    static std::map<K, boost::function<T*()> > _;
    return _;
  }
};

template <typename T>
class new_default {
 public:
  T* operator()() const { return new T(); }
};

template <typename T, typename U>
class new_convert {
 public:
  new_convert(const U& u) : _(u) {}
  T* operator()() const { return new T(_); }

 private:
  new_convert();
  U _;
};

template <typename T, typename U, typename K = std::string>
class registrar {
 public:
  explicit registrar(const K& k) {
    registry<T, K>::insert(
        typename registry<T, K>::value_type(k, new_default<U>()));
  }
  template <typename V>
  registrar(const K& k, const V& v) {
    registry<T, K>::insert(
        typename registry<T, K>::value_type(k, new_convert<U, V>(v)));
  }
};
}
