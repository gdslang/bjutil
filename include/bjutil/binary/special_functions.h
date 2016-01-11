/*
 * special_functions.h
 *
 *  Created on: Jan 11, 2016
 *      Author: Julian Kranz
 */

#pragma once
#include <bjutil/binary/elf_provider.h>
#include <map>
#include <set>

enum special_function_type {
  ALLOCATION
};

//typedef std::map<special_function_type, std::set<size_t>> type_functions_t;
typedef std::map<size_t, special_function_type> type_functions_t;

class special_functions {
private:
  type_functions_t type_functions;

  special_functions(type_functions_t type_functions) : type_functions(type_functions) {
  }
public:
  special_functions() {
  }

  type_functions_t const& get_type_functions() {
    return type_functions;
  }

  static special_functions from_elf_provider(elf_provider &ep);
};
