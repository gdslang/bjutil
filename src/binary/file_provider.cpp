/*
 * file_provider.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#include <fcntl.h>
#include <bjutil/binary/file_provider.h>
#include <bjutil/binary/stream_provider.h>

file_provider::file_provider(const char *file) :
    stream_provider(fopen(file, "r")) {
}

file_provider::file_provider(char* buffer, size_t size) :
  stream_provider(fmemopen(buffer, size, "r")) {
}

file_provider::~file_provider() {
}
