/*
 * sliced_memory.cpp
 *
 *  Created on: Aug 9, 2014
 *      Author: jucs
 */

#include <bjutil/sliced_memory.h>
#include <algorithm>
#include <iostream>

using namespace std;

static bool smaller(slice const &a, slice const &b) {
  return a.address > b.address;
}

sliced_memory::sliced_memory(std::vector<slice> slices) {
  sort(slices.begin(), slices.end(), &smaller);
  this->slices = slices;
}

std::tuple<bool, slice> sliced_memory::deref(void *address) {
  auto result = lower_bound(slices.begin(), slices.end(), slice(address), &smaller);
  bool success = result != slices.end() && ((char*)address - (char*)(*result).address) < (*result).size;
  return make_tuple(success, success ? *result : slice(0));
}
