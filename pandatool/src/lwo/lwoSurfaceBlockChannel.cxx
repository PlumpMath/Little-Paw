// Filename: lwoSurfaceBlockChannel.cxx
// Created by:  drose (24Apr01)
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

#include "lwoSurfaceBlockChannel.h"
#include "lwoInputFile.h"

#include "dcast.h"
#include "indent.h"

TypeHandle LwoSurfaceBlockChannel::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: LwoSurfaceBlockChannel::read_iff
//       Access: Public, Virtual
//  Description: Reads the data of the chunk in from the given input
//               file, if possible.  The ID and length of the chunk
//               have already been read.  stop_at is the byte position
//               of the file to stop at (based on the current position
//               at in->get_bytes_read()).  Returns true on success,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool LwoSurfaceBlockChannel::
read_iff(IffInputFile *in, size_t stop_at) {
  LwoInputFile *lin = DCAST(LwoInputFile, in);

  _channel_id = lin->get_id();

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: LwoSurfaceBlockChannel::write
//       Access: Public, Virtual
//  Description:
////////////////////////////////////////////////////////////////////
void LwoSurfaceBlockChannel::
write(ostream &out, int indent_level) const {
  indent(out, indent_level)
    << get_id() << " { channel_id = " << _channel_id << " }\n";
}
