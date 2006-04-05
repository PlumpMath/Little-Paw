// Filename: glOcclusionQueryContext_src.h
// Created by:  drose (27Mar06)
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

#include "pandabase.h"
#include "occlusionQueryContext.h"
#include "deletedChain.h"

class GraphicsStateGuardian;

////////////////////////////////////////////////////////////////////
//       Class : GLOcclusionQueryContext
// Description :
////////////////////////////////////////////////////////////////////
class EXPCL_GL CLP(OcclusionQueryContext) : public OcclusionQueryContext {
public:
  INLINE CLP(OcclusionQueryContext)(GraphicsStateGuardian *gsg);
  virtual ~CLP(OcclusionQueryContext)();
  ALLOC_DELETED_CHAIN(CLP(OcclusionQueryContext));

  virtual bool is_answer_ready() const;
  virtual void waiting_for_answer();
  virtual int get_num_fragments() const;

  GLuint _index;
  GraphicsStateGuardian *_gsg;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    OcclusionQueryContext::init_type();
    register_type(_type_handle, CLASSPREFIX_QUOTED "OcclusionQueryContext",
                  OcclusionQueryContext::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "glOcclusionQueryContext_src.I"

