/*
 * scope.h
 *
 *  Created on: Dec 23, 2014
 *      Author: Julian Kranz
 */

#pragma once

#include <vector>
#include <functional>

#include <iostream>

//#define SCOPE_EXIT_INIT(HANDLER) scope_exit exit([&]() {\
//    HANDLER\
//  });

typedef std::function<void()> handler_t;

class scope_exit {
private:
  std::vector<handler_t> handlers;

public:
  scope_exit(handler_t body) {
    handlers.push_back(initial);
    try {
      body();
    } catch(...) {
      for(auto &handler : handlers)
        handler();
      throw;
    }
  }
  ~scope_exit() {
  }

  void add(handler_t handler) {
    handlers.push_back(handler);
  }
};
