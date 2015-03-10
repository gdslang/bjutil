/*
 * print.h
 *
 *  COLL_Treated on: Nov 24, 2014
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
#include <map>


template<typename, typename, typename...>
class printer;

//template<typename...Ts>
//using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));
//
//template<typename T, typename...Ts>
//using remove_t = tuple_cat_t<
//    typename std::conditional<
//        std::is_same<T, Ts>::value,
//        std::tuple<>,
//        std::tuple<Ts>
//    >::type...
//>;
//http://stackoverflow.com/questions/23855712/how-can-a-type-be-removed-from-a-template-parameter-pack

#define APPEND_OP(COLL, DEF) template<typename COLL_T, typename R, typename... E, \
  typename std::enable_if<std::is_same<COLL_T, std::COLL<E...>>::value>::type* DEF> \
  std::ostream &operator<<(std::ostream &out, const printer<COLL_T, R, E...> &sp)
#define APPEND_OP_FWD(COLL) APPEND_OP(COLL, = nullptr)
#define APPEND_OP_IMPL(COLL) APPEND_OP(COLL, )

APPEND_OP_FWD(set);
APPEND_OP_FWD(map);

template<typename COLL_T, typename R, typename... ELEM_T>
class printer {
  friend std::ostream &operator<< <>(std::ostream &out, const printer<COLL_T, R, ELEM_T...> &sp);
public:
  typedef std::tuple<std::function<R(const ELEM_T &e)>...> printers_t;
private:
  const COLL_T &coll;
  printers_t printers;
public:
  printer(const COLL_T &coll, printers_t printers) : coll(coll), printers(printers) {
  }
};

template<typename T>
std::function<std::string()> stream(const T &elem) {
  return [=]() {
    std::stringstream ss;
    ss << elem;
    return ss.str();
  };
}

template<typename T>
std::function<std::string()> stream_ptr(const T &elem) {
  return [=]() {
    std::stringstream ss;
    ss << *elem;
    return ss.str();
  };
}

template<typename T>
std::function<std::string(const T &elem)> stream() {
  return [](const T &elem) {
    return stream(elem)();
  };
}

template<typename T>
std::function<std::string(const T &elem)> stream_ptr() {
  return [](const T &elem) {
    return stream_ptr(elem)();
  };
}

template<typename X, typename R, typename ... ELEM_T>
printer<X, R, ELEM_T...> print(const X &coll, std::function<R(const ELEM_T &e)> ... printers) {
  return printer<X, R, ELEM_T...>(coll, make_tuple(printers...));
}

template<typename K_T, typename V_T>
printer<std::map<K_T, V_T>, std::string, K_T, V_T> print(const std::map<K_T, V_T> &coll) {
  return print(coll, stream<K_T>(), stream<V_T>());
}

template<typename ELEM_T>
printer<std::set<ELEM_T>, std::string, ELEM_T> print(const std::set<ELEM_T> &coll) {
  return print(coll, stream<ELEM_T>());
}

template<typename T>
std::function<std::string(const T &elem)> stream_printer() {
  return [](const T &elem) {
    return stream(print(elem))();
  };
}

APPEND_OP_IMPL(set) {
  decltype(std::get<0>(sp.printers)) &&print = std::get<0>(sp.printers);
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

APPEND_OP_IMPL(map) {
  decltype(std::get<0>(sp.printers)) &&key_print = std::get<0>(sp.printers);
  decltype(std::get<1>(sp.printers)) &&value_print = std::get<1>(sp.printers);
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
