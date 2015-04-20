/*
 * binary_provider.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */


#include <bjutil/binary/binary_provider.h>
#include <tuple>

using std::make_tuple;

binary_provider::~binary_provider() {
}

tuple<bool, binary_provider::entry_t> binary_provider::symbol(string symbol_name) const {
  auto it = symbols.find(symbol_name);
  if(it == symbols.end())
    return make_tuple(false, binary_provider::entry_t());
  else
    return make_tuple(true, it->second);
}

void binary_provider::add_symbol(string symbol_name, entry_t entry) {
  symbols[symbol_name] = entry;
}

std::tuple<bool, uint64_t> binary_provider::deref(void* address) {
  return make_tuple(false, 0);
}
