/*
 * stream_provider.h
 *
 *  Created on: May 9, 2014
 *      Author: Julian Kranz
 */

#pragma once

#include "binary_provider.h"
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <tuple>

class stream_provider : public binary_provider {
private:
  uint8_t *data = nullptr;
  size_t size;

public:
  stream_provider(FILE *f);
  stream_provider(stream_provider const&) = delete;
  stream_provider(stream_provider &&other) : binary_provider(std::move(other)) {
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
  }
  stream_provider& operator=(stream_provider const&) = delete;
  stream_provider& operator=(stream_provider &&other) {
    binary_provider::operator=(std::move(other));
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
    return *this;
  }
  virtual ~stream_provider();

  virtual tuple<bool, entry_t> symbol(string symbol_name) const;
  virtual entry_t bin_range();
  virtual data_t get_data() const;
};
