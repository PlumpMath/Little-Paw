// Filename: geomVertexColumn.cxx
// Created by:  drose (06Mar05)
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

#include "geomVertexColumn.h"
#include "geomVertexData.h"
#include "bamReader.h"
#include "bamWriter.h"

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Copy Assignment Operator
//       Access: Published
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
operator = (const GeomVertexColumn &copy) {
  _name = copy._name;
  _num_components = copy._num_components;
  _numeric_type = copy._numeric_type;
  _contents = copy._contents;
  _start = copy._start;
  _column_alignment = copy._column_alignment;

  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_name
//       Access: Published
//  Description: Replaces the name of an existing column.  This is
//               only legal on an unregistered format (i.e. when
//               constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_name(InternalName *name) {
  _name = name;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_num_components
//       Access: Published
//  Description: Changes the number of components of an existing
//               column.  This is only legal on an unregistered format
//               (i.e. when constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_num_components(int num_components) {
  _num_components = num_components;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_numeric_type
//       Access: Published
//  Description: Changes the numeric type an existing column.  This is
//               only legal on an unregistered format (i.e. when
//               constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_numeric_type(NumericType numeric_type) {
  _numeric_type = numeric_type;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_contents
//       Access: Published
//  Description: Changes the semantic meaning of an existing column.
//               This is only legal on an unregistered format
//               (i.e. when constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_contents(Contents contents) {
  _contents = contents;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_start
//       Access: Published
//  Description: Changes the start byte of an existing column.
//               This is only legal on an unregistered format
//               (i.e. when constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_start(int start) {
  _start = start;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::set_column_alignment
//       Access: Published
//  Description: Changes the column alignment of an existing column.
//               This is only legal on an unregistered format
//               (i.e. when constructing the format initially).
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
set_column_alignment(int column_alignment) {
  _column_alignment = column_alignment;
  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::output
//       Access: Published
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
output(ostream &out) const {
  out << *get_name() << "(" << get_num_components();
  switch (get_numeric_type()) {
  case NT_uint8:
    out << "b";
    break;

  case NT_uint16:
    out << "s";
    break;

  case NT_uint32:
    out << "l";
    break;

  case NT_packed_dcba:
    out << "p-";
    break;

  case NT_packed_dabc:
    out << "p";
    break;

  case NT_float32:
    out << "f";
    break;

  case NT_float64:
    out << "d";
    break;

  case NT_stdfloat:
    out << "?";
    break;
  }

  out << ")";
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::setup
//       Access: Private
//  Description: Called once at construction time (or at bam-reading
//               time) to initialize the internal dependent values.
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
setup() {
  nassertv(_num_components > 0 && _start >= 0);

  _num_values = _num_components;

  if (_numeric_type == NT_stdfloat) {
    if (vertices_float64) {
      _numeric_type = NT_float64;
    } else {
      _numeric_type = NT_float32;
    }
  }

  switch (_numeric_type) {
  case NT_uint16:
    _component_bytes = 2;  // sizeof(PN_uint16)
    break;

  case NT_uint32:
    _component_bytes = 4;  // sizeof(PN_uint32)
    break;

  case NT_uint8:
    _component_bytes = 1;
    break;

  case NT_packed_dcba:
  case NT_packed_dabc:
    _component_bytes = 4;  // sizeof(PN_uint32)
    _num_values *= 4;
    break;

  case NT_float32:
    _component_bytes = 4;  // sizeof(PN_float32)
    break;

  case NT_float64:
    _component_bytes = 8;  // sizeof(PN_float64)
    break;

  case NT_stdfloat:
    nassertv(false);
    break;
  }

  if (_column_alignment < 1) {
    // The default column alignment is to align to the individual
    // numeric components, or to vertex_column_alignment, whichever is
    // greater.
    _column_alignment = max(_component_bytes, (int)vertex_column_alignment);
  }

  // Enforce the column alignment requirements on the _start byte.
  _start = ((_start + _column_alignment - 1) / _column_alignment) * _column_alignment;

  _total_bytes = _component_bytes * _num_components;

  if (_packer != NULL) {
    delete _packer;
  }
  
  _packer = make_packer();
  _packer->_column = this;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::make_packer
//       Access: Private
//  Description: Returns a newly-allocated Packer object suitable for
//               packing and unpacking this column.  The _column
//               member of the packer is not filled in.
////////////////////////////////////////////////////////////////////
GeomVertexColumn::Packer *GeomVertexColumn::
make_packer() const {
  switch (get_contents()) {
  case C_point:
  case C_clip_point:
  case C_texcoord:
    // These types are read as a 4-d homogeneous point.
    switch (get_numeric_type()) {
    case NT_float32:
      if (sizeof(float) == sizeof(PN_float32)) {
        // Use the native float type implementation for a tiny bit
        // more optimization.
        switch (get_num_components()) {
        case 2:
          return new Packer_point_nativefloat_2;
        case 3:
          return new Packer_point_nativefloat_3;
        case 4:
          return new Packer_point_nativefloat_4;
        }
      } else {
        switch (get_num_components()) {
        case 2:
          return new Packer_point_float32_2;
        case 3:
          return new Packer_point_float32_3;
        case 4:
          return new Packer_point_float32_4;
        }
      }
      break;
    case NT_float64:
      if (sizeof(double) == sizeof(PN_float64)) {
        // Use the native float type implementation for a tiny bit
        // more optimization.
        switch (get_num_components()) {
        case 2:
          return new Packer_point_nativedouble_2;
        case 3:
          return new Packer_point_nativedouble_3;
        case 4:
          return new Packer_point_nativedouble_4;
        }
      } else {
        switch (get_num_components()) {
        case 2:
          return new Packer_point_float64_2;
        case 3:
          return new Packer_point_float64_3;
        case 4:
          return new Packer_point_float64_4;
        }
      }
      break;
    default:
      break;
    }
    return new Packer_point;

  case C_color:
    switch (get_numeric_type()) {
    case NT_uint8:
      switch (get_num_components()) {
      case 4:
        return new Packer_rgba_uint8_4;
        
      default:
        break;
      }
      break;
    case NT_packed_dabc:
      switch (get_num_components()) {
      case 1:
        return new Packer_argb_packed;
        
      default:
        break;
      }
      break;
    case NT_float32:
      switch (get_num_components()) {
      case 4:
        if (sizeof(float) == sizeof(PN_float32)) {
          // Use the native float type implementation for a tiny bit
          // more optimization.
          return new Packer_rgba_nativefloat_4;
        } else {
          return new Packer_rgba_float32_4;
        }
        
      default:
        break;
      }
      break;
    default:
      break;
    }
    return new Packer_color;

  default:
    // Otherwise, we just read it as a generic value.
    switch (get_numeric_type()) {
    case NT_float32:
      switch (get_num_components()) {
      case 3:
        if (sizeof(float) == sizeof(PN_float32)) {
          // Use the native float type implementation for a tiny bit
          // more optimization.
          return new Packer_nativefloat_3;
        } else {
          return new Packer_float32_3;
        }

      default:
        break;
      }
      break;
    case NT_float64:
      switch (get_num_components()) {
      case 3:
        if (sizeof(double) == sizeof(PN_float64)) {
          // Use the native float type implementation for a tiny bit
          // more optimization.
          return new Packer_nativedouble_3;
        } else {
          return new Packer_float64_3;
        }

      default:
        break;
      }
      break;
    default:
      break;
    }
    return new Packer;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::write_datagram
//       Access: Public
//  Description: Writes the contents of this object to the datagram
//               for shipping out to a Bam file.
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
write_datagram(BamWriter *manager, Datagram &dg) {
  manager->write_pointer(dg, _name);
  dg.add_uint8(_num_components);
  dg.add_uint8(_numeric_type);
  dg.add_uint8(_contents);
  dg.add_uint16(_start);
  dg.add_uint8(_column_alignment);
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::complete_pointers
//       Access: Public
//  Description: Receives an array of pointers, one for each time
//               manager->read_pointer() was called in fillin().
//               Returns the number of pointers processed.
////////////////////////////////////////////////////////////////////
int GeomVertexColumn::
complete_pointers(TypedWritable **p_list, BamReader *manager) {
  int pi = 0;

  _name = DCAST(InternalName, p_list[pi++]);

  return pi;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::fillin
//       Access: Protected
//  Description: This internal function is called by make_from_bam to
//               read in all of the relevant data from the BamFile for
//               the new GeomVertexColumn.
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::
fillin(DatagramIterator &scan, BamReader *manager) {
  manager->read_pointer(scan);

  _num_components = scan.get_uint8();
  _numeric_type = (NumericType)scan.get_uint8();
  _contents = (Contents)scan.get_uint8();
  _start = scan.get_uint16();

  _column_alignment = 1;
  if (manager->get_file_minor_ver() >= 29) {
    _column_alignment = scan.get_uint8();
  }

  setup();
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::Destructor
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
GeomVertexColumn::Packer::
~Packer() {
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data1f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
float GeomVertexColumn::Packer::
get_data1f(const unsigned char *pointer) {
  switch (_column->get_numeric_type()) {
  case NT_uint8:
    return maybe_scale_color_f(*pointer);

  case NT_uint16:
    return *(const PN_uint16 *)pointer;

  case NT_uint32:
    return *(const PN_uint32 *)pointer;

  case NT_packed_dcba:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword));
    }

  case NT_packed_dabc:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword));
    }

  case NT_float32:
    return *(const PN_float32 *)pointer;

  case NT_float64:
    return *(const PN_float64 *)pointer;

  default:
    nassertr(false, 0.0f);
  }

  return 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2f &GeomVertexColumn::Packer::
get_data2f(const unsigned char *pointer) {
  if (_column->get_num_values() == 1) {
    _v2.set(get_data1f(pointer), 0.0f);
    return _v2;

  } else {
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_f(pointer[0], pointer[1]);
      return _v2;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v2.set(pi[0], pi[1]);
      }
      return _v2;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v2.set(pi[0], pi[1]);
      }
      return _v2;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v2.set(pi[0], pi[1]);
      }
      return _v2;

    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v2.set(pi[0], pi[1]);
      }
      return _v2;

    case NT_stdfloat:
      nassertr(false, _v2);
    }
  }

  return _v2;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer::
get_data3f(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v3.set(get_data1f(pointer), 0.0f, 0.0f);
    return _v3;

  case 2:
    {
      const LVecBase2f &v2 = get_data2f(pointer);
      _v3.set(v2[0], v2[1], 0.0f);
    }
    return _v3;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_f(pointer[0], pointer[1], pointer[2]);
      return _v3;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v3.set(pi[0], pi[1], pi[2]);
      }
      return _v3;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v3.set(pi[0], pi[1], pi[2]);
      }
      return _v3;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword));
      }
      return _v3;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword));
      }
      return _v3;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v3.set(pi[0], pi[1], pi[2]);
      }
      return _v3;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v3.set(pi[0], pi[1], pi[2]);
      }
      return _v3;

    case NT_stdfloat:
      nassertr(false, _v3);
    }
  }

  return _v3;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer::
