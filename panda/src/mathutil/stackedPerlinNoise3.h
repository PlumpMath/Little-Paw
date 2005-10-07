// Filename: stackedPerlinNoise3.h
// Created by:  drose (05Oct05)
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

#ifndef STACKEDPERLINNOISE3_H
#define STACKEDPERLINNOISE3_H

#include "pandabase.h"
#include "perlinNoise3.h"
#include "pvector.h"

////////////////////////////////////////////////////////////////////
//       Class : StackedPerlinNoise3
// Description : Implements a multi-layer PerlinNoise, with one or
//               more high-frequency noise functions added to a
//               lower-frequency base noise function.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDA StackedPerlinNoise3 {
PUBLISHED:
  StackedPerlinNoise3(double sx, double sy, double sz, int num_levels = 3,
                      double scale_factor = 4.0f, double amp_scale = 0.5f,
                      int table_size = 256, unsigned long seed = 0);

  INLINE double noise(double x, double y, double z);
  INLINE float noise(const LVecBase3f &value);
  double noise(const LVecBase3d &value);

private:
  double _amp_scale;

  typedef pvector<PerlinNoise3> Noises;
  Noises _noises;
};

#include "stackedPerlinNoise3.I"

#endif

