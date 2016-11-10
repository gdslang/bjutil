/*
 * gdsl_init.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Julian Kranz
 */

#pragma once

#include <cppgdsl/gdsl.h>
#include <cppgdsl/frontend/frontend.h>
#include <string>
#include <stdint.h>

struct bj_gdsl {
private:
  void *buffer;

public:
  gdsl::gdsl *gdsl;

  bj_gdsl(gdsl::gdsl *gdsl, void *buffer) : gdsl(gdsl), buffer(buffer) {
  }
  bj_gdsl(bj_gdsl const&) = delete;
  bj_gdsl& operator=(bj_gdsl const&) = delete;
  bj_gdsl(bj_gdsl &&other) {
    buffer = other.buffer;
    gdsl = other.gdsl;
    other.buffer = nullptr;
    other.gdsl = nullptr;
  }
  bj_gdsl& operator=(bj_gdsl &&other) {
    buffer = other.buffer;
    gdsl = other.gdsl;
    other.buffer = nullptr;
    other.gdsl = nullptr;
    return *this;
  }
  ~bj_gdsl();
};


bj_gdsl gdsl_init_elf(gdsl::_frontend *f, std::string filename, std::string section_name, std::string function_name, size_t excess_bytes);
bj_gdsl gdsl_init_elf(gdsl::_frontend *f, std::string filename, std::string section_name, std::string function_name, bool full_section);
bj_gdsl gdsl_init_elf(gdsl::_frontend *f, std::string filename, std::string section_name, std::string function_name);
bj_gdsl gdsl_init_immediate(gdsl::_frontend *f, uint32_t immediate, size_t ip);
bj_gdsl gdsl_init_binfile(gdsl::_frontend *f, std::string filename, size_t ip);
