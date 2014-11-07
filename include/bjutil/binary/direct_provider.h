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
  uint8_t *data = NULL;
  size_t size;

public:
  direct_provider(uint8_t *data, size_t size);
  virtual ~direct_provider();

  virtual tuple<bool, entry_t> entry(string symbol);
  virtual bin_range_t bin_range();
  virtual data_t get_data();
};
