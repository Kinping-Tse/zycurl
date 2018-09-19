dnl config.m4 for extension zycurl

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(zycurl, for zycurl support,
dnl Make sure that the comment is aligned:
dnl [  --with-zycurl             Include zycurl support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(zycurl, whether to enable zycurl support,
dnl Make sure that the comment is aligned:
[  --enable-zycurl          Enable zycurl support], no)

if test "$PHP_ZYCURL" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=\`$PKG_CONFIG foo --cflags\`
  dnl     LIBFOO_LIBDIR=\`$PKG_CONFIG foo --libs\`
  dnl     LIBFOO_VERSON=\`$PKG_CONFIG foo --modversion\`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, ZYCURL_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-zycurl -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/zycurl.h"  # you most likely want to change this
  dnl if test -r $PHP_ZYCURL/$SEARCH_FOR; then # path given as parameter
  dnl   ZYCURL_DIR=$PHP_ZYCURL
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for zycurl files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ZYCURL_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ZYCURL_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the zycurl distribution])
  dnl fi

  dnl # --with-zycurl -> add include path
  dnl PHP_ADD_INCLUDE($ZYCURL_DIR/include)

  dnl # --with-zycurl -> check for lib and symbol presence
  dnl LIBNAME=ZYCURL # you may want to change this
  dnl LIBSYMBOL=ZYCURL # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ZYCURL_DIR/$PHP_LIBDIR, ZYCURL_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ZYCURLLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong zycurl lib version or lib not found])
  dnl ],[
  dnl   -L$ZYCURL_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ZYCURL_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_ZYCURL, 1, [ Have zycurl support ])

  PHP_NEW_EXTENSION(zycurl, zycurl.c, $ext_shared)
fi
