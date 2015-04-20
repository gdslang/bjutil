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
  uint8_t *data = NULL;
  size_t size;

public:
  stream_provider(FILE *f);
  virtual ~stream_provider();

  virtual tuple<bool, entry_t> symbol(string symbol_name) const;
  virtual entry_t bin_range();
  virtual data_t get_data() const;
};
