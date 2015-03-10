/*
 * autogc.cpp
 *
 *  Created on: Mar 10, 2015
 *      Author: Julian Kranz
 */

#include <bjutil/autogc.h>

autogc::~autogc() {
  for(auto finalizer : finalizers)
    finalizer();
}
