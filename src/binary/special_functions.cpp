/*
 * special_functions.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: Julian Kranz
 */
#include <bjutil/binary/binary_provider.h>
#include <bjutil/binary/special_functions.h>
#include <tuple>

using namespace std;

special_functions special_functions::from_elf_provider(elf_provider &ep) {
  type_functions_t tf;

  bool found;
  binary_provider::entry_t entry;
  tie(found, entry) = ep.symbol("malloc@plt");
  if(found)
    tf[entry.address] = ALLOCATION;

  return special_functions(tf);
}
