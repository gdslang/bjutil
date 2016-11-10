/*
 * direct_provider.h
 *
 *  Created on: May 9, 2014
 *      Author: Julian Kranz
 */

#pragma once
#include "binary_provider.h"

class direct_provider : public binary_provider {
private:
  uint8_t *data = nullptr;
  size_t size;

public:
  direct_provider(uint8_t *data, size_t size);
  direct_provider(direct_provider const&) = delete;
  direct_provider(direct_provider &&other) : binary_provider(std::move(other)) {
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
  }
  direct_provider& operator=(direct_provider const&) = delete;
  direct_provider& operator=(direct_provider &&other) {
    binary_provider::operator=(std::move(other));
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
    return *this;
  }
  virtual ~direct_provider();

  virtual tuple<bool, entry_t> symbol(string symbol_name) const;
  virtual entry_t bin_range();
  virtual data_t get_data() const;
};
