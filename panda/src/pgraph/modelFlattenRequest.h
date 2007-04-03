// Filename: modelFlattenRequest.h
// Created by:  drose (30Mar07)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#ifndef MODELFLATTENREQUEST
#define MODELFLATTENREQUEST

#include "pandabase.h"

#include "asyncTask.h"
#include "pandaNode.h"
#include "pointerTo.h"

////////////////////////////////////////////////////////////////////
//       Class : ModelFlattenRequest
// Description : This class object manages a single asynchronous
//               request to flatten a model.  The model will be
//               duplicated and flattened in a sub-thread (if
//               threading is available), without affecting the
//               original model; and when the result is done it may be
//               retrieved from this object.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA ModelFlattenRequest : public AsyncTask {
PUBLISHED:
  INLINE ModelFlattenRequest(PandaNode *orig);
  
  INLINE PandaNode *get_orig() const;
  
  INLINE bool is_ready() const;
  INLINE PandaNode *get_model() const;
  
protected:
  virtual bool do_task();
  
private:
  PT(PandaNode) _orig;
  bool _is_ready;
  PT(PandaNode) _model;
  
public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    AsyncTask::init_type();
    register_type(_type_handle, "ModelFlattenRequest",
                  AsyncTask::get_class_type());
    }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}
  
private:
  static TypeHandle _type_handle;
};

#include "modelFlattenRequest.I"

#endif
