// Filename: shaderAttrib.h
// Created by: jyelon (01Sep05)
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

#ifndef SHADERATTRIB_H
#define SHADERATTRIB_H

#include "pandabase.h"
#include "renderAttrib.h"
#include "pointerTo.h"
#include "shaderInput.h"
#include "shader.h"

////////////////////////////////////////////////////////////////////
//       Class : ShaderAttrib
// Description : 
////////////////////////////////////////////////////////////////////

class EXPCL_PANDA_PGRAPH ShaderAttrib: public RenderAttrib {

private:
  INLINE ShaderAttrib();
  INLINE ShaderAttrib(const ShaderAttrib &copy);

PUBLISHED:
  static CPT(RenderAttrib) make();
  static CPT(RenderAttrib) make_off();
  
  enum {
    F_disable_alpha_write = 0,  // Suppress writes to color buffer alpha channel.
    F_subsume_alpha_test  = 1,  // Shader promises to subsume the alpha test using TEXKILL
  };

  INLINE bool               has_shader() const;
  INLINE bool               auto_shader() const;
  INLINE int                get_shader_priority() const;
  
  CPT(RenderAttrib) set_shader(const Shader *s, int priority=0) const;
  CPT(RenderAttrib) set_shader_off(int priority=0) const;
  CPT(RenderAttrib) set_shader_auto(int priority=0) const;
  CPT(RenderAttrib) clear_shader() const;
  CPT(RenderAttrib) set_shader_input(const ShaderInput *inp) const;
  CPT(RenderAttrib) set_shader_input(InternalName *id, Texture *tex,       int priority=0) const;
  CPT(RenderAttrib) set_shader_input(InternalName *id, const NodePath &np, int priority=0) const;
  CPT(RenderAttrib) set_shader_input(InternalName *id, const LVector4f &v, int priority=0) const;
  CPT(RenderAttrib) set_shader_input(InternalName *id, double n1=0, double n2=0, double n3=0, double n4=1,
                                     int priority=0) const;
  CPT(RenderAttrib) set_shader_input(const string &id, Texture *tex,       int priority=0) const;
  CPT(RenderAttrib) set_shader_input(const string &id, const NodePath &np, int priority=0) const;
  CPT(RenderAttrib) set_shader_input(const string &id, const LVector4f &v, int priority=0) const;
  CPT(RenderAttrib) set_shader_input(const string &id, double n1=0, double n2=0, double n3=0, double n4=1,
                                     int priority=0) const;

  CPT(RenderAttrib) set_flag(int flag, bool value) const;
  CPT(RenderAttrib) clear_flag(int flag) const;

  CPT(RenderAttrib) clear_shader_input(InternalName *id) const;
  CPT(RenderAttrib) clear_shader_input(const string &id) const;

  INLINE bool        get_flag(int flag) const;
  
  const Shader      *get_shader() const;
  const ShaderInput *get_shader_input(InternalName *id) const;
  const ShaderInput *get_shader_input(const string &id) const;

  const NodePath    &get_shader_input_nodepath(InternalName *id) const;
  const LVector4f   &get_shader_input_vector(InternalName *id) const;
  Texture*           get_shader_input_texture(InternalName *id) const;
  
  static void register_with_read_factory();
  
public:
  virtual void store_into_slot(AttribSlots *slots) const;

protected:
  virtual RenderAttrib *make_default_impl() const;
  virtual int compare_to_impl(const RenderAttrib *other) const;
  virtual CPT(RenderAttrib) compose_impl(const RenderAttrib *other) const;
  
private:

  CPT(Shader) _shader;
  int         _shader_priority;
  bool        _auto_shader;
  bool        _has_shader;
  int         _flags;
  int         _has_flags;
  typedef pmap < CPT(InternalName), CPT(ShaderInput) > Inputs;
  Inputs _inputs;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    RenderAttrib::init_type();
    register_type(_type_handle, "ShaderAttrib",
                  RenderAttrib::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};


#include "shaderAttrib.I"

#endif  // SHADERATTRIB_H



