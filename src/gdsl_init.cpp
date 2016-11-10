/*
 * gdsl_init.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Julian Kranz
 */

#include <bjutil/binary/elf_provider.h>
#include <bjutil/binary/file_provider.h>
#include <bjutil/gdsl_init.h>
#include <functional>
#include <iostream>
#include <string.h>
#include <string>

using namespace std;

bj_gdsl::~bj_gdsl() {
  if(gdsl != nullptr) {
    gdsl->set_code(NULL, 0, 0);
    delete gdsl;
  }
  if(buffer != nullptr) free(buffer);
}

static bj_gdsl gdsl_init_elf_(gdsl::_frontend *f, std::string filename, std::string section_name,
  std::string function_name,
  function<size_t(binary_provider::entry_t, binary_provider::entry_t)> get_size) {
  elf_provider elfp = [&]() {
    try {
      return elf_provider(filename.c_str());
    } catch(string &s) {
      cout << "Error initializing elf provider: " << s << endl;
      throw string("no elf() :/");
    }
  }();
  binary_provider::entry_t section;
  bool success;
  tie(success, section) = elfp.section(section_name);
  if(!success) throw string("Invalid section .text");

  binary_provider::entry_t function;
  tie(ignore, function) = elfp.symbol(function_name);

  unsigned char *buffer = (unsigned char *)malloc(section.size);
  memcpy(buffer, elfp.get_data().data + section.offset, section.size);

  size_t size = get_size(section, function);

  gdsl::gdsl *gdsl = new gdsl::gdsl(f);

  gdsl->set_code(buffer, size, section.address);
  if(gdsl->seek(function.address)) {
    delete gdsl;
    throw string("Unable to seek to given function_name");
  }
  //  cout << g.get_ip() << endl;

  return bj_gdsl(gdsl, buffer);
}

bj_gdsl gdsl_init_elf(gdsl::_frontend *f, std::string filename, std::string section_name,
  std::string function_name, size_t excess_bytes) {
  return gdsl_init_elf_(f, filename, section_name, function_name,
    [&](binary_provider::entry_t section, binary_provider::entry_t function) {
      size_t size = (function.offset - section.offset) + function.size + excess_bytes;
      if(size > section.size) size = section.size;
      return size;
    });
}

bj_gdsl gdsl_init_elf(gdsl::_frontend *f, std::string filename, std::string section_name,
  std::string function_name, bool full_section) {
  return gdsl_init_elf_(f, filename, section_name, function_name,
    [&](binary_provider::entry_t section, binary_provider::entry_t function) {
      if(full_section)
        return section.size;
      else
        return (function.offset - section.offset) + function.size;
    });
}

bj_gdsl gdsl_init_elf(
  gdsl::_frontend *f, std::string filename, std::string section_name, std::string function_name) {
  return gdsl_init_elf(f, filename, section_name, function_name, false);
}

bj_gdsl gdsl_init_immediate(gdsl::_frontend *f, uint32_t immediate, size_t ip) {
  uint32_t *buffer_heap = (uint32_t *)malloc(sizeof(immediate));
  *buffer_heap = immediate;
  gdsl::gdsl *gdsl = new gdsl::gdsl(f);
  gdsl->set_code((unsigned char *)buffer_heap, sizeof(immediate), ip);
  return bj_gdsl(gdsl, buffer_heap);
}

bj_gdsl gdsl_init_binfile(gdsl::_frontend *f, std::string filename, size_t ip) {
  file_provider fp(filename.c_str());

  binary_provider::data_t data = fp.get_data();

  unsigned char *buffer = (unsigned char *)malloc(data.size);
  memcpy(buffer, data.data, data.size);

  gdsl::gdsl *gdsl = new gdsl::gdsl(f);
  gdsl->set_code(buffer, data.size, ip);

  return bj_gdsl(gdsl, buffer);
}
