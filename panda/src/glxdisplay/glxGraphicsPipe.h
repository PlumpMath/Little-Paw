// Filename: glxGraphicsPipe.h
// Created by:  mike (09Jan97)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#ifndef GLXGRAPHICSPIPE_H
#define GLXGRAPHICSPIPE_H

#include "pandabase.h"
#include "graphicsPipe.h"

class glxGraphicsWindow;
class FrameBufferProperties;

#ifdef CPPPARSER
// A simple hack so interrogate can parse this file.
typedef int Display;
typedef int Window;
typedef int XErrorEvent;
typedef int XVisualInfo;
typedef int Atom;
#else
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif  // CPPPARSER

////////////////////////////////////////////////////////////////////
//       Class : glxGraphicsPipe
// Description : This graphics pipe represents the interface for
//               creating OpenGL graphics windows on an X-based
//               (e.g. Unix) client.
////////////////////////////////////////////////////////////////////
class glxGraphicsPipe : public GraphicsPipe {
public:
  glxGraphicsPipe(const string &display = string());
  virtual ~glxGraphicsPipe();

  virtual string get_interface_name() const;
  static PT(GraphicsPipe) pipe_constructor();

  INLINE Display *get_display() const;
  INLINE int get_screen() const;
  INLINE Window get_root() const;

  INLINE Atom get_wm_delete_window() const;

protected:
  virtual PT(GraphicsStateGuardian) make_gsg(const FrameBufferProperties &properties);
  virtual PT(GraphicsWindow) make_window(GraphicsStateGuardian *gsg);

private:
  XVisualInfo *choose_visual(FrameBufferProperties &properties) const;
  XVisualInfo *try_for_visual(int framebuffer_mode,
                              int want_depth_bits, int want_color_bits) const;

  static void install_error_handlers();
  static int error_handler(Display *display, XErrorEvent *error);
  static int io_error_handler(Display *display);

  Display *_display;
  int _screen;
  Window _root;

  Atom _wm_protocols;
  Atom _wm_delete_window;

  typedef int ErrorHandlerFunc(Display *, XErrorEvent *);
  typedef int IOErrorHandlerFunc(Display *);
  static bool _error_handlers_installed;
  static ErrorHandlerFunc *_prev_error_handler;
  static IOErrorHandlerFunc *_prev_io_error_handler;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    GraphicsPipe::init_type();
    register_type(_type_handle, "glxGraphicsPipe",
                  GraphicsPipe::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "glxGraphicsPipe.I"

#endif
