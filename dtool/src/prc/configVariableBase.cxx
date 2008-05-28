// Filename: configVariableBase.cxx
// Created by:  drose (21Oct04)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "configVariableBase.h"

////////////////////////////////////////////////////////////////////
//     Function: ConfigVariableBase::Constructor
//       Access: Protected
//  Description: This constructor is only intended to be called from a
//               specialized ConfigVariableFoo derived class.
////////////////////////////////////////////////////////////////////
ConfigVariableBase::
ConfigVariableBase(const string &name, 
                   ConfigVariableBase::ValueType value_type,
                   const string &description, int flags) :
  _core(ConfigVariableManager::get_global_ptr()->make_variable(name))
{
  if (value_type != VT_undefined) {
    _core->set_value_type(value_type);
  }
#ifdef PRC_SAVE_DESCRIPTIONS
  if (!description.empty()) {
    _core->set_description(description);
  }
#endif  // PRC_SAVE_DESCRIPTIONS
  if (flags != 0) {
    _core->set_flags(flags);
  }
}
