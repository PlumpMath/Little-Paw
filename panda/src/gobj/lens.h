// Filename: lens.h
// Created by:  drose (18Feb99)
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

#ifndef LENS_H
#define LENS_H

#include "pandabase.h"

#include "typedWritableReferenceCount.h"
#include "luse.h"
#include "geom.h"
#include "updateSeq.h"
#include "geomVertexData.h"
#include "pointerTo.h"

class BoundingVolume;

////////////////////////////////////////////////////////////////////
//       Class : Lens
// Description : A base class for any number of different kinds of
//               lenses, linear and otherwise.  Presently, this
//               includes perspective and orthographic lenses.
//
//               A Lens object is the main part of a Camera node
//               (defined in sgraph), which defines the fundamental
//               interface to point-of-view for rendering.  Lenses are
//               also used in other contexts, however; for instance, a
//               Spotlight is also defined using a lens.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA_GOBJ Lens : public TypedWritableReferenceCount {
public:
  Lens();
  Lens(const Lens &copy);
  void operator = (const Lens &copy);

PUBLISHED:
  enum StereoChannel {
    SC_mono    = 0x00,
    SC_left    = 0x01,
    SC_right   = 0x02,
    SC_stereo  = 0x03,  // == SC_left | SC_right
  };

  virtual PT(Lens) make_copy() const=0;

  INLINE bool extrude(const LPoint2 &point2d,
                      LPoint3 &near_point, LPoint3 &far_point) const;
  INLINE bool extrude(const LPoint3 &point2d,
                      LPoint3 &near_point, LPoint3 &far_point) const;
  INLINE bool extrude_vec(const LPoint2 &point2d, LVector3 &vec3d) const;
  INLINE bool extrude_vec(const LPoint3 &point2d, LVector3 &vec3d) const;
  INLINE bool project(const LPoint3 &point3d, LPoint3 &point2d) const;
  INLINE bool project(const LPoint3 &point3d, LPoint2 &point2d) const;

  INLINE void set_change_event(const string &event);
  INLINE const string &get_change_event() const;

  void set_coordinate_system(CoordinateSystem cs);
  INLINE CoordinateSystem get_coordinate_system() const;

  void clear();

  void set_film_size(PN_stdfloat width);
  INLINE void set_film_size(PN_stdfloat width, PN_stdfloat height);
  void set_film_size(const LVecBase2 &film_size);
  const LVecBase2 &get_film_size() const;

  INLINE void set_film_offset(PN_stdfloat x, PN_stdfloat y);
  INLINE void set_film_offset(const LVecBase2 &film_offset);
  INLINE const LVector2 &get_film_offset() const;

  void set_focal_length(PN_stdfloat focal_length);
  PN_stdfloat get_focal_length() const;

  void set_min_fov(PN_stdfloat min_fov);
  void set_fov(PN_stdfloat fov);
  INLINE void set_fov(PN_stdfloat hfov, PN_stdfloat vfov);
  void set_fov(const LVecBase2 &fov);
  const LVecBase2 &get_fov() const;
  INLINE PN_stdfloat get_hfov() const;
  INLINE PN_stdfloat get_vfov() const;
  PN_stdfloat get_min_fov() const;

  void set_aspect_ratio(PN_stdfloat aspect_ratio);
  PN_stdfloat get_aspect_ratio() const;

  INLINE void set_near(PN_stdfloat near_distance);
  INLINE PN_stdfloat get_near() const;
  INLINE void set_far(PN_stdfloat far_distance);
  INLINE PN_stdfloat get_far() const;
  INLINE void set_near_far(PN_stdfloat near_distance, PN_stdfloat far_distance);

  static PN_stdfloat get_default_near();
  static PN_stdfloat get_default_far();
  
  INLINE void set_view_hpr(PN_stdfloat h, PN_stdfloat p, PN_stdfloat r);
  void set_view_hpr(const LVecBase3 &view_hpr);
  const LVecBase3 &get_view_hpr() const;
  INLINE void set_view_vector(PN_stdfloat x, PN_stdfloat y, PN_stdfloat z, PN_stdfloat i, PN_stdfloat j, PN_stdfloat k);
  void set_view_vector(const LVector3 &view_vector, const LVector3 &up_vector);
  const LVector3 &get_view_vector() const;
  const LVector3 &get_up_vector() const;
  LPoint3 get_nodal_point() const;

  void set_interocular_distance(PN_stdfloat interocular_distance);
  PN_stdfloat get_interocular_distance() const;
  void set_convergence_distance(PN_stdfloat convergence_distance);
  PN_stdfloat get_convergence_distance() const;

  void set_view_mat(const LMatrix4 &view_mat);
  const LMatrix4 &get_view_mat() const;
  void clear_view_mat();

  void set_keystone(const LVecBase2 &keystone);
  INLINE const LVecBase2 &get_keystone() const;
  void clear_keystone();
  
  // These flags are passed in as the last parameter to control the
  // behavior of set_frustum_from_corners().  See the documentation
  // for that method for an explanation of each flag.
  enum FromCorners {
    FC_roll         = 0x0001,
    FC_camera_plane = 0x0002,
    FC_off_axis     = 0x0004,
    FC_aspect_ratio = 0x0008,
    FC_shear        = 0x0010,
    FC_keystone     = 0x0020,
  };
  void set_frustum_from_corners(const LVecBase3 &ul, const LVecBase3 &ur,
                                const LVecBase3 &ll, const LVecBase3 &lr,
                                int flags);

  void recompute_all();

  virtual bool is_linear() const;
  virtual bool is_perspective() const;
  virtual bool is_orthographic() const;
  virtual PT(Geom) make_geometry();

  virtual PT(BoundingVolume) make_bounds() const;

  const LMatrix4 &get_projection_mat(StereoChannel channel = SC_mono) const;
  const LMatrix4 &get_projection_mat_inv(StereoChannel channel = SC_mono) const;

  const LMatrix4 &get_film_mat() const;
  const LMatrix4 &get_film_mat_inv() const;

  const LMatrix4 &get_lens_mat() const;
  const LMatrix4 &get_lens_mat_inv() const;

  virtual void output(ostream &out) const;
  virtual void write(ostream &out, int indent_level = 0) const;

