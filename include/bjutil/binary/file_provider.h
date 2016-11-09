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
  file_provider(FILE *f);
  file_provider(file_provider const&) = default;
  file_provider(file_provider &&) = default;
  file_provider& operator=(file_provider const&) = default;
  file_provider& operator=(file_provider &&) = default;
  virtual ~file_provider();
};
