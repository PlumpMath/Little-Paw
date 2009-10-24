// This directory contains the code for the panda3d.exe executable,
// the "standalone" part of the Panda3D plugin/runtime system.  Define
// HAVE_P3D_PLUGIN in your Config.pp to build it.

#define BUILD_DIRECTORY $[and $[HAVE_P3D_PLUGIN],$[HAVE_OPENSSL],$[HAVE_ZLIB],$[HAVE_TINYXML]]

#begin bin_target
  #define USE_PACKAGES openssl zlib
  #define TARGET panda3d

  #define LOCAL_LIBS plugin_common

  #define OTHER_LIBS \
    prc:c dtoolutil:c dtoolbase:c dtool:m \
    interrogatedb:c dconfig:c dtoolconfig:m \
    downloader:c express:c pandaexpress:m \
    pystub

  #define OSX_SYS_FRAMEWORKS Foundation AppKit Carbon

  #define SOURCES \
    panda3d.cxx panda3d.h panda3d.I \
    panda3dMain.cxx

  #define WIN_SYS_LIBS user32.lib gdi32.lib shell32.lib ole32.lib

#end bin_target

#begin bin_target
  // On Windows, we also need to build panda3dw.exe, the non-console
  // version of panda3d.exe.

  #define BUILD_TARGET $[WINDOWS_PLATFORM]
  #define USE_PACKAGES openssl zlib
  #define TARGET panda3dw

  #define LOCAL_LIBS plugin_common

  #define OTHER_LIBS \
    prc:c dtoolutil:c dtoolbase:c dtool:m \
    interrogatedb:c dconfig:c dtoolconfig:m \
    express:c downloader:c pandaexpress:m \
    pystub

  #define OSX_SYS_FRAMEWORKS Foundation AppKit Carbon

  #define SOURCES \
    panda3d.cxx panda3d.h panda3d.I \
    panda3dWinMain.cxx

  #define WIN_SYS_LIBS user32.lib gdi32.lib shell32.lib ole32.lib

#end bin_target

#begin bin_target
  // On Mac, we'll build panda3d_mac, which is the Carbon-friendly
  // application we wrap in a bundle, for picking a p3d file from
  // Finder.

  #define BUILD_TARGET $[OSX_PLATFORM]
  #define USE_PACKAGES openssl zlib
  #define TARGET panda3d_mac

  #define LOCAL_LIBS plugin_common

  #define OTHER_LIBS \
    prc:c dtoolutil:c dtoolbase:c dtool:m \
    interrogatedb:c dconfig:c dtoolconfig:m \
    downloader:c express:c pandaexpress:m \
    pystub

  #define OSX_SYS_FRAMEWORKS Foundation AppKit Carbon

  #define SOURCES \
    panda3d.cxx panda3d.h panda3d.I \
    panda3dMac.cxx panda3dMac.h panda3dMac.I

#end bin_target
