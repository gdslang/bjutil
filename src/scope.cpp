/*
 * scope.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: jucs
 */

#include <bjutil/scope.h>

void scope_exit::call() {
  for(auto &handler : handlers)
    handler();
}

scope_exit::scope_exit(handler_t body) {
  try {
    body();
  } catch(...) {
    call();
    throw;
  }
  call();
}

void scope_exit::operator ()(handler_t handler) {
  handlers.push_back(handler);
}
