/*
 * stream_provider.cpp
 *
 *  Created on: May 9, 2014
 *      Author: Julian Kranz
 */

#include <bjutil/binary/stream_provider.h>
#include <fcntl.h>
#include <string>
#include <tuple>

using std::make_tuple;

stream_provider::stream_provider(FILE *f) {
  if(f == NULL)
    throw string("Invalid file");

  size_t chunk = 32;
  size_t data_length = 0;
  size = 4 * chunk;
  data = (uint8_t*)malloc(size);
  while(!feof(f)) {
    if(data_length + chunk > size) {
      size <<= 1;
      data = (uint8_t*)realloc(data, size);
    }
    data_length += fread(data + data_length, 1, chunk, f);
  }
  fclose(f);
  size = data_length;
}

stream_provider::~stream_provider() {
  free(data);
}

tuple<bool, binary_provider::entry_t> stream_provider::entry(string symbol) {
  if(symbol == "main") {
    entry_t entry;
    entry.address = 0;
    entry.offset = 0;
    entry.size = 0;
    return make_tuple(true, entry);
  }
  return binary_provider::entry(symbol);
}

binary_provider::entry_t stream_provider::bin_range() {
  entry_t range;

  range.address = 0;
  range.offset = 0;
  range.size =  size;

  return range;
}

binary_provider::data_t stream_provider::get_data() {
  data_t data;
  data.data = this->data;
  data.size = this->size;
  return data;
}