public:
  INLINE const UpdateSeq &get_last_change() const;

protected:
  INLINE void adjust_user_flags(int clear_flags, int set_flags);
  INLINE void adjust_comp_flags(int clear_flags, int set_flags);

  void throw_change_event();

  virtual bool extrude_impl(const LPoint3 &point2d,
                            LPoint3 &near_point, LPoint3 &far_point) const;
  virtual bool extrude_vec_impl(const LPoint3 &point2d, LVector3 &vec) const;
  virtual bool project_impl(const LPoint3 &point3d, LPoint3 &point2d) const;

  virtual void compute_film_size();
  virtual void compute_focal_length();
  virtual void compute_fov();
  virtual void compute_aspect_ratio();
  virtual void compute_view_hpr();
  virtual void compute_view_vector();
  virtual void compute_projection_mat();
  virtual void compute_film_mat();
  virtual void compute_lens_mat();

  virtual PN_stdfloat fov_to_film(PN_stdfloat fov, PN_stdfloat focal_length, bool horiz) const;
  virtual PN_stdfloat fov_to_focal_length(PN_stdfloat fov, PN_stdfloat film_size, bool horiz) const;
  virtual PN_stdfloat film_to_fov(PN_stdfloat film_size, PN_stdfloat focal_length, bool horiz) const;

private:
  void resequence_fov_triad(char &newest, char &older_a, char &older_b) const;
  int define_geom_data();
  static void build_shear_mat(LMatrix4 &shear_mat,
                              const LPoint3 &cul, const LPoint3 &cur,
                              const LPoint3 &cll, const LPoint3 &clr);
  static PN_stdfloat sqr_dist_to_line(const LPoint3 &point, const LPoint3 &origin, 
                                const LVector3 &vec);

protected:
  string _change_event;
  UpdateSeq _last_change;
  CoordinateSystem _cs;

  LVecBase2 _film_size;
  LVector2 _film_offset;
  PN_stdfloat _focal_length;
  LVecBase2 _fov;
  PN_stdfloat _min_fov;
  PN_stdfloat _aspect_ratio;
  PN_stdfloat _near_distance, _far_distance;

  LVecBase3 _view_hpr;
  LVector3 _view_vector, _up_vector;
  PN_stdfloat _interocular_distance;
  PN_stdfloat _convergence_distance;
  LVecBase2 _keystone;

  LMatrix4 _film_mat, _film_mat_inv;
  LMatrix4 _lens_mat, _lens_mat_inv;
  LMatrix4 _projection_mat, _projection_mat_inv;
  LMatrix4 _projection_mat_left, _projection_mat_left_inv;
  LMatrix4 _projection_mat_right, _projection_mat_right_inv;

  enum UserFlags {
    // Parameters the user may have explicitly specified.
    UF_film_width           = 0x0001,
    UF_film_height          = 0x0002,
    UF_focal_length         = 0x0004,
    UF_hfov                 = 0x0008,
    UF_vfov                 = 0x0010,
    UF_aspect_ratio         = 0x0020,
    UF_view_hpr             = 0x0040,
    UF_view_vector          = 0x0080,
    UF_interocular_distance = 0x0100,
    UF_convergence_distance = 0x0200,
    UF_view_mat             = 0x0400,
    UF_keystone             = 0x0800,
    UF_min_fov              = 0x1000,
  };

  enum CompFlags {
    // Values that may need to be recomputed.
    CF_film_mat            = 0x0001,
    CF_film_mat_inv        = 0x0002,
    CF_lens_mat            = 0x0004,
    CF_lens_mat_inv        = 0x0008,
    CF_projection_mat      = 0x0010,
    CF_projection_mat_inv  = 0x0020,
    CF_projection_mat_left_inv  = 0x0040,
    CF_projection_mat_right_inv = 0x0080,
    CF_mat                 = 0x00ff,  // all of the above.

    CF_film_size           = 0x0100,
    CF_aspect_ratio        = 0x0200,
    CF_view_hpr            = 0x0400,
    CF_view_vector         = 0x0800,
    CF_focal_length        = 0x1000,
    CF_fov                 = 0x2000,
  };
  short _user_flags;
  short _comp_flags;

  // The user may only specify two of these three parameters.
  // Specifying the third parameter wipes out the first one specified.
  // We therefore need to remember the order in which the user has
  // specified these three parameters.  A bit of a mess.
  char _focal_length_seq, _fov_seq, _film_size_seq;

  PT(GeomVertexData) _geom_data;

public:
  virtual void write_datagram(BamWriter *manager, Datagram &dg);

protected:
  void fillin(DatagramIterator &scan, BamReader *manager);

public:
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedWritableReferenceCount::init_type();
    register_type(_type_handle, "Lens",
                  TypedWritableReferenceCount::get_class_type());
  }

private:
  static TypeHandle _type_handle;
};

EXPCL_PANDA_GOBJ INLINE ostream &operator << (ostream &out, const Lens &lens);

#include "lens.I"

#endif

