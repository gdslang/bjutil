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

  void call();
public:
  scope_exit(handler_t body);
  ~scope_exit() {
  }
  void operator()(handler_t handler);
};
