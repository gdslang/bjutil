/*
 * file_provider.h
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#pragma once

#include <bjutil/binary/stream_provider.h>

class file_provider : public stream_provider {
public:
  file_provider(char const *file);
  file_provider(char *buffer, size_t size);
  virtual ~file_provider();
};
