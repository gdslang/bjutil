/*
 * autogc.h
 *
 *  Created on: Mar 10, 2015
 *      Author: Julian Kranz
 */

#pragma once

#include <vector>
#include <functional>

class autogc {
private:
  std::vector<std::function<void()>> finalizers;

public:
  template<typename T>
  T *operator()(T *arg) {
    finalizers.push_back([=]() {
      delete arg;
    });
    return arg;
  }
  ~autogc();
};
