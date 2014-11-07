/*
 * sliced_memory.h
 *
 *  Created on: Aug 8, 2014
 *      Author: jucs
 */

#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <vector>
#include <tuple>

struct slice {
  void *address;
  size_t size;
  std::string symbol;

  slice(void *address, size_t size, std::string symbol) :
      address(address), size(size), symbol(symbol) {
  }

  slice(void *address) :
      address(address), size(0), symbol("") {
  }

  slice() : slice(0) {
  }
};

class sliced_memory {
private:
  std::vector<slice> slices;
public:
  sliced_memory(std::vector<slice> slices);

  std::tuple<bool, slice> deref(void *address);
};
