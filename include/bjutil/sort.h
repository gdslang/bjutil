/*
 * sort.h
 *
 *  Created on: May 5, 2015
 *      Author: Julian Kranz
 */

#pragma once

#include <algorithm>
#include <vector>

template<std::size_t I = 0, typename U, typename ... Tp>
static inline typename std::enable_if<I == sizeof...(Tp), void>::type
build(U &v, std::tuple<Tp...>& t)
{}

template<std::size_t I = 0, typename U, typename... Tp>
static inline typename std::enable_if<I < sizeof...(Tp), void>::type
build(U &v, std::tuple<Tp...>& t) {
  v.push_back(std::get<I>(t));
  build<I + 1, U, Tp...>(v, t);
}

template<std::size_t I = 0, typename U, typename ... Tp>
static inline typename std::enable_if<I == sizeof...(Tp), void>::type
tuplefy(U &v, std::tuple<Tp...>& t)
{}

template<std::size_t I = 0, typename U, typename... Tp>
static inline typename std::enable_if<I < sizeof...(Tp), void>::type
tuplefy(U &v, std::tuple<Tp...>& t) {
  std::get<I>(t) = v[I];
  tuplefy<I + 1, U, Tp...>(v, t);
}

template<typename _Compare, typename ... _Elements>
std::tuple<typename std::__decay_and_strip<_Elements>::__type...> tsortc(_Compare __comp, _Elements&&... __args) {
  typedef std::tuple<typename std::__decay_and_strip<_Elements>::__type...> __result_type;
  __result_type tup = __result_type(std::forward<_Elements>(__args)...);
  __result_type tup_sorted;
  std::vector<typename std::tuple_element<0, __result_type >::type> v;
  build(v, tup);
  sort(v.begin(), v.end(), __comp);
  tuplefy(v, tup_sorted);
  return tup_sorted;
}

template<typename ... _Elements>
std::tuple<typename std::__decay_and_strip<_Elements>::__type...> tsort(_Elements&&... __args) {
  typedef std::tuple<typename std::__decay_and_strip<_Elements>::__type...> __result_type;
  return tsortc(std::less<typename std::tuple_element<0, __result_type >::type>(), __args...);
}
