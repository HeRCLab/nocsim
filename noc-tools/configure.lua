-- Public domain
--
-- Do not edit!
-- This file was generated from configure.in by BSDBuild 3.2.
--
-- To regenerate this file, get the latest BSDBuild release from
-- http://bsdbuild.hypertriton.com/, and use the command:
--
--    $ cat configure.in | mkconfigure > configure
--
hdefs = {}
mdefs = {}
hdefs["ENABLE_NLS"] = nil
hdefs["PREFIX"] = "/usr/local"
hdefs["BINDIR"] = "/usr/local/bin"
hdefs["LIBDIR"] = "/usr/local/lib"
hdefs["MODULEDIR"] = "/usr/local/lib"
hdefs["LIBEXECDIR"] = "/usr/local/libexec"
hdefs["DATADIR"] = "/usr/local/share"
hdefs["STATEDIR"] = "/usr/local/var"
hdefs["SYSCONFDIR"] = "/usr/local/etc"
hdefs["LOCALEDIR"] = "/usr/local/share/locale"
hdefs["MANDIR"] = "/usr/local/man"
hdefs["PACKAGE"] = "nocsim-gui"
hdefs["VERSION"] = "2.0.0"
hdefs["EXECSUFFIX"] = ""
hdefs["HAVE_CC_WARNINGS"] = "yes"
hdefs["HAVE_FLOAT"] = "yes"
hdefs["HAVE_LONG_DOUBLE"] = "yes"
hdefs["HAVE_LONG_LONG"] = "yes"
hdefs["HAVE_CYGWIN"] = nil
hdefs["HAVE_CC_MWINDOWS"] = nil
hdefs["HAVE_CC_MCONSOLE"] = nil
hdefs["HAVE_CC"] = "yes"
hdefs["HAVE_CC65"] = nil
hdefs["HAVE_AGAR"] = "yes"
hdefs["AGAR_CFLAGS"] = "-I/home/cad/.local/agar1.6//include/agar   -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16        -I/usr/include/libpng16          "
hdefs["AGAR_LIBS"] = "-L/home/cad/.local/agar1.6//lib -lag_gui -lag_core  -lfreetype -lfontconfig -lfreetype  -L/usr/local/lib -lGL -lX11 -lXinerama  -lm   -lpng16 -lpthread     -ldl    "
mdefs["AGAR_CFLAGS"] = "-I/home/cad/.local/agar1.6//include/agar   -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16        -I/usr/include/libpng16          "
mdefs["AGAR_LIBS"] = "-L/home/cad/.local/agar1.6//lib -lag_gui -lag_core  -lfreetype -lfontconfig -lfreetype  -L/usr/local/lib -lGL -lX11 -lXinerama  -lm   -lpng16 -lpthread     -ldl    "
mdefs["BINDIR"] = "/usr/local/bin"
mdefs["CC"] = "/usr/bin/cc"
mdefs["CC_COMPILE"] = "-c"
mdefs["CFLAGS"] = " -DENABLE_DEBUGGER -Wall -Wextra -pedantic -DEBUG -Og -g"
mdefs["DATADIR"] = "/usr/local/share"
mdefs["ENABLE_NLS"] = "no"
mdefs["EXECSUFFIX"] = ""
mdefs["HAVE_AGAR"] = "yes"
mdefs["HAVE_CC"] = "yes"
mdefs["HAVE_CC65"] = "no"
mdefs["HAVE_CC_MCONSOLE"] = "no"
mdefs["HAVE_CC_MWINDOWS"] = "no"
mdefs["HAVE_CC_WARNINGS"] = "yes"
mdefs["HAVE_CYGWIN"] = "no"
mdefs["HAVE_GETTEXT"] = "no"
mdefs["HAVE_LD_NO_UNDEFINED"] = ""
mdefs["HAVE_LD_STATIC_LIBGCC"] = ""
mdefs["HAVE_TCL"] = ""
mdefs["LIBDIR"] = "/usr/local/lib"
mdefs["LIBEXECDIR"] = "/usr/local/libexec"
mdefs["LIBTOOLOPTS_SHARED"] = ""
mdefs["LOCALEDIR"] = "/usr/local/share/locale"
mdefs["MANDIR"] = "/usr/local/man"
mdefs["MODULEDIR"] = "/usr/local/lib"
mdefs["PACKAGE"] = "nocsim-gui"
mdefs["PATH_SEPARATOR"] = ":"
mdefs["PKGCONFIG"] = "/usr/bin/pkg-config"
mdefs["PKGCONFIG_LIBDIR"] = "${PREFIX}/lib/pkgconfig"
mdefs["PROG_BUNDLE"] = ""
mdefs["PROG_CLI_CFLAGS"] = ""
mdefs["PROG_CLI_FLAGS"] = ""
mdefs["PROG_GUI_CFLAGS"] = ""
mdefs["PROG_GUI_FLAGS"] = ""
mdefs["PROG_PREFIX"] = ""
mdefs["PROG_SUFFIX"] = ""
mdefs["PROG_TRANSFORM"] = "s,x,x,"
mdefs["STATEDIR"] = "/usr/local/var"
mdefs["SYSCONFDIR"] = "/usr/local/etc"
mdefs["TCL_CFLAGS"] = "-I/usr/include/tcl8.6/"
mdefs["TCL_LIBS"] = "-ltcl -ltclstub"
mdefs["VERSION"] = "2.0.0"
