/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Dmitry Stogov                                                |
   +----------------------------------------------------------------------+
   $Id$
*/

#ifndef PHP_PERL_H
#define PHP_PERL_H

#include "php.h"

extern zend_module_entry perl_module_entry;
#define phpext_perl_ptr &perl_module_entry

#ifdef PHP_WIN32
#define PHP_PERL_API __declspec(dllexport)
#else
#define PHP_PERL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#endif
