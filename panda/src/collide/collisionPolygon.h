// Filename: collisionPolygon.h
// Created by:  drose (25Apr00)
// 
////////////////////////////////////////////////////////////////////

#ifndef COLLISIONPOLYGON_H
#define COLLISIONPOLYGON_H

#include <pandabase.h>

#include "collisionPlane.h"

#include <vector_LPoint2f.h>

///////////////////////////////////////////////////////////////////
// 	 Class : CollisionPolygon
// Description : 
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA CollisionPolygon : public CollisionPlane {
PUBLISHED:
  INLINE CollisionPolygon(const LPoint3f &a, const LPoint3f &b,
			  const LPoint3f &c);
  INLINE CollisionPolygon(const LPoint3f &a, const LPoint3f &b,
			  const LPoint3f &c, const LPoint3f &d);
  INLINE CollisionPolygon(const LPoint3f *begin, const LPoint3f *end);

public:
  CollisionPolygon(const CollisionPolygon &copy);

  virtual CollisionSolid *make_copy();

  INLINE static bool verify_points(const LPoint3f &a, const LPoint3f &b,
				   const LPoint3f &c);
  INLINE static bool verify_points(const LPoint3f &a, const LPoint3f &b,
				   const LPoint3f &c, const LPoint3f &d);
  static bool verify_points(const LPoint3f *begin, const LPoint3f *end);


  virtual int
  test_intersection(CollisionHandler *record,
		    const CollisionEntry &entry,
		    const CollisionSolid *into) const;

  virtual void xform(const LMatrix4f &mat);

  virtual void output(ostream &out) const;

protected:
  INLINE CollisionPolygon(void);
  virtual void recompute_bound();

protected:
  virtual int
  test_intersection_from_sphere(CollisionHandler *record,
				const CollisionEntry &entry) const;
  virtual int
  test_intersection_from_ray(CollisionHandler *record,
			     const CollisionEntry &entry) const;

  virtual void recompute_viz(Node *parent);

private:
  bool is_inside(const LPoint2f &p) const;

  void setup_points(const LPoint3f *begin, const LPoint3f *end);
  LPoint2f to_2d(const LPoint3f &point3d) const;
  LPoint3f to_3d(const LPoint2f &point2d) const;

private:
  typedef vector_LPoint2f Points;
  Points _points;
  LPoint2f _median;

  enum AxisType {
    AT_x, AT_y, AT_z
  };
  AxisType _axis;
  bool _reversed;

public:
  static void register_with_read_factory(void);
  virtual void write_datagram(BamWriter* manager, Datagram &me);  

  static TypedWriteable *make_CollisionPolygon(const FactoryParams &params);

protected:
  void fillin(DatagramIterator& scan, BamReader* manager);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    CollisionPlane::init_type();
    register_type(_type_handle, "CollisionPolygon",
		  CollisionPlane::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "collisionPolygon.I"

#endif


