dnl $Id$
dnl config.m4 for extension perl

PHP_ARG_WITH(perl, for perl support,
[  --with-perl[=DIR]      Include perl support. DIR is the perl base directory.])

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
  PHP_SUBST(EXTRA_LDFLAGS)
dnl  PHP_EVAL_LIBLINE($PERL_LDFLAGS, PERL_SHARED_LIBADD)
dnl  PHP_EVAL_INCLINE($PERL_CFLAGS)

dnl  PHP_SUBST(PERL_SHARED_LIBADD)

dnl  PHP_ADD_LIBRARY_WITH_PATH(perl, $PERL_DIR, MYSQL_SHARED_LIBADD)
dnl  PHP_ADD_INCLUDE($PERL_DIR)
  AC_DEFINE(HAVE_PERL,1,[Whether you have perl])
  PHP_NEW_EXTENSION(perl, php_perl.c, $ext_shared)
fi
