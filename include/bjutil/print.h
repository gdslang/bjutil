/*
 * print.h
 *
 *  Created on: Nov 24, 2014
 *      Author: Julian Kranz
 */

#pragma once
#include <set>
#include <string>
#include <functional>
#include <sstream>
#include <iostream>
#include <tuple>
#include <type_traits>

template<typename, typename, typename...>
class printer;

template<typename C, typename R, typename... E, typename std::enable_if<std::is_same<C, std::set<E...>>::value>::type* = nullptr>
std::ostream &operator<<(std::ostream &out, const printer<C, R, E...> &sp);;
template<typename C, typename R, typename... E, typename std::enable_if<std::is_same<C, std::map<E...>>::value>::type* = nullptr>
std::ostream &operator<<(std::ostream &out, const printer<C, R, E...> &sp);

template<typename COLL_T, typename R, typename... E>
class printer {
  friend std::ostream &operator<< <>(std::ostream &out, const printer<COLL_T, R, E...> &sp);
public:
  typedef std::tuple<std::function<R(const E &e)>...> printers_t;
private:
  const COLL_T &coll;
  printers_t printers;
public:
  printer(const COLL_T &coll, printers_t printers) : coll(coll), printers(printers) {
  }
};


template<typename X, typename R, typename... T>
printer<X, R, T...> print(const X &coll, std::function<R(const T &e)>... printers) {
  return printer<X, R, T...>(coll, make_tuple(printers...));
}

template<typename X, typename Y>
printer<std::map<X, Y>, std::string, X, Y> print(const std::map<X, Y> &coll) {
  std::function<std::string(const X&)> default_kp = [](const X& k) -> std::string {
    std::stringstream ss;
    ss << k;
    return ss.str();
  };
  std::function<std::string(const Y&)> default_vp = [](const Y& v) -> std::string {
    std::stringstream ss;
    ss << v;
    return ss.str();
  };
  return print(coll, default_kp, default_vp);
}

template<typename X>
printer<std::set<X>, std::string, X> print(const std::set<X> &coll) {
  std::function<std::string(const X&)> default_ = [](const X& v) -> std::string {
    std::stringstream ss;
    ss << v;
    return ss.str();
  };
  return print(coll, default_);
}

template<typename C, typename R, typename... E, typename std::enable_if<std::is_same<C, std::set<E...>>::value>::type*>
std::ostream &operator<<(std::ostream &out, const printer<C, R, E...> &sp) {
  decltype(std::get<0>(sp.printers)) print = std::get<0>(sp.printers);
  bool first = true;
  out << "{";
  for(auto &e : sp.coll) {
    if(!first) out << ", ";
    else first = false;
    out << print(e);
  }
  out << "}";
  return out;
}

template<typename C, typename R, typename... E, typename std::enable_if<std::is_same<C, std::map<E...>>::value>::type*>
std::ostream &operator<<(std::ostream &out, const printer<C, R, E...> &sp) {
  decltype(std::get<0>(sp.printers)) key_print = std::get<0>(sp.printers);
  decltype(std::get<1>(sp.printers)) value_print = std::get<1>(sp.printers);
  bool first = true;
  out << "{";
  for(auto &e : sp.coll) {
    if(!first) out << ", ";
    else first = false;
    out << "(" << key_print(e.first) << " -> " << value_print(e.second) << ")";
  }
  out << "}";
  return out;
}