get_data4f(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4.set(get_data1f(pointer), 0.0f, 0.0f, 0.0f);
    return _v4;

  case 2:
    {
      const LVecBase2f &v2 = get_data2f(pointer);
      _v4.set(v2[0], v2[1], 0.0f, 0.0f);
    }
    return _v4;

  case 3:
    {
      const LVecBase3f &v3 = get_data3f(pointer);
      _v4.set(v3[0], v3[1], v3[2], 0.0f);
    }
    return _v4;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_f(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;

    case NT_stdfloat:
      nassertr(false, _v4);
    }
  }

  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data1d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
double GeomVertexColumn::Packer::
get_data1d(const unsigned char *pointer) {
  switch (_column->get_numeric_type()) {
  case NT_uint8:
    return maybe_scale_color_d(*pointer);

  case NT_uint16:
    return *(const PN_uint16 *)pointer;

  case NT_uint32:
    return *(const PN_uint32 *)pointer;

  case NT_packed_dcba:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword));
    }

  case NT_packed_dabc:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword));
    }

  case NT_float32:
    return *(const PN_float32 *)pointer;

  case NT_float64:
    return *(const PN_float64 *)pointer;

  case NT_stdfloat:
    nassertr(false, 0.0f);
  }

  return 0.0f;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2d &GeomVertexColumn::Packer::
