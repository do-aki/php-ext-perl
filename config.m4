dnl config.m4 for extension perl

PHP_ARG_WITH([perl],
  [for perl support],
  [AS_HELP_STRING([--with-perl[=DIR]],
    [Include perl support. DIR is the perl base directory.])])

if test "$PHP_PERL" != "no"; then
  for i in $PHP_PERL /usr/local /usr; do
    if test -x $i/bin/perl; then
      PERL_DIR=$i
      break
    fi
  done

  if test -z "$PERL_DIR"; then
    AC_MSG_ERROR([Cannot find perl executable under $PHP_PERL.])
  fi

  EXTRA_CFLAGS=`$PERL_DIR/bin/perl -MExtUtils::Embed -e ccopts`
  EXTRA_LDFLAGS=`$PERL_DIR/bin/perl -MExtUtils::Embed -e ldopts`
  PHP_SUBST(EXTRA_CFLAGS)
	LIBS=${LIBS}\ ${EXTRA_LDFLAGS}

  LIBNAME=perl
  LIBSYMBOL=perl_alloc
  PHP_CHECK_LIBRARY(perl,perl_alloc,
  [
    PHP_ADD_LIBRARY($LIBNAME, 1, EXTRA_CFLAGS)
    AC_HEADER_STDC
  ],[
    AC_MSG_ERROR([Library $LIBNAME was not found with the function $LIBSYMBOL in it.])
  ])

  AC_DEFINE(HAVE_PERL, 1, [ Have perl support ])

  PHP_NEW_EXTENSION(perl, zv_to_sv.c sv_to_zv.c  php_perl.c, $ext_shared)
fi
