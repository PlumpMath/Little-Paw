// Filename: graphicsStateGuardianBase.h
// Created by:  drose (06Oct99)
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

#ifndef GRAPHICSSTATEGUARDIANBASE_H
#define GRAPHICSSTATEGUARDIANBASE_H

#include "pandabase.h"

#include "typedWritableReferenceCount.h"
#include "luse.h"
#include "pmutex.h"

// A handful of forward references.

class Thread;
class RenderBuffer;
class GraphicsWindow;
class NodePath;

class VertexBufferContext;
class IndexBufferContext;
class OcclusionQueryContext;
class GeomContext;
class GeomNode;
class Geom;
class GeomPipelineReader;
class GeomVertexData;
class GeomVertexDataPipelineReader;
class GeomVertexArrayData;
class GeomPrimitive;
class GeomPrimitivePipelineReader;
class GeomTriangles;
class GeomTristrips;
class GeomTrifans;
class GeomLines;
class GeomLinestrips;
class GeomPoints;
class GeomMunger;

class SceneSetup;
class PreparedGraphicsObjects;
class GraphicsOutput;
class Texture;
class TextureContext;
class ShaderContext;
class Shader;
class RenderState;
class TransformState;
class Material;

class ColorScaleAttrib;
class TexMatrixAttrib;
class ColorAttrib;
class TextureAttrib;
class LightAttrib;
class MaterialAttrib;
class RenderModeAttrib;
class AntialiasAttrib;
class RescaleNormalAttrib;
class ColorBlendAttrib;
class ColorWriteAttrib;
class AlphaTestAttrib;
class DepthTestAttrib;
class DepthWriteAttrib;
class TexGenAttrib;
class ShaderAttrib;
class CullFaceAttrib;
class ClipPlaneAttrib;
class ShadeModelAttrib;
class TransparencyAttrib;
class FogAttrib;
class DepthOffsetAttrib;

class PointLight;
class DirectionalLight;
class Spotlight;
class AmbientLight;

class DisplayRegion;
class Lens;

////////////////////////////////////////////////////////////////////
//       Class : GraphicsStateGuardianBase
// Description : This is a base class for the GraphicsStateGuardian
//               class, which is itself a base class for the various
//               GSG's for different platforms.  This class contains
//               all the function prototypes to support the
//               double-dispatch of GSG to geoms, transitions, etc.  It
//               lives in a separate class in its own package so we
//               can avoid circular build dependency problems.
//
//               GraphicsStateGuardians are not actually writable to
//               bam files, of course, but they may be passed as event
//               parameters, so they inherit from
//               TypedWritableReferenceCount instead of
//               TypedReferenceCount for that convenience.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA_GSGBASE GraphicsStateGuardianBase : public TypedWritableReferenceCount {
PUBLISHED:
  virtual bool prefers_triangle_strips() const=0;
  virtual int get_max_vertices_per_array() const=0;
  virtual int get_max_vertices_per_primitive() const=0;

  virtual int get_max_texture_dimension() const=0;

  virtual bool get_supports_multisample() const=0;
  virtual int get_supported_geom_rendering() const=0;
  virtual bool get_supports_occlusion_query() const=0;

public:
  // These are some general interface functions; they're defined here
  // mainly to make it easy to call these from code in some directory
  // that display depends on.
  virtual SceneSetup *get_scene() const=0;

#ifndef CPPPARSER
  // We hide this from interrogate, so that it will be properly
  // exported from the GraphicsStateGuardian class, later.
  virtual PreparedGraphicsObjects *get_prepared_objects()=0;
#endif

  virtual TextureContext *prepare_texture(Texture *tex)=0;
  virtual void release_texture(TextureContext *tc)=0;

  virtual GeomContext *prepare_geom(Geom *geom)=0;
  virtual void release_geom(GeomContext *gc)=0;

  virtual ShaderContext *prepare_shader(Shader *shader)=0;
  virtual void release_shader(ShaderContext *sc)=0;
  
  virtual VertexBufferContext *prepare_vertex_buffer(GeomVertexArrayData *data)=0;
  virtual void release_vertex_buffer(VertexBufferContext *vbc)=0;

  virtual IndexBufferContext *prepare_index_buffer(GeomPrimitive *data)=0;
  virtual void release_index_buffer(IndexBufferContext *ibc)=0;

  virtual void begin_occlusion_query()=0;
  virtual PT(OcclusionQueryContext) end_occlusion_query()=0;

  virtual PT(GeomMunger) get_geom_munger(const RenderState *state,
                                         Thread *current_thread)=0;

  virtual void set_state_and_transform(const RenderState *state,
                                       const TransformState *transform)=0;

  // This function may only be called during a render traversal; it
  // will compute the distance to the indicated point, assumed to be
  // in eye coordinates, from the camera plane.  This is a virtual
  // function because different GSG's may define the eye coordinate
  // space differently.
  virtual float compute_distance_to(const LPoint3f &point) const=0;

  // These are used to implement decals.  If depth_offset_decals()
  // returns true, none of the remaining functions will be called,
  // since depth offsets can be used to implement decals fully (and
  // usually faster).
  virtual bool depth_offset_decals()=0;
  virtual CPT(RenderState) begin_decal_base_first()=0;
  virtual CPT(RenderState) begin_decal_nested()=0;
  virtual CPT(RenderState) begin_decal_base_second()=0;
  virtual void finish_decal()=0;

  // Defined here are some internal interface functions for the
  // GraphicsStateGuardian.  These are here to support
  // double-dispatching from Geoms and NodeTransitions, and are
  // intended to be invoked only directly by the appropriate Geom and
  // NodeTransition types.  They're public only because it would be too
  // inconvenient to declare each of those types to be friends of this
  // class.

  virtual bool begin_draw_primitives(const GeomPipelineReader *geom_reader, 
                                     const GeomMunger *munger,
                                     const GeomVertexDataPipelineReader *data_reader, 
                                     bool force)=0;
  virtual bool draw_triangles(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual bool draw_tristrips(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual bool draw_trifans(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual bool draw_lines(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual bool draw_linestrips(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual bool draw_points(const GeomPrimitivePipelineReader *reader, bool force)=0;
  virtual void end_draw_primitives()=0;

  virtual bool framebuffer_copy_to_texture
  (Texture *tex, int z, const DisplayRegion *dr, const RenderBuffer &rb)=0;
  virtual bool framebuffer_copy_to_ram
  (Texture *tex, int z, const DisplayRegion *dr, const RenderBuffer &rb)=0;
  
  virtual CoordinateSystem get_internal_coordinate_system() const=0;
  virtual const TransformState *get_cs_transform() const=0;

  virtual void bind_light(PointLight *light_obj, const NodePath &light, 
                          int light_id) { }
  virtual void bind_light(DirectionalLight *light_obj, const NodePath &light,
                          int light_id) { }
  virtual void bind_light(Spotlight *light_obj, const NodePath &light,
                          int light_id) { }

PUBLISHED:
  static GraphicsStateGuardianBase *get_default_gsg();
  static void set_default_gsg(GraphicsStateGuardianBase *default_gsg);

public:
  static void add_gsg(GraphicsStateGuardianBase *gsg);
  static void remove_gsg(GraphicsStateGuardianBase *gsg);

private:
  typedef pvector<GraphicsStateGuardianBase *> GSGs;
  static GSGs _gsgs;
  static GraphicsStateGuardianBase *_default_gsg;
  static Mutex _lock;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedWritableReferenceCount::init_type();
    register_type(_type_handle, "GraphicsStateGuardianBase",
                  TypedWritableReferenceCount::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#endif