get_data2d(const unsigned char *pointer) {
  if (_column->get_num_values() == 1) {
    _v2d.set(get_data1d(pointer), 0.0f);
    return _v2d;

  } else {
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_d(pointer[0], pointer[1]);
      return _v2d;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v2d.set(pi[0], pi[1]);
      }
      return _v2d;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v2d.set(pi[0], pi[1]);
      }
      return _v2d;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2d;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2d;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v2d.set(pi[0], pi[1]);
      }
      return _v2d;

    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v2d.set(pi[0], pi[1]);
      }
      return _v2d;

    case NT_stdfloat:
      nassertr(false, _v2d);
    }
  }

  return _v2d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer::
get_data3d(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v3d.set(get_data1d(pointer), 0.0f, 0.0f);
    return _v3d;

  case 2:
    {
      const LVecBase2d &v2 = get_data2d(pointer);
      _v3d.set(v2[0], v2[1], 0.0f);
    }
    return _v3d;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_d(pointer[0], pointer[1], pointer[2]);
      return _v3d;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v3d.set(pi[0], pi[1], pi[2]);
      }
      return _v3d;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v3d.set(pi[0], pi[1], pi[2]);
      }
      return _v3d;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword));
      }
      return _v3d;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword));
      }
      return _v3d;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v3d.set(pi[0], pi[1], pi[2]);
      }
      return _v3d;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v3d.set(pi[0], pi[1], pi[2]);
      }
      return _v3d;

    case NT_stdfloat:
      nassertr(false, _v3d);
    }
  }

  return _v3d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4d &GeomVertexColumn::Packer::
get_data4d(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4d.set(get_data1d(pointer), 0.0f, 0.0f, 0.0f);
    return _v4d;

  case 2:
    {
      const LVecBase2d &v2 = get_data2d(pointer);
      _v4d.set(v2[0], v2[1], 0.0f, 0.0f);
    }
    return _v4d;

  case 3:
    {
      const LVecBase3d &v3 = get_data3d(pointer);
      _v4d.set(v3[0], v3[1], v3[2], 0.0f);
    }
    return _v4d;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_d(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4d;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;

    case NT_stdfloat:
      nassertr(false, _v4d);
    }
  }

  return _v4d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data1i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
int GeomVertexColumn::Packer::
get_data1i(const unsigned char *pointer) {
  switch (_column->get_numeric_type()) {
  case NT_uint8:
    return *pointer;

  case NT_uint16:
    return *(const PN_uint16 *)pointer;

  case NT_uint32:
    return *(const PN_uint32 *)pointer;

  case NT_packed_dcba:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return GeomVertexData::unpack_abcd_d(dword);
    }

  case NT_packed_dabc:
    {
      PN_uint32 dword = *(const PN_uint32 *)pointer;
      return GeomVertexData::unpack_abcd_b(dword);
    }

  case NT_float32:
    return (int)*(const PN_float32 *)pointer;

  case NT_float64:
    return (int)*(const PN_float64 *)pointer;

  case NT_stdfloat:
    nassertr(false, 0);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data2i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2i &GeomVertexColumn::Packer::
get_data2i(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v2i.set(get_data1i(pointer), 0);
    return _v2i;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      _v2i.set(pointer[0], pointer[1]);
      return _v2i;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v2i.set(pi[0], pi[1]);
      }
      return _v2i;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v2i.set(pi[0], pi[1]);
      }
      return _v2i;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v2i.set(GeomVertexData::unpack_abcd_d(dword),
                 GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2i;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v2i.set(GeomVertexData::unpack_abcd_b(dword),
                 GeomVertexData::unpack_abcd_c(dword));
      }
      return _v2i;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v2i.set((int)pi[0], (int)pi[1]);
      }
      return _v2i;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v2i.set((int)pi[0], (int)pi[1]);
      }
      return _v2i;

    case NT_stdfloat:
      nassertr(false, _v2i);
    }
  }

  return _v2i;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data3i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3i &GeomVertexColumn::Packer::
