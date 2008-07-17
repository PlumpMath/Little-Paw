// Filename: virtualFileSystem.h
// Created by:  drose (03Aug02)
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

#ifndef VIRTUALFILESYSTEM_H
#define VIRTUALFILESYSTEM_H

#include "pandabase.h"

#include "virtualFile.h"
#include "filename.h"
#include "dSearchPath.h"
#include "pointerTo.h"
#include "config_express.h"
#include "pvector.h"

class Multifile;
class VirtualFileMount;
class VirtualFileComposite;

////////////////////////////////////////////////////////////////////
//       Class : VirtualFileSystem
// Description : A hierarchy of directories and files that appears to
//               be one continuous file system, even though the files
//               may originate from several different sources that may
//               not be related to the actual OS's file system.
//
//               For instance, a VirtualFileSystem can transparently
//               mount one or more Multifiles as their own
//               subdirectory hierarchies.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEXPRESS VirtualFileSystem {
PUBLISHED:
  VirtualFileSystem();
  ~VirtualFileSystem();

  enum MountFlags {
    MF_owns_pointer   = 0x0001,    // This flag is no longer used.
    MF_read_only      = 0x0002,
  };

  BLOCKING bool mount(Multifile *multifile, const string &mount_point, int flags);
  BLOCKING bool mount(const Filename &physical_filename, const string &mount_point, 
                      int flags, const string &password = "");
  BLOCKING int unmount(Multifile *multifile);
  BLOCKING int unmount(const Filename &physical_filename);
  BLOCKING int unmount_point(const string &mount_point);
  BLOCKING int unmount_all();

  BLOCKING bool chdir(const string &new_directory);
  BLOCKING const Filename &get_cwd() const;

  BLOCKING PT(VirtualFile) get_file(const Filename &filename) const;
  BLOCKING PT(VirtualFile) find_file(const Filename &filename, 
                                     const DSearchPath &searchpath) const;
  BLOCKING bool resolve_filename(Filename &filename, const DSearchPath &searchpath,
                                 const string &default_extension = string()) const;
  BLOCKING int find_all_files(const Filename &filename, const DSearchPath &searchpath,
                              DSearchPath::Results &results) const;

  BLOCKING INLINE bool exists(const Filename &filename) const;
  BLOCKING INLINE bool is_directory(const Filename &filename) const;
  BLOCKING INLINE bool is_regular_file(const Filename &filename) const;

  INLINE void ls(const string &filename) const;
  INLINE void ls_all(const string &filename) const;

  void write(ostream &out) const;

  static VirtualFileSystem *get_global_ptr();

  BLOCKING INLINE string read_file(const Filename &filename, bool auto_unwrap) const;
  BLOCKING istream *open_read_file(const Filename &filename, bool auto_unwrap) const;
  BLOCKING void close_read_file(istream *stream) const;

public:
  INLINE bool read_file(const Filename &filename, string &result, bool auto_unwrap) const;
  INLINE bool read_file(const Filename &filename, pvector<unsigned char> &result, bool auto_unwrap) const;

  void scan_mount_points(vector_string &names, const Filename &path) const;

private:
  Filename normalize_mount_point(const string &mount_point) const;
  bool found_match(PT(VirtualFile) &found_file, VirtualFileComposite *&composite_file,
                   VirtualFileMount *mount, const string &local_filename,
                   const Filename &original_filename, bool implicit_pz_file) const;
  static void parse_option(const string &option,
                           int &flags, string &password);

  typedef pvector<VirtualFileMount *> Mounts;
  Mounts _mounts;
  Filename _cwd;

  static VirtualFileSystem *_global_ptr;
};

#include "virtualFileSystem.I"

#endif
