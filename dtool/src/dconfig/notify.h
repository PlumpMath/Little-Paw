// Filename: notify.h
// Created by:  drose (28Feb00)
// 
////////////////////////////////////////////////////////////////////

#ifndef NOTIFY_H
#define NOTIFY_H

#include <dtoolbase.h>

#include "notifyCategory.h"
#include "notifySeverity.h"

#include <string>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////
// 	 Class : Notify
// Description : An object that handles general error reporting to the
//               user.  It contains a pointer to an ostream, initially
//               cerr, which can be reset at will to point to
//               different output devices, according to the needs of
//               the application.  All output generated within Panda
//               should vector through the Notify ostream.
//
//               This also includes a collection of Categories and
//               Severities, which may be independently enabled or
//               disabled, so that error messages may be squelched or
//               respected according to the wishes of the user.
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOL Notify {
PUBLISHED:
  Notify();
  ~Notify();

  void set_ostream_ptr(ostream *ostream_ptr, bool delete_later);
  ostream *get_ostream_ptr() const;

  typedef bool AssertHandler(const char *expression, int line,
			     const char *source_file);
  
  void set_assert_handler(AssertHandler *assert_handler);
  void clear_assert_handler();
  bool has_assert_handler() const;
  AssertHandler *get_assert_handler() const;

  bool has_assert_failed() const;
  const string &get_assert_error_message() const;
  void clear_assert_failed();

  NotifyCategory *get_top_category();
  NotifyCategory *get_category(const string &basename, 
			       NotifyCategory *parent_category);
  NotifyCategory *get_category(const string &basename, 
			       const string &parent_fullname);
  NotifyCategory *get_category(const string &fullname);

  static ostream &out();
  static ostream &null();
  static Notify *ptr();

  static long get_literal_flag();

  bool assert_failure(const char *expression, int line,
		      const char *source_file);

  static NotifySeverity string_severity(const string &string);

private:
  ostream *_ostream_ptr;
  bool _owns_ostream_ptr;
  ostream *_null_ostream_ptr;

  AssertHandler *_assert_handler;
  bool _assert_failed;
  string _assert_error_message;

  typedef map<string, NotifyCategory *> Categories;
  Categories _categories;

  static Notify *_global_ptr;
};


// This defines the symbol nout in the same way that cerr and cout are
// defined, for compactness of C++ code that uses Notify in its
// simplest form.  Maybe it's a good idea to define this symbol and
// maybe it's not, but it does seem that "nout" isn't likely to
// collide with any other name.

#define nout (Notify::out())

// Here are a couple of assert-type functions.  These are designed to
// avoid simply dumping core, since that's quite troublesome when the
// programmer is working in a higher-level environment that is calling
// into the C++ layer.

// nassertr() is intended to be used in functions that have return
// values; it returns the indicated value if the assertion fails.

// nassertv() is intended to be used in functions that do not have
// return values; it simply returns if the assertion fails.

// nassertd() does not return from the function, but instead executes
// the following block of code (like an if statement) if the assertion
// fails.

#ifdef NDEBUG

#define nassertr(condition, return_value)  
#define nassertv(condition)
#define nassertd(condition) if (false)
// We trust the compiler to optimize the above out.

#else   // NDEBUG

#define nassertr(condition, return_value) \
  { \
    if (!(condition)) { \
      if (Notify::ptr()->assert_failure(#condition, __LINE__, __FILE__)) { \
        return return_value; \
      } \
    } \
  }

#define nassertv(condition) \
  { \
    if (!(condition)) { \
      if (Notify::ptr()->assert_failure(#condition, __LINE__, __FILE__)) { \
        return; \
      } \
    } \
  }

#define nassertd(condition) \
  if (!(condition) && \
      Notify::ptr()->assert_failure(#condition, __LINE__, __FILE__))

#endif  // NDEBUG


#include "notify.I"

#endif