get_data3i(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v3i.set(get_data1i(pointer), 0, 0);
    return _v3i;

  case 2:
    {
      const LVecBase2i &v2 = get_data2i(pointer);
      _v3i.set(v2[0], v2[1], 0);
    }
    return _v3i;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      _v3i.set(pointer[0], pointer[1], 0);
      return _v3i;

    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v3i.set(pi[0], pi[1], pi[2]);
      }
      return _v3i;

    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v3i.set(pi[0], pi[1], pi[2]);
      }
      return _v3i;

    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v3i.set(GeomVertexData::unpack_abcd_d(dword),
                 GeomVertexData::unpack_abcd_c(dword),
                 GeomVertexData::unpack_abcd_b(dword));
      }
      return _v3i;

    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v3i.set(GeomVertexData::unpack_abcd_b(dword),
                 GeomVertexData::unpack_abcd_c(dword),
                 GeomVertexData::unpack_abcd_d(dword));
      }
      return _v3i;

    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v3i.set((int)pi[0], (int)pi[1], (int)pi[2]);
      }
      return _v3i;

    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v3i.set((int)pi[0], (int)pi[1], (int)pi[2]);
      }
      return _v3i;

    case NT_stdfloat:
      nassertr(false, _v3i);
    }
  }

  return _v3i;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::get_data4i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4i &GeomVertexColumn::Packer::
