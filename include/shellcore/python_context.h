/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef _PYTHON_CONTEXT_H_
#define _PYTHON_CONTEXT_H_

#include "shellcore/shell_python.h"

#include <Python.h>
#include "shellcore/python_type_conversion.h"

namespace shcore {

class Python_context
{
public:
  Python_context() throw (Exception);
  ~Python_context();

  Value execute(const std::string &code);

  Value pyobj_to_shcore_value(PyObject *value);
  PyObject *shcore_value_to_pyobj(const Value &value);

  PyObject *get_global(const std::string &value); 
  void set_global(const std::string &name, const Value &value);
private:
  struct Python_context_impl;
  Python_context_impl *_impl;

  PyObject *_globals;
  PyThreadState *_main_thread_state;

  Python_type_bridger types;
};

}

#endif
