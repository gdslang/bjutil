/*
 * binary_provider.h
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <map>

using std::string;
using std::tuple;
using std::map;

class binary_provider {
public:
  virtual ~binary_provider();

  struct entry_t {
    size_t offset;
    size_t address;
    size_t size;

    entry_t(size_t offset, size_t address, size_t size) :
        offset(offset), address(address), size(size) {
    }

    entry_t() :
        entry_t(0, 0, 0) {
    }
  };

  virtual tuple<bool, entry_t> entry(string symbol);
  void add_entry(string symbol, entry_t entry);

  struct bin_range_t {
    size_t offset;
    size_t address;
    size_t size;
  };

  /**
   * Get the code section within the binary data (e.g. the .text section in an elf file)
   */
  virtual bin_range_t bin_range() = 0;

  struct data_t {
    uint8_t *data;
    size_t size;
  };

  virtual data_t get_data() = 0;

  virtual std::tuple<bool, uint64_t> deref(void *address);
private:
  map<string, entry_t> symbols;
};