get_data4i(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4i.set(get_data1i(pointer), 0, 0, 0);
    return _v4i;

  case 2:
    {
      const LVecBase2i &v2 = get_data2i(pointer);
      _v4i.set(v2[0], v2[1], 0, 0);
    }
    return _v4i;

  case 3:
    {
      const LVecBase3i &v3 = get_data3i(pointer);
      _v4i.set(v3[0], v3[1], v3[2], 0);
    }
    return _v4i;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      _v4i.set(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4i;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4i.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4i;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4i.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4i;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v4i.set(GeomVertexData::unpack_abcd_d(dword),
                 GeomVertexData::unpack_abcd_c(dword),
                 GeomVertexData::unpack_abcd_b(dword),
                 GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4i;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        _v4i.set(GeomVertexData::unpack_abcd_b(dword),
                 GeomVertexData::unpack_abcd_c(dword),
                 GeomVertexData::unpack_abcd_d(dword),
                 GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4i;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4i.set((int)pi[0], (int)pi[1], (int)pi[2], (int)pi[3]);
      }
      return _v4i;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4i.set((int)pi[0], (int)pi[1], (int)pi[2], (int)pi[3]);
      }
      return _v4i;

    case NT_stdfloat:
      nassertr(false, _v4i);
    }
  }

  return _v4i;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data1f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data1f(unsigned char *pointer, float data) {
  switch (_column->get_num_values()) {
  case 1:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      *pointer = maybe_unscale_color_f(data);
      break;
      
    case NT_uint16:
      *(PN_uint16 *)pointer = (unsigned int)data;
      break;
      
    case NT_uint32:
      *(PN_uint32 *)pointer = (unsigned int)data;
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      *(PN_float32 *)pointer = data;
      break;
      
    case NT_float64:
      *(PN_float64 *)pointer = data;
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 2:
    set_data2f(pointer, LVecBase2f(data, 0.0f));
    break;

  case 3:
    set_data3f(pointer, LVecBase3f(data, 0.0f, 0.0f));
    break;

  case 4:
    set_data4f(pointer, LVecBase4f(data, 0.0f, 0.0f, 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data2f(unsigned char *pointer, const LVecBase2f &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1f(pointer, data[0]);

  case 2:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_f(data);
      pointer[0] = _a;
      pointer[1] = _b;
      break;
      
    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
      }
      break;
      
    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 3:
    set_data3f(pointer, LVecBase3f(data[0], data[1], 0.0f));
    break;

  default:
    set_data4f(pointer, LVecBase4f(data[0], data[1], 0.0f, 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data3f(unsigned char *pointer, const LVecBase3f &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1f(pointer, data[0]);
    break;

  case 2:
    set_data2f(pointer, LVecBase2f(data[0], data[1]));
    break;
    
  case 3:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_f(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      break;
      
    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
      }
      break;
      
    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  default:
    set_data4f(pointer, LVecBase4f(data[0], data[1], data[2], 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1f(pointer, data[0]);
    break;

  case 2:
    set_data2f(pointer, LVecBase2f(data[0], data[1]));
    break;

  case 3:
    set_data3f(pointer, LVecBase3f(data[0], data[1], data[2]));
    break;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_f(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      pointer[3] = _d;
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;
      
    case NT_packed_dcba:
      maybe_unscale_color_f(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _c, _b, _a);
      break;
      
    case NT_packed_dabc:
      maybe_unscale_color_f(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _a, _b, _c);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data1d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data1d(unsigned char *pointer, double data) {
  switch (_column->get_num_values()) {
  case 1:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      *pointer = maybe_unscale_color_d(data);
      break;
      
    case NT_uint16:
      *(PN_uint16 *)pointer = (unsigned int)data;
      break;
      
    case NT_uint32:
      *(PN_uint32 *)pointer = (unsigned int)data;
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      *(PN_float32 *)pointer = data;
      break;
      
    case NT_float64:
      *(PN_float64 *)pointer = data;
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 2:
    set_data2d(pointer, LVecBase2d(data, 0.0f));
    break;

  case 3:
    set_data3d(pointer, LVecBase3d(data, 0.0f, 0.0f));
    break;

  case 4:
    set_data4d(pointer, LVecBase4d(data, 0.0f, 0.0f, 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data2d(unsigned char *pointer, const LVecBase2d &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1d(pointer, data[0]);

  case 2:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_d(data);
      pointer[0] = _a;
      pointer[1] = _b;
      break;
      
    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
      }
      break;
      
    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 3:
    set_data3d(pointer, LVecBase3d(data[0], data[1], 0.0f));
    break;

  default:
    set_data4d(pointer, LVecBase4d(data[0], data[1], 0.0f, 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data3d(unsigned char *pointer, const LVecBase3d &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1d(pointer, data[0]);
    break;

  case 2:
    set_data2d(pointer, LVecBase2d(data[0], data[1]));
    break;
    
  case 3:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_d(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      break;
      
    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
      }
      break;
      
    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  default:
    set_data4d(pointer, LVecBase4d(data[0], data[1], data[2], 0.0f));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data4d(unsigned char *pointer, const LVecBase4d &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1d(pointer, data[0]);
    break;

  case 2:
    set_data2d(pointer, LVecBase2d(data[0], data[1]));
    break;

  case 3:
    set_data3d(pointer, LVecBase3d(data[0], data[1], data[2]));
    break;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_d(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      pointer[3] = _d;
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;
      
    case NT_packed_dcba:
      maybe_unscale_color_d(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _c, _b, _a);
      break;
      
    case NT_packed_dabc:
      maybe_unscale_color_d(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _a, _b, _c);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data1i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data1i(unsigned char *pointer, int data) {
  switch (_column->get_num_values()) {
  case 1:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      *pointer = data;
      nassertv((*pointer) == data);
      break;
      
    case NT_uint16:
      *(PN_uint16 *)pointer = data;
      nassertv(*(PN_uint16 *)pointer == data);
      break;
      
    case NT_uint32:
      *(PN_uint32 *)pointer = data;
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      *(PN_float32 *)pointer = (float)data;
      break;
      
    case NT_float64:
      *(PN_float64 *)pointer = (double)data;
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 2:
    set_data2i(pointer, LVecBase2i(data, 0));
    break;

  case 3:
    set_data3i(pointer, LVecBase3i(data, 0, 0));
    break;

  default:
    set_data4i(pointer, LVecBase4i(data, 0, 0, 0));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data2i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data2i(unsigned char *pointer, const LVecBase2i &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1i(pointer, data[0]);
    break;

  case 2:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      pointer[0] = data[0];
      pointer[1] = data[1];
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  case 3:
    set_data3i(pointer, LVecBase3i(data[0], data[1], 0));
    break;

  default:
    set_data4i(pointer, LVecBase4i(data[0], data[1], 0, 0));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data3i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data3i(unsigned char *pointer, const LVecBase3i &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1i(pointer, data[0]);
    break;

  case 2:
    set_data2i(pointer, LVecBase2i(data[0], data[1]));
    break;

  case 3:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      pointer[0] = data[0];
      pointer[1] = data[1];
      pointer[2] = data[2];
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;
      
    case NT_packed_dcba:
    case NT_packed_dabc:
      nassertv(false);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;

  default:
    set_data4i(pointer, LVecBase4i(data[0], data[1], data[2], 0));
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer::set_data4i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer::
set_data4i(unsigned char *pointer, const LVecBase4i &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1i(pointer, data[0]);
    break;

  case 2:
    set_data2i(pointer, LVecBase2i(data[0], data[1]));
    break;

  case 3:
    set_data3i(pointer, LVecBase3i(data[0], data[1], data[2]));
    break;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      pointer[0] = data[0];
      pointer[1] = data[1];
      pointer[2] = data[2];
      pointer[3] = data[3];
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_packed_dcba:
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(data[3], data[2], data[1], data[0]);
      break;
      
    case NT_packed_dabc:
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(data[3], data[0], data[1], data[2]);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data1f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
float GeomVertexColumn::Packer_point::
get_data1f(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4f &v4 = get_data4f(pointer);
    return v4[0] / v4[3];
  } else {
    return Packer::get_data1f(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2f &GeomVertexColumn::Packer_point::
get_data2f(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4f &v4 = get_data4f(pointer);
    _v2.set(v4[0] / v4[3], v4[1] / v4[3]);
    return _v2;
  } else {
    return Packer::get_data2f(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer_point::
get_data3f(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4f &v4 = get_data4f(pointer);
    _v3.set(v4[0] / v4[3], v4[1] / v4[3], v4[2] / v4[3]);
    return _v3;
  } else {
    return Packer::get_data3f(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_point::
get_data4f(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4.set(get_data1i(pointer), 0.0f, 0.0f, 1.0f);
    return _v4;

  case 2:
    {
      const LVecBase2f &v2 = get_data2f(pointer);
      _v4.set(v2[0], v2[1], 0.0f, 1.0f);
    }
    return _v4;

  case 3:
    {
      const LVecBase3f &v3 = get_data3f(pointer);
      _v4.set(v3[0], v3[1], v3[2], 1.0f);
    }
    return _v4;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_f(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;

    case NT_stdfloat:
      nassertr(false, _v4);
    }
  }

  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data1d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
double GeomVertexColumn::Packer_point::
get_data1d(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4d &v4 = get_data4d(pointer);
    return v4[0] / v4[3];
  } else {
    return Packer::get_data1d(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2d &GeomVertexColumn::Packer_point::
get_data2d(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4d &v4 = get_data4d(pointer);
    _v2d.set(v4[0] / v4[3], v4[1] / v4[3]);
    return _v2d;
  } else {
    return Packer::get_data2d(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer_point::
get_data3d(const unsigned char *pointer) {
  if (_column->get_num_values() == 4) {
    const LVecBase4d &v4 = get_data4d(pointer);
    _v3d.set(v4[0] / v4[3], v4[1] / v4[3], v4[2] / v4[3]);
    return _v3d;
  } else {
    return Packer::get_data3d(pointer);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::get_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4d &GeomVertexColumn::Packer_point::
get_data4d(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4d.set(get_data1i(pointer), 0.0f, 0.0f, 1.0f);
    return _v4d;

  case 2:
    {
      const LVecBase2d &v2 = get_data2d(pointer);
      _v4d.set(v2[0], v2[1], 0.0f, 1.0f);
    }
    return _v4d;

  case 3:
    {
      const LVecBase3d &v3 = get_data3d(pointer);
      _v4d.set(v3[0], v3[1], v3[2], 1.0f);
    }
    return _v4d;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_d(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4d;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;

    case NT_stdfloat:
      nassertr(false, _v4d);
    }
  }

  return _v4d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data1f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data1f(unsigned char *pointer, float data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data, 0.0f, 0.0f, 1.0f));
  } else {
    Packer::set_data1f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data2f(unsigned char *pointer, const LVecBase2f &data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data[0], data[1], 0.0f, 1.0f));
  } else {
    Packer::set_data2f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data3f(unsigned char *pointer, const LVecBase3f &data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data[0], data[1], data[2], 1.0f));
  } else {
    Packer::set_data3f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1f(pointer, data[0] / data[3]);
    break;

  case 2:
    set_data2f(pointer, LVecBase2f(data[0] / data[3], data[1] / data[3]));
    break;

  case 3:
    set_data3f(pointer, LVecBase3f(data[0] / data[3], data[1] / data[3], data[2] / data[3]));
    break;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_f(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      pointer[3] = _d;
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;
      
    case NT_packed_dcba:
      maybe_unscale_color_f(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _c, _b, _a);
      break;
      
    case NT_packed_dabc:
      maybe_unscale_color_f(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _a, _b, _c);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data1d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data1d(unsigned char *pointer, double data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data, 0.0f, 0.0f, 1.0f));
  } else {
    Packer::set_data1d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data2d(unsigned char *pointer, const LVecBase2d &data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data[0], data[1], 0.0f, 1.0f));
  } else {
    Packer::set_data2d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data3d(unsigned char *pointer, const LVecBase3d &data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data[0], data[1], data[2], 1.0f));
  } else {
    Packer::set_data3d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point::set_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point::
set_data4d(unsigned char *pointer, const LVecBase4d &data) {
  switch (_column->get_num_values()) {
  case 1:
    set_data1d(pointer, data[0] / data[3]);
    break;

  case 2:
    set_data2d(pointer, LVecBase2d(data[0] / data[3], data[1] / data[3]));
    break;

  case 3:
    set_data3d(pointer, LVecBase3d(data[0] / data[3], data[1] / data[3], data[2] / data[3]));
    break;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_unscale_color_d(data);
      pointer[0] = _a;
      pointer[1] = _b;
      pointer[2] = _c;
      pointer[3] = _d;
      break;

    case NT_uint16:
      {
        PN_uint16 *pi = (PN_uint16 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;

    case NT_uint32:
      {
        PN_uint32 *pi = (PN_uint32 *)pointer;
        pi[0] = (unsigned int)data[0];
        pi[1] = (unsigned int)data[1];
        pi[2] = (unsigned int)data[2];
        pi[3] = (unsigned int)data[3];
      }
      break;
      
    case NT_packed_dcba:
      maybe_unscale_color_d(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _c, _b, _a);
      break;
      
    case NT_packed_dabc:
      maybe_unscale_color_d(data);
      *(PN_uint32 *)pointer = GeomVertexData::pack_abcd(_d, _a, _b, _c);
      break;
      
    case NT_float32:
      {
        PN_float32 *pi = (PN_float32 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;
      
    case NT_float64:
      {
        PN_float64 *pi = (PN_float64 *)pointer;
        pi[0] = data[0];
        pi[1] = data[1];
        pi[2] = data[2];
        pi[3] = data[3];
      }
      break;

    case NT_stdfloat:
      nassertv(false);
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_color::
get_data4f(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4.set(get_data1i(pointer), 0.0f, 0.0f, 1.0f);
    return _v4;

  case 2:
    {
      const LVecBase2f &v2 = get_data2f(pointer);
      _v4.set(v2[0], v2[1], 0.0f, 1.0f);
    }
    return _v4;

  case 3:
    {
      const LVecBase3f &v3 = get_data3f(pointer);
      _v4.set(v3[0], v3[1], v3[2], 1.0f);
    }
    return _v4;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_f(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_f(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4;

    case NT_stdfloat:
      nassertr(false, _v4);
    }
  }

  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::get_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4d &GeomVertexColumn::Packer_color::
get_data4d(const unsigned char *pointer) {
  switch (_column->get_num_values()) {
  case 1:
    _v4d.set(get_data1i(pointer), 0.0f, 0.0f, 1.0f);
    return _v4d;

  case 2:
    {
      const LVecBase2f &v2 = get_data2f(pointer);
      _v4d.set(v2[0], v2[1], 0.0f, 1.0f);
    }
    return _v4d;

  case 3:
    {
      const LVecBase3f &v3 = get_data3f(pointer);
      _v4d.set(v3[0], v3[1], v3[2], 1.0f);
    }
    return _v4d;

  default:
    switch (_column->get_numeric_type()) {
    case NT_uint8:
      maybe_scale_color_d(pointer[0], pointer[1], pointer[2], pointer[3]);
      return _v4d;
      
    case NT_uint16:
      {
        const PN_uint16 *pi = (const PN_uint16 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_uint32:
      {
        const PN_uint32 *pi = (const PN_uint32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_packed_dcba:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_packed_dabc:
      {
        PN_uint32 dword = *(const PN_uint32 *)pointer;
        maybe_scale_color_d(GeomVertexData::unpack_abcd_b(dword),
                            GeomVertexData::unpack_abcd_c(dword),
                            GeomVertexData::unpack_abcd_d(dword),
                            GeomVertexData::unpack_abcd_a(dword));
      }
      return _v4d;
      
    case NT_float32:
      {
        const PN_float32 *pi = (const PN_float32 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;
      
    case NT_float64:
      {
        const PN_float64 *pi = (const PN_float64 *)pointer;
        _v4d.set(pi[0], pi[1], pi[2], pi[3]);
      }
      return _v4d;

    case NT_stdfloat:
      nassertr(false, _v4d);
    }
  }

  return _v4d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data1f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data1f(unsigned char *pointer, float data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data, 0.0f, 0.0f, 1.0f));
  } else {
    Packer::set_data1f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data2f(unsigned char *pointer, const LVecBase2f &data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data[0], data[1], 0.0f, 1.0f));
  } else {
    Packer::set_data2f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data3f(unsigned char *pointer, const LVecBase3f &data) {
  if (_column->get_num_values() == 4) {
    set_data4f(pointer, LVecBase4f(data[0], data[1], data[2], 1.0f));
  } else {
    Packer::set_data3f(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data1d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data1d(unsigned char *pointer, double data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data, 0.0f, 0.0f, 1.0f));
  } else {
    Packer::set_data1d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data2d(unsigned char *pointer, const LVecBase2d &data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data[0], data[1], 0.0f, 1.0f));
  } else {
    Packer::set_data2d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_color::set_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_color::
set_data3d(unsigned char *pointer, const LVecBase3d &data) {
  if (_column->get_num_values() == 4) {
    set_data4d(pointer, LVecBase4d(data[0], data[1], data[2], 1.0f));
  } else {
    Packer::set_data3d(pointer, data);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_float32_3::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer_float32_3::
get_data3f(const unsigned char *pointer) {
  const PN_float32 *pi = (const PN_float32 *)pointer;
  _v3.set(pi[0], pi[1], pi[2]);
  return _v3;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_float32_3::set_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_float32_3::
set_data3f(unsigned char *pointer, const LVecBase3f &data) {
  PN_float32 *pi = (PN_float32 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_2::get_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2f &GeomVertexColumn::Packer_point_float32_2::
get_data2f(const unsigned char *pointer) {
  const PN_float32 *pi = (const PN_float32 *)pointer;
  _v2.set(pi[0], pi[1]);
  return _v2;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_2::set_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float32_2::
set_data2f(unsigned char *pointer, const LVecBase2f &data) {
  PN_float32 *pi = (PN_float32 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_3::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer_point_float32_3::
get_data3f(const unsigned char *pointer) {
  const PN_float32 *pi = (const PN_float32 *)pointer;
  _v3.set(pi[0], pi[1], pi[2]);
  return _v3;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_3::set_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float32_3::
set_data3f(unsigned char *pointer, const LVecBase3f &data) {
  PN_float32 *pi = (PN_float32 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_4::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_point_float32_4::
get_data4f(const unsigned char *pointer) {
  const PN_float32 *pi = (const PN_float32 *)pointer;
  _v4.set(pi[0], pi[1], pi[2], pi[3]);
  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float32_4::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float32_4::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  PN_float32 *pi = (PN_float32 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
  pi[3] = data[3];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_nativefloat_3::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer_nativefloat_3::
get_data3f(const unsigned char *pointer) {
  return *(const LVecBase3f *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativefloat_2::get_data2f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2f &GeomVertexColumn::Packer_point_nativefloat_2::
get_data2f(const unsigned char *pointer) {
  return *(const LVecBase2f *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativefloat_3::get_data3f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3f &GeomVertexColumn::Packer_point_nativefloat_3::
get_data3f(const unsigned char *pointer) {
  return *(const LVecBase3f *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativefloat_4::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_point_nativefloat_4::
get_data4f(const unsigned char *pointer) {
  return *(const LVecBase4f *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_float64_3::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer_float64_3::
get_data3d(const unsigned char *pointer) {
  const PN_float64 *pi = (const PN_float64 *)pointer;
  _v3d.set(pi[0], pi[1], pi[2]);
  return _v3d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_float64_3::set_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_float64_3::
set_data3d(unsigned char *pointer, const LVecBase3d &data) {
  PN_float64 *pi = (PN_float64 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_2::get_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2d &GeomVertexColumn::Packer_point_float64_2::
get_data2d(const unsigned char *pointer) {
  const PN_float64 *pi = (const PN_float64 *)pointer;
  _v2d.set(pi[0], pi[1]);
  return _v2d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_2::set_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float64_2::
set_data2d(unsigned char *pointer, const LVecBase2d &data) {
  PN_float64 *pi = (PN_float64 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_3::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer_point_float64_3::
get_data3d(const unsigned char *pointer) {
  const PN_float64 *pi = (const PN_float64 *)pointer;
  _v3d.set(pi[0], pi[1], pi[2]);
  return _v3d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_3::set_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float64_3::
set_data3d(unsigned char *pointer, const LVecBase3d &data) {
  PN_float64 *pi = (PN_float64 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_4::get_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4d &GeomVertexColumn::Packer_point_float64_4::
get_data4d(const unsigned char *pointer) {
  const PN_float64 *pi = (const PN_float64 *)pointer;
  _v4d.set(pi[0], pi[1], pi[2], pi[3]);
  return _v4d;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_float64_4::set_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_point_float64_4::
set_data4d(unsigned char *pointer, const LVecBase4d &data) {
  PN_float64 *pi = (PN_float64 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
  pi[3] = data[3];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_nativedouble_3::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer_nativedouble_3::
get_data3d(const unsigned char *pointer) {
  return *(const LVecBase3d *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativedouble_2::get_data2d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase2d &GeomVertexColumn::Packer_point_nativedouble_2::
get_data2d(const unsigned char *pointer) {
  return *(const LVecBase2d *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativedouble_3::get_data3d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase3d &GeomVertexColumn::Packer_point_nativedouble_3::
get_data3d(const unsigned char *pointer) {
  return *(const LVecBase3d *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_point_nativedouble_4::get_data4d
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4d &GeomVertexColumn::Packer_point_nativedouble_4::
get_data4d(const unsigned char *pointer) {
  return *(const LVecBase4d *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_argb_packed::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_argb_packed::
get_data4f(const unsigned char *pointer) {
  PN_uint32 dword = *(const PN_uint32 *)pointer;
  _v4.set((float)GeomVertexData::unpack_abcd_b(dword) / 255.0f,
          (float)GeomVertexData::unpack_abcd_c(dword) / 255.0f,
          (float)GeomVertexData::unpack_abcd_d(dword) / 255.0f,
          (float)GeomVertexData::unpack_abcd_a(dword) / 255.0f);
  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_argb_packed::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_argb_packed::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  // when packing an argb, we want to make sure we cap 
  // the input values at 1 since going above one will cause 
  // the value to be truncated.
  float newData[4];
  for (int i = 0; i < 4; i++) {
    if (data[i] > 1.0)
      newData[i] = 1.0;
    else
      newData[i] = data[i];
  }
  *(PN_uint32 *)pointer = GeomVertexData::pack_abcd
    ((unsigned int)(newData[3] * 255.0f),
     (unsigned int)(newData[0] * 255.0f),
     (unsigned int)(newData[1] * 255.0f),
     (unsigned int)(newData[2] * 255.0f));
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_rgba_uint8_4::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_rgba_uint8_4::
get_data4f(const unsigned char *pointer) {
  _v4.set((float)pointer[0] / 255.0f,
          (float)pointer[1] / 255.0f,
          (float)pointer[2] / 255.0f,
          (float)pointer[3] / 255.0f);
  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_rgba_uint8_4::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_rgba_uint8_4::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  pointer[0] = (unsigned int)(data[0] * 255.0f);
  pointer[1] = (unsigned int)(data[1] * 255.0f);
  pointer[2] = (unsigned int)(data[2] * 255.0f);
  pointer[3] = (unsigned int)(data[3] * 255.0f);
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_rgba_float32_4::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_rgba_float32_4::
get_data4f(const unsigned char *pointer) {
  const PN_float32 *pi = (const PN_float32 *)pointer;
  _v4.set(pi[0], pi[1], pi[2], pi[3]);
  return _v4;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_rgba_float32_4::set_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_rgba_float32_4::
set_data4f(unsigned char *pointer, const LVecBase4f &data) {
  PN_float32 *pi = (PN_float32 *)pointer;
  pi[0] = data[0];
  pi[1] = data[1];
  pi[2] = data[2];
  pi[3] = data[3];
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_rgba_nativefloat_4::get_data4f
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
const LVecBase4f &GeomVertexColumn::Packer_rgba_nativefloat_4::
get_data4f(const unsigned char *pointer) {
  return *(const LVecBase4f *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_uint16_1::get_data1i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
int GeomVertexColumn::Packer_uint16_1::
get_data1i(const unsigned char *pointer) {
  return *(const PN_uint16 *)pointer;
}

////////////////////////////////////////////////////////////////////
//     Function: GeomVertexColumn::Packer_uint16_1::set_data1i
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void GeomVertexColumn::Packer_uint16_1::
set_data1i(unsigned char *pointer, int data) {
  *(PN_uint16 *)pointer = data;
  nassertv(*(PN_uint16 *)pointer == data);
}
