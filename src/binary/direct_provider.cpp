/*
 * direct_provider.cpp
 *
 *  Created on: May 9, 2014
 *      Author: Julian Kranz
 */

#include <bjutil/binary/binary_provider.h>
#include <bjutil/binary/direct_provider.h>
#include <string>
#include <tuple>

using std::make_tuple;

direct_provider::direct_provider(uint8_t *data, size_t size) {
  this->data = data;
  this->size = size;
}

direct_provider::~direct_provider() {
  free(data);
}

tuple<bool, binary_provider::entry_t> direct_provider::symbol(string symbol_name) const {
  if(symbol_name != "main") return binary_provider::symbol(symbol_name);

  entry_t entry;

  entry.address = 0;
  entry.offset = 0;
  entry.size = 0;

  return make_tuple(true, entry);
}

binary_provider::entry_t direct_provider::bin_range() {
  entry_t range;

  range.address = 0;
  range.offset = 0;
  range.size = size;

  return range;
}

binary_provider::data_t direct_provider::get_data() const {
  data_t data;
  data.data = this->data;
  data.size = this->size;
  return data;
}
