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
   | Author: Dmitry Stogov <dmitry@zend.com>                              |
   +----------------------------------------------------------------------+
   $Id$
*/

#ifdef COMPILE_DL_PERL
#define HAVE_PERL 1
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_PERL

#ifdef ZEND_WIN32
#  define _WINSOCK2API_           /* using winsock.h instead of winsock2.h */
#  define READDIR_H
#  define _INET_H_
#  define _NETDB_H_
#endif

#include <EXTERN.h>               /* from the Perl distribution */
#include <perl.h>                 /* from the Perl distribution */
#include <perliol.h>              /* from the Perl distribution */

//#include <XSUB.h>                 /* from the Perl distribution */

#define ST(off) PL_stack_base[ax + (off)]

#undef YYDEBUG                    /* bypass macros redeclaration warning */
#undef END_EXTERN_C               /* bypass macros redeclaration warning */

#include "php.h"
#include "php_ini.h"
#include "zend_objects_API.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "SAPI.h"
#include "php_perl.h"

ZEND_BEGIN_MODULE_GLOBALS(perl)
  PerlInterpreter *perl;
  HashTable       perl_objects; /* this hash is used to make one to one
                                   mapping between Perl and PHP objects */
ZEND_END_MODULE_GLOBALS(perl)

#ifdef ZTS
#define PERLG(v) TSRMG(perl_globals_id, zend_perl_globals *, v)
#else
#define PERLG(v) (perl_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(perl);

PHP_METHOD(Perl, eval);
PHP_METHOD(Perl, require);

static zend_function_entry perl_functions[] = {
  PHP_ME(Perl, eval, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Perl, require, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

/****************************************************************************/
/* This code was produced by `perl -MExtUtils::Embed -e xsinit`             */

EXTERN_C void xs_init (pTHX);

EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);

EXTERN_C void
xs_init(pTHX)
{
  char *file = __FILE__;
  dXSUB_SYS;

  /* DynaLoader is a special case */
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
/****************************************************************************/

typedef enum _perl_context {
  PERL_SCALAR,
  PERL_ARRAY,
  PERL_HASH
} perl_context;

typedef struct php_perl_object {
  zend_object   zo;
  SV           *sv;           /* Perl's representation of object */
  HashTable    *properties;   /* Temporary collection of object properties */
  perl_context  context;      /* flag for next perl call, property */
} php_perl_object;


static zend_class_entry* perl_class_entry;
static zend_class_entry* perl_exception_class_entry;

/* PHP <-> Perl data conversion routines */
static SV*  php_perl_zval_to_sv_noref(PerlInterpreter* my_perl, zval* zv, HashTable* var_hash TSRMLS_DC);
static SV*  php_perl_zval_to_sv_ref(PerlInterpreter* my_perl, zval* zv, HashTable* var_hash TSRMLS_DC);
static SV*  php_perl_zval_to_sv(PerlInterpreter* my_perl, zval* zv TSRMLS_DC);
static zval* php_perl_sv_to_zval_noref(PerlInterpreter* my_perl, SV* sv, zval* zv, HashTable* var_hash TSRMLS_DC);
static zval* php_perl_sv_to_zval_ref(PerlInterpreter* my_perl, SV* sv, zval* zv, HashTable* var_hash TSRMLS_DC);
static zval* php_perl_sv_to_zval(PerlInterpreter* my_perl, SV* sv, zval* zv TSRMLS_DC);

/* Handlers for Perl objects overloading */
static zend_object_value php_perl_clone(zval *object TSRMLS_DC);
static zval* php_perl_read_property(zval *object, zval *member, int type TSRMLS_DC);
static void php_perl_write_property(zval *object, zval *member, zval *value TSRMLS_DC);
static zval* php_perl_read_dimension(zval *object, zval *offset, int type TSRMLS_DC);
static void php_perl_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC);
static int php_perl_has_property(zval *object, zval *member, int check_empty TSRMLS_DC);
static void php_perl_unset_property(zval *object, zval *member TSRMLS_DC);
static int php_perl_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC);
static void php_perl_unset_dimension(zval *object, zval *offset TSRMLS_DC);
static HashTable* php_perl_get_properties(zval *object TSRMLS_DC);
static zend_function *php_perl_get_method(zval *object, char *method, int method_len TSRMLS_DC);
static int php_perl_call_function_handler(char *method, INTERNAL_FUNCTION_PARAMETERS);
static zend_function *php_perl_get_constructor(zval *object TSRMLS_DC);
static zend_class_entry* php_perl_get_class_entry(zval *object TSRMLS_DC);
static int php_perl_get_class_name(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC);
static zval* php_perl_get(zval *object TSRMLS_DC);
static void php_perl_set(zval **object, zval *value TSRMLS_DC);

static void php_perl_constructor_handler(INTERNAL_FUNCTION_PARAMETERS);

static void php_perl_destructor(void *perl_object, zend_object_handle handle TSRMLS_DC);
static void php_perl_cleaner(void *perl_object, zend_object_handle handle TSRMLS_DC);

static zend_object_handlers php_perl_object_handlers = {
  zend_objects_store_add_ref,      /* add_ref */
  zend_objects_store_del_ref,      /* del_ref */
  php_perl_clone,                  /* clone_obj */

  php_perl_read_property,          /* read_property */
  php_perl_write_property,         /* write_property */
  php_perl_read_dimension,         /* read_dimension */
  php_perl_write_dimension,        /* write_dimension */
  NULL,                            /* get_property_ptr_ptr */
  NULL,                            /* get */
  NULL,                            /* set */
  php_perl_has_property,           /* has_property */
  php_perl_unset_property,         /* unset_property */
  php_perl_has_dimension,          /* has_dimension */
  php_perl_unset_dimension,        /* unset_dimension */
  php_perl_get_properties,         /* get_properties */
  php_perl_get_method,             /* get_method */
  php_perl_call_function_handler,  /* call_method */
  php_perl_get_constructor,        /* get_constructor */
  php_perl_get_class_entry,        /* get_class_entry */
  php_perl_get_class_name,         /* get_class_name */
  NULL,                            /* compare_objects */
  NULL,                            /* cast_object */
};

static zend_object_handlers php_perl_proxy_handlers = {
  zend_objects_store_add_ref,      /* add_ref */
  zend_objects_store_del_ref,      /* del_ref */
  php_perl_clone,                  /* clone_obj */

  php_perl_read_property,          /* read_property */
  php_perl_write_property,         /* write_property */
  php_perl_read_dimension,         /* read_dimension */
  php_perl_write_dimension,        /* write_dimension */
  NULL,                            /* get_property_ptr_ptr */
  php_perl_get,                    /* get */
  php_perl_set,                    /* set */
  php_perl_has_property,           /* has_property */
  php_perl_unset_property,         /* unset_property */
  php_perl_has_dimension,          /* has_dimension */
  php_perl_unset_dimension,        /* unset_dimension */
  php_perl_get_properties,         /* get_properties */
  php_perl_get_method,             /* get_method */
  php_perl_call_function_handler,  /* call_method */
  php_perl_get_constructor,        /* get_constructor */
  php_perl_get_class_entry,        /* get_class_entry */
  php_perl_get_class_name,         /* get_class_name */
  NULL,                            /* compare_objects */
  NULL,                            /* cast_object */
};

zend_internal_function php_perl_constructor_function = {
  ZEND_INTERNAL_FUNCTION,
  "Perl",
  NULL,
  0,
  NULL,
  2,
  0,
  NULL,
  0,
  0,
  php_perl_constructor_handler
};

static SV* PerlIOPHP_getarg(pTHX_ PerlIO *f, CLONE_PARAMS *param, int flags)
{
  Perl_croak(aTHX_ "an attempt to getarg from a stale io handle");
  return NULL;
}

static SSize_t PerlIOPHP_write(pTHX_ PerlIO *f, const void *vbuf, Size_t count)
{
  return zend_write(vbuf, count);
}

static IV PerlIOPHP_flush(pTHX_ PerlIO *f)
{
  TSRMLS_FETCH();
  sapi_flush(TSRMLS_C);
  return 0;
}

static IV PerlIOPHP_noop_fail(pTHX_ PerlIO *f)
{
    return -1;
}

static PerlIO_funcs PerlIO_PHP = {
    sizeof(PerlIO_funcs),
    "PHP",
    sizeof(struct _PerlIO),
    PERLIO_K_MULTIARG | PERLIO_K_RAW,
    PerlIOBase_pushed,
    PerlIOBase_popped,
    NULL,
    PerlIOBase_binmode,
    PerlIOPHP_getarg,
    PerlIOBase_fileno,
    PerlIOBase_dup,
    PerlIOBase_read,
    NULL,
    PerlIOPHP_write,
    NULL,                       /* can't seek on STD{IN|OUT}, fail on call*/
    NULL,                       /* can't tell on STD{IN|OUT}, fail on call*/
    PerlIOBase_close,
    PerlIOPHP_flush,
    PerlIOPHP_noop_fail,        /* fill */
    PerlIOBase_eof,
    PerlIOBase_error,
    PerlIOBase_clearerr,
    PerlIOBase_setlinebuf,
    NULL,                       /* get_base */
    NULL,                       /* get_bufsiz */
    NULL,                       /* get_ptr */
    NULL,                       /* get_cnt */
    NULL,                       /* set_ptrcnt */
};

/* Creates Perl interpriter if it was not created brfore */
static PerlInterpreter* php_perl_init(TSRMLS_D)
{
  PerlInterpreter* my_perl = PERLG(perl);

  if (my_perl == NULL) {
    char *embedding[] = { "", "-e", "0" };

    my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    perl_parse(my_perl, xs_init, 3, embedding, (char **)NULL);
/*???FIXME: I don't understend if it need or not
    perl_run(my_perl);
*/
    PerlIO_define_layer(aTHX_ &PerlIO_PHP);
    PerlIO_apply_layers(aTHX_ PerlIO_stdout(), "w", ":PHP");

    PERLG(perl) = my_perl;
    zend_hash_init(&PERLG(perl_objects), 0, NULL, NULL, 0);
  }
  return my_perl;
}

/* Destroys Perl interpriter if it was created brfore */
static void php_perl_destroy(TSRMLS_D)
{
  PerlInterpreter* my_perl = PERLG(perl);

  if (my_perl != NULL) {
    zend_hash_destroy(&PERLG(perl_objects));
    perl_destruct(my_perl);
    perl_free(my_perl);
    PERLG(perl) = NULL;
  }
}

/* Remembers mapping betwenn Perl and PHP object */
static void php_perl_remember_object(SV* sv, zend_object_handle handle TSRMLS_DC) {
  zend_hash_add(&PERLG(perl_objects), (char*)SvRV(sv), sizeof(sv),
                &handle, sizeof(zend_object_handle), NULL);
}

/* Forgets mapping betwenn Perl and PHP object */
static inline void php_perl_forget_object(SV* sv TSRMLS_DC) {
  if (sv) {
    zend_hash_del(&PERLG(perl_objects), (char*)SvRV(sv), sizeof(sv));
  }
}

/* Converts PHP's value to Perl's eqivalent */
static SV* php_perl_zval_to_sv(PerlInterpreter *my_perl,
                               zval            *zv TSRMLS_DC)
{
  HashTable var_hash;
  SV* sv;

  zend_hash_init(&var_hash, 0, NULL, NULL, 0);
  sv = php_perl_zval_to_sv_ref(my_perl, zv, &var_hash TSRMLS_CC);
  zend_hash_destroy(&var_hash);
  return sv;
}

static SV* php_perl_zval_to_sv_ref(PerlInterpreter *my_perl,
                                   zval            *zv,
                                   HashTable       *var_hash TSRMLS_DC)
{
  SV* sv;

  if ((zv->is_ref || Z_TYPE_P(zv) == IS_OBJECT || Z_TYPE_P(zv) == IS_ARRAY) &&
      zend_hash_find(var_hash, (char*)zv, sizeof(zv), (void**)&sv) == SUCCESS) {
    sv = *(SV**)sv;
    SvREFCNT_inc(sv);
    return sv;
  }
  sv = php_perl_zval_to_sv_noref(my_perl, zv, var_hash TSRMLS_CC);
  if (zv->is_ref || Z_TYPE_P(zv) == IS_OBJECT || Z_TYPE_P(zv) == IS_ARRAY) {
    zend_hash_add(var_hash, (char*)zv, sizeof(zv), &sv, sizeof(SV*), NULL);
  }
  return sv;
}

static SV* php_perl_zval_to_sv_noref(PerlInterpreter *my_perl,
                                     zval            *zv,
                                     HashTable       *var_hash TSRMLS_DC)
{
  switch (Z_TYPE_P(zv)) {
    case IS_NULL:
      return &PL_sv_undef;
    case IS_LONG:
      return newSViv(Z_LVAL_P(zv));
    case IS_DOUBLE:
      return newSVnv(Z_DVAL_P(zv));
    case IS_STRING:
      return newSVpv(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
    case IS_ARRAY: {
      HashTable* ht = zv->value.ht;
      int is_hash  = 0;

      /* checking if 'hv' is array or hash */
      zend_hash_internal_pointer_reset(ht);
      while (1) {
        int key_type = zend_hash_get_current_key_type(ht);
        if (key_type == HASH_KEY_NON_EXISTANT) {
          break;
        }
        if (key_type == HASH_KEY_IS_STRING) {
          is_hash = 1;
          break;
        }
        zend_hash_move_forward(ht);
      }

      if (is_hash) {
        /* converting to Perl's hash */
        HV* hv = newHV();
        SV* sv = (SV*)newRV((SV*)hv);
        zval** zv_ptr;

        if (zv->is_ref || Z_TYPE_P(zv) == IS_ARRAY) {
          zend_hash_add(var_hash, (char*)zv, sizeof(zv), &sv, sizeof(SV*), NULL);
        }

        for (zend_hash_internal_pointer_reset(ht);
             zend_hash_get_current_data(ht, (void **) &zv_ptr) == SUCCESS;
             zend_hash_move_forward(ht)
            ) {
          char* key;
          int key_len;
          ulong index;

          if (zend_hash_get_current_key_ex(ht, &key, &key_len, &index, 0, NULL) != HASH_KEY_IS_STRING) {
            char xkey[16];
            zend_sprintf(xkey, "%ld", index);
            hv_store(hv, xkey, strlen(xkey),
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, var_hash TSRMLS_CC), 0);
          } else {
            hv_store(hv, key, key_len-1,
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, var_hash TSRMLS_CC), 0);
          }
        }

        return sv;
      } else {
        /* converting to Perl's array */
        AV* av = newAV();
        SV* sv = (SV*)newRV((SV*)av);
        zval** zv_ptr;

        if (zv->is_ref || Z_TYPE_P(zv) == IS_ARRAY) {
          zend_hash_add(var_hash, (char*)zv, sizeof(zv), &sv, sizeof(SV*), NULL);
        }

        for (zend_hash_internal_pointer_reset(ht);
             zend_hash_get_current_data(ht, (void **) &zv_ptr) == SUCCESS;
             zend_hash_move_forward(ht)
            ) {
          char *key;
          ulong index;

          if (zend_hash_get_current_key(ht, &key, &index, 0) != HASH_KEY_IS_STRING) {
            av_store(av, index,
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, var_hash TSRMLS_CC));
          }
        }
        return sv;
      }
    }
    case IS_OBJECT:
      if (zv->value.obj.handlers == &php_perl_object_handlers ||
          zv->value.obj.handlers == &php_perl_proxy_handlers) {
        php_perl_object *obj = zend_object_store_get_object(zv TSRMLS_CC);
        return newSVsv(obj->sv);
      }
      break;
    case IS_BOOL:
      return Z_LVAL_P(zv)?&PL_sv_yes:&PL_sv_no;
    default:
      break;
  }
  zend_error(E_ERROR, "[perl] Can't convert PHP type (%d) to Perl",
             Z_TYPE_P(zv));
  return &PL_sv_undef;
}

/* Converts Perl's value to PHP's eqivalent */
static zval* php_perl_sv_to_zval(PerlInterpreter* my_perl, SV* sv, zval* zv TSRMLS_DC)
{
  HashTable var_hash;

  zend_hash_init(&var_hash, 0, NULL, NULL, 0);
  zv = php_perl_sv_to_zval_ref(my_perl, sv, zv, &var_hash TSRMLS_CC);
  zend_hash_destroy(&var_hash);
  return zv;
}

static zval* php_perl_sv_to_zval_ref(PerlInterpreter *my_perl,
                                     SV              *sv,
                                     zval            *zv,
                                     HashTable       *var_hash TSRMLS_DC)
{
  zval** z;

  if (SvREFCNT(sv) > 1 &&
      zend_hash_find(var_hash, (char*)sv, sizeof(sv), (void**)&z) == SUCCESS) {
    if (zv != NULL) {
      FREE_ZVAL(zv);
    }
    if (Z_TYPE_PP(z) != IS_OBJECT) {
      (*z)->is_ref = 1;
    }
    (*z)->refcount++;
    return *z;
  }

  if (zv == NULL) {ALLOC_INIT_ZVAL(zv);}

  if (SvREFCNT(sv) > 1) {
    zend_hash_add(var_hash, (char*)sv, sizeof(sv), &zv, sizeof(zval*), NULL);
  }
  return php_perl_sv_to_zval_noref(my_perl, sv, zv, var_hash TSRMLS_CC);
}

static zval* php_perl_sv_to_zval_noref(PerlInterpreter *my_perl,
                                       SV              *sv,
                                       zval            *zv,
                                       HashTable       *var_hash TSRMLS_DC)
{
  if (sv) {
    if (SvTYPE(sv) == SVt_NULL) {    /* null */
      ZVAL_NULL(zv);
    } else if (SvIOK(sv)) {          /* integer */
      ZVAL_LONG(zv, SvIV(sv));
    } else if (SvNOK(sv)) {          /* double */
      ZVAL_DOUBLE(zv, SvNV(sv));
    } else if (SvPOK(sv)) {          /* string */
      int  len;
      char *str = SvPV(sv, len);
      ZVAL_STRINGL(zv, str, len, 1);
    } else if (sv_isobject(sv)) {    /* object */
      zend_object_handle* handle;
      if (zend_hash_find(&PERLG(perl_objects), (char*)SvRV(sv), sizeof(SV*), (void**)&handle) == SUCCESS) {
        zv->type = IS_OBJECT;
        zv->value.obj.handlers = &php_perl_object_handlers;;
        zv->value.obj.handle   = *handle;
        zend_objects_store_add_ref(zv TSRMLS_CC);
      } else {
        php_perl_object *obj = (php_perl_object*)emalloc(sizeof(php_perl_object));
        obj->sv = sv;
        obj->properties = NULL;
        SvREFCNT_inc(sv);
        zv->type = IS_OBJECT;
        zv->value.obj.handlers = &php_perl_object_handlers;
        zv->value.obj.handle =
          zend_objects_store_put(obj, php_perl_destructor, NULL, NULL TSRMLS_CC);
        php_perl_remember_object(sv, zv->value.obj.handle TSRMLS_CC);
      }
    } else if (SvROK(sv)) {                        /* reference */
      zv = php_perl_sv_to_zval_ref(my_perl, SvRV(sv), zv, var_hash TSRMLS_CC);
    } else if (SvTYPE(sv) == SVt_PVAV) {           /* array */
      I32 i   = 0;
      I32 len = av_len((AV*)sv);

      array_init(zv);
      for (i = 0; i <= len; i++) {
        SV** el_sv = av_fetch((AV*)sv, i, 0);
        if (el_sv != NULL && *el_sv != NULL) {
          add_index_zval(zv, i,
            php_perl_sv_to_zval_ref(my_perl, *el_sv, NULL, var_hash TSRMLS_CC));
        }
      }
    } else if (SvTYPE(sv) == SVt_PVHV) {           /* hash */
      SV*   el_sv;
      char* key;
      I32   key_len;

      array_init(zv);
      hv_iterinit((HV*)sv);
      while ((el_sv = hv_iternextsv((HV*)sv, &key, &key_len)) != NULL) {
        add_assoc_zval_ex(zv, key, key_len+1,
          php_perl_sv_to_zval_ref(my_perl, el_sv, NULL, var_hash TSRMLS_CC));
      }
    } else {
      zend_error(E_ERROR, "[perl] Can't convert Perl type (%ld) to PHP",
                 SvTYPE(sv));
    }
  }
  return zv;
}

/* Calls constructor of Perl's class and returns created object */
static SV* php_perl_call_constructor(PerlInterpreter* my_perl,
                                     const char* class_name,
                                     int class_name_len,
                                     const char* constructor,
                                     int constructor_len,
                                     int argc,
                                     zval** argv[] TSRMLS_DC)
{
  SV*   ret;
  int   func_len = class_name_len + constructor_len + 3;
  char *func = (char*)emalloc(func_len);

  dSP;                            /* initialize stack pointer         */

  int   i;

  strcpy(func, class_name);
  strcpy(func + class_name_len, "::");
  strcpy(func + class_name_len + 2, constructor);


  ENTER;                          /* everything created after here    */
  SAVETMPS;                       /* ...is a temporary variable.      */
  PUSHMARK(SP);                   /* remember the stack pointer       */

  XPUSHs(sv_2mortal(newSVpv(class_name, class_name_len)));
  for (i=0; i<argc; i++) {
    XPUSHs(sv_2mortal(php_perl_zval_to_sv(my_perl, *argv[i] TSRMLS_CC)));
  }

  PUTBACK;                        /* make local stack pointer global  */
  call_pv(func, G_SCALAR | G_EVAL | G_KEEPERR);        /* call the function                */
  SPAGAIN;                        /* refresh stack pointer            */

  ret = POPs;
  SvREFCNT_inc(ret);

  PUTBACK;
  FREETMPS;                       /* free that return value           */
  LEAVE;                          /* ...and the XPUSHed "mortal" args.*/

  efree(func);
  return ret;
}

/* Calls method of Perl's object */
static void php_perl_call_method(PerlInterpreter* my_perl, SV* obj,
                                 const char* func, int argc, zval** argv[],
                                 perl_context context,
                                 zval* return_value TSRMLS_DC)
{
  dSP;                            /* initialize stack pointer         */

  int i;

  ENTER;                          /* everything created after here    */
  SAVETMPS;                       /* ...is a temporary variable.      */
  PUSHMARK(SP);                   /* remember the stack pointer       */

  XPUSHs(obj);

  for (i=0; i<argc; i++) {
    XPUSHs(sv_2mortal(php_perl_zval_to_sv(my_perl, *argv[i] TSRMLS_CC)));
  }

  PUTBACK;                        /* make local stack pointer global  */
  if (return_value != NULL) {
    if (context != PERL_SCALAR) {
      int count, i;
      I32 ax;
      HashTable var_hash;

      count = call_method(func, G_ARRAY | G_EVAL | G_KEEPERR);        /* call the function                */
      SPAGAIN;                        /* refresh stack pointer            */
      sp -= count;
      ax = (sp - PL_stack_base) + 1;
      zend_hash_init(&var_hash, 0, NULL, NULL, 0);
      array_init(return_value);
      if (context == PERL_ARRAY) {
        for (i = 0; i < count; i++) {
          add_next_index_zval(return_value,
            php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(i), NULL, &var_hash TSRMLS_CC));
        }
      } else {
        for (i = 0; i < count; i++) {
          int  key_len;
          char *key = SvPV(ST(i), key_len);
          if (i != count-1) {
            add_assoc_zval_ex(return_value, key, key_len+1,
              php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(++i), NULL, &var_hash TSRMLS_CC));
          } else {
            add_assoc_null_ex(return_value, key, key_len+1);
          }
        }
      }
      zend_hash_destroy(&var_hash);
    } else {
      call_method(func, G_SCALAR | G_EVAL | G_KEEPERR);        /* call the function                */
      SPAGAIN;                        /* refresh stack pointer            */

      php_perl_sv_to_zval(my_perl, POPs, return_value TSRMLS_CC);
    }
  } else {
    call_method(func, G_DISCARD | G_EVAL | G_KEEPERR);        /* call the function                */
    SPAGAIN;                        /* refresh stack pointer            */
  }
  PUTBACK;
  FREETMPS;                       /* free that return value           */
  LEAVE;                          /* ...and the XPUSHed "mortal" args.*/
}

/* Calls Perl's function */
static void php_perl_call(PerlInterpreter* my_perl,
                          const char* func, int argc, zval** argv[],
                          perl_context context,
                          zval* return_value TSRMLS_DC)
{
  dSP;                            /* initialize stack pointer         */

  int i;

  ENTER;                          /* everything created after here    */
  SAVETMPS;                       /* ...is a temporary variable.      */
  PUSHMARK(SP);                   /* remember the stack pointer       */

  for (i=0; i<argc; i++) {
    XPUSHs(sv_2mortal(php_perl_zval_to_sv(my_perl, *argv[i] TSRMLS_CC)));
  }
  PUTBACK;                        /* make local stack pointer global  */
  if (return_value != NULL) {
    if (context != PERL_SCALAR) {
      int count, i;
      I32 ax;
      HashTable var_hash;

      count = call_pv(func, G_ARRAY | G_EVAL | G_KEEPERR);        /* call the function                */
      SPAGAIN;                        /* refresh stack pointer            */
      sp -= count;
      ax = (sp - PL_stack_base) + 1;
      zend_hash_init(&var_hash, 0, NULL, NULL, 0);
      array_init(return_value);
      if (context == PERL_ARRAY) {
        for (i = 0; i < count; i++) {
          add_next_index_zval(return_value,
            php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(i), NULL, &var_hash TSRMLS_CC));
        }
      } else {
        for (i = 0; i < count; i++) {
          int  key_len;
          char *key = SvPV(ST(i), key_len);
          if (i != count-1) {
            add_assoc_zval_ex(return_value, key, key_len+1,
              php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(++i), NULL, &var_hash TSRMLS_CC));
          } else {
            add_assoc_null_ex(return_value, key, key_len+1);
          }
        }
      }
      zend_hash_destroy(&var_hash);
    } else {
      call_pv(func, G_SCALAR | G_EVAL | G_KEEPERR);        /* call the function                */
      SPAGAIN;                        /* refresh stack pointer            */

      php_perl_sv_to_zval(my_perl, POPs, return_value TSRMLS_CC);
    }
  } else {
    call_pv(func, G_DISCARD | G_EVAL | G_KEEPERR);        /* call the function                */
    SPAGAIN;                        /* refresh stack pointer            */
  }
  PUTBACK;
  FREETMPS;                       /* free that return value           */
  LEAVE;                          /* ...and the XPUSHed "mortal" args.*/
}

/****************************************************************************/

static zval* php_perl_get(zval *object TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  zval *retval;

  if (obj->sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not get value");
    return NULL;
  }
  retval = php_perl_sv_to_zval(my_perl, obj->sv, NULL TSRMLS_CC);
  /* ensure we're creating a temporary variable */
  if (retval) {retval->refcount = 0;}
  return retval;
}

static void php_perl_set(zval **object, zval *value TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(*object TSRMLS_CC);
  SV *sv = obj->sv;

  if (sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not set value");
    return;
  } 
  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }
  sv_setsv(sv, php_perl_zval_to_sv(my_perl, value TSRMLS_CC));
}

/* Returns element of array based Perl's object */
static zval* php_perl_read_dimension(zval *object, zval *offset, int type TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  zval *retval = NULL;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;

  if (sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not get dimension");
    return NULL;
  }
  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }

  if (SvTYPE(sv) == SVt_PVAV) {
    AV* av = (AV*)sv;
    SV** prop_val;

    convert_to_long(offset);

    prop_val = av_fetch(av, Z_LVAL_P(offset), 0);
    if (prop_val != NULL) {
      zend_bool write = FALSE;
      if (type != BP_VAR_R && type != BP_VAR_IS) {
        if (*prop_val != NULL && type != BP_VAR_R && type != BP_VAR_IS) {
          write = TRUE;
/*
          SV* tmp_sv = *prop_val;

          while (1) {
            if (sv_isobject(tmp_sv) || 
                SvTYPE(tmp_sv) == SVt_PVAV || 
                SvTYPE(tmp_sv) == SVt_PVHV) {
              write = TRUE;
              break;
            } else if (SvTYPE(tmp_sv) != SVt_RV) {
              break;
            }
            tmp_sv = SvRV(tmp_sv);
          }
*/
        }
      }
      if (write && !sv_isobject(*prop_val)) {
        zend_object_value new_value;
        php_perl_object *obj = (php_perl_object*)emalloc(sizeof(php_perl_object));

        obj->zo.ce = perl_class_entry;
        obj->sv = newRV(*prop_val);
        obj->properties = NULL;
        obj->context = PERL_SCALAR;

        ALLOC_INIT_ZVAL(retval);
        retval->refcount = 0;
        retval->is_ref = 1;
        retval->type = IS_OBJECT;
        Z_OBJ_HT_P(retval) = &php_perl_proxy_handlers;
        Z_OBJ_HANDLE_P(retval) = zend_objects_store_put(obj, php_perl_destructor, NULL, NULL TSRMLS_CC);        
      } else {
        ALLOC_INIT_ZVAL(retval);
        retval = php_perl_sv_to_zval(my_perl, *prop_val, retval TSRMLS_CC);
        /* ensure we're creating a temporary variable */
        if (retval) {retval->refcount = 0;}
      }
    }
  } else if (SvTYPE(sv) == SVt_PVHV) {
    retval = php_perl_read_property(object, offset, type TSRMLS_CC);
  } else {
    zend_error(E_WARNING, "[perl] Object is not an array");
  }
  if (retval == NULL) {
    return EG(uninitialized_zval_ptr);
  } else {
    return retval;
  }
}

/* Sets element of array based Perl's object */
static void php_perl_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;

  if (sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not set dimension");
  }
  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }

  if (SvTYPE(sv) == SVt_PVAV) {
    AV* av = (AV*)sv;

    convert_to_long(offset);

    av_store(av, Z_LVAL_P(offset), php_perl_zval_to_sv(my_perl, value TSRMLS_CC));
  } else if (SvTYPE(sv) == SVt_PVHV) {
    php_perl_write_property(object, offset, value TSRMLS_CC);
  } else {
    zend_error(E_WARNING, "[perl] Object is not an array");
  }
}

/* Checks if element of array based Perl's object isset or empty */
static int php_perl_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC)
{
  int ret = 0;
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;

  if (sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not check dimension");
    return 0;
  }
  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }
  if (SvTYPE(sv) == SVt_PVAV) {
    AV* av = (AV*)sv;

    convert_to_long(offset);

    if (check_empty) {
      /* empty() */
      SV** prop_val = av_fetch(av, Z_LVAL_P(offset), 0);
      if (prop_val != NULL) {
        zval *zv;
        ALLOC_INIT_ZVAL(zv);
        php_perl_sv_to_zval(my_perl, *prop_val, zv TSRMLS_CC);
        ret = zend_is_true(zv);
        zval_ptr_dtor(&zv);
      }
    } else {
      /* isset() */
      if (av_exists(av, Z_LVAL_P(offset))) {
        ret = 1;
      }
    }
  } else if (SvTYPE(sv) == SVt_PVHV) {
    ret = php_perl_has_property(object, offset, check_empty TSRMLS_CC);
  } else {
    zend_error(E_WARNING, "[perl] Object is not an array");
  }
  return ret;
}

/* Deletes element of array based Perl's object */
static void php_perl_unset_dimension(zval *object, zval *offset TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;

  if (sv == NULL) {
    zend_error(E_ERROR, "[perl] Can not unset dimension");
  }
  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }
  if (SvTYPE(sv) == SVt_PVAV) {
    AV* av = (AV*)sv;

    convert_to_long(offset);

    av_delete(av, Z_LVAL_P(offset), G_DISCARD);
  } else if (SvTYPE(sv) == SVt_PVHV) {
    php_perl_unset_property(object, offset TSRMLS_CC);
  } else {
    zend_error(E_WARNING, "[perl] Object is not an array");
  }
}

/* Returns propery of hash based Perl's object */
static zval* php_perl_read_property(zval *object, zval *member, int type TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  zval *retval = NULL;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  zval tmp_member;
  SV* sv = NULL;
  zend_bool write = obj->context != PERL_SCALAR && 
                    type != BP_VAR_R && type != BP_VAR_IS;

  if (member->type != IS_STRING) {
    tmp_member = *member;
    zval_copy_ctor(&tmp_member);
    convert_to_string(&tmp_member);
    member = &tmp_member;
  }
  if (obj->context == PERL_SCALAR) {
    if (zend_binary_strcmp(Z_STRVAL_P(member), Z_STRLEN_P(member), "array", sizeof("array")-1) == 0) {
      php_perl_object *new_obj;
      zval* new_object;

      new_obj = emalloc(sizeof(php_perl_object));
      memcpy(new_obj, obj, sizeof(php_perl_object));
      new_obj->context = PERL_ARRAY;

      ALLOC_INIT_ZVAL(new_object);
      new_object->refcount = 0;
      new_object->type = IS_OBJECT;
      new_object->value.obj.handlers = &php_perl_object_handlers;
      new_object->value.obj.handle =
        zend_objects_store_put(new_obj, php_perl_cleaner, NULL, NULL TSRMLS_CC);

      return new_object;
    } else if (zend_binary_strcmp(Z_STRVAL_P(member),Z_STRLEN_P(member),"hash",sizeof("hash")-1) == 0) {
      php_perl_object *new_obj;
      zval* new_object;

      new_obj = emalloc(sizeof(php_perl_object));
      memcpy(new_obj, obj, sizeof(php_perl_object));
      new_obj->context = PERL_HASH;

      ALLOC_INIT_ZVAL(new_object);
      new_object->refcount = 0;
      new_object->type = IS_OBJECT;
      new_object->value.obj.handlers = &php_perl_object_handlers;
      new_object->value.obj.handle =
        zend_objects_store_put(new_obj, php_perl_cleaner, NULL, NULL TSRMLS_CC);

      return new_object;
    } else if (zend_binary_strcmp(Z_STRVAL_P(member),Z_STRLEN_P(member),"scalar",sizeof("scalar")-1) == 0) {
      php_perl_object *new_obj;
      zval* new_object;

      new_obj = emalloc(sizeof(php_perl_object));
      memcpy(new_obj, obj, sizeof(php_perl_object));
      new_obj->context = PERL_SCALAR;

      ALLOC_INIT_ZVAL(new_object);
      new_object->refcount = 0;
      new_object->type = IS_OBJECT;
      new_object->value.obj.handlers = &php_perl_object_handlers;
      new_object->value.obj.handle =
        zend_objects_store_put(new_obj, php_perl_cleaner, NULL, NULL TSRMLS_CC);

      return new_object;
    }
  }

  if (obj->sv == NULL) {
    if (obj->context == PERL_ARRAY) {
      sv = (SV*)get_av(Z_STRVAL_P(member), write);
      if (sv && !AvARRAY(sv)) {if (write) {av_clear((AV*)sv);} else {sv = NULL;}}
    } else if (obj->context == PERL_HASH) {
      sv = (SV*)get_hv(Z_STRVAL_P(member), write);
      if (sv && !HvARRAY(sv)) {if (write) {hv_clear((HV*)sv);} else {sv = NULL;}}
    } else {
      sv = get_sv(Z_STRVAL_P(member), FALSE);
      if (sv && !SvOK(sv)) {sv = NULL;}
    }
    if (sv == NULL) {
      if (obj->context == PERL_ARRAY) {
        zend_error(E_NOTICE, "[perl] Undefined variable: '@%s'", Z_STRVAL_P(member));
      } else if (obj->context == PERL_HASH) {
        zend_error(E_NOTICE, "[perl] Undefined variable: '%%%s'", Z_STRVAL_P(member));
      } else {
        zend_error(E_NOTICE, "[perl] Undefined variable: '$%s'", Z_STRVAL_P(member));
      }
    }
  } else {
    sv = obj->sv;
    while (SvTYPE(sv) == SVt_RV) {
      sv = SvRV(sv);
    }

    if (SvTYPE(sv) == SVt_PVHV) {
      HV* hv = (HV*)sv;
      SV** prop_val;

      prop_val = hv_fetch(hv, Z_STRVAL_P(member), Z_STRLEN_P(member), 0);
      if (prop_val != NULL) {
        sv = *prop_val;
        write = FALSE;
        if (sv != NULL && type != BP_VAR_R && type != BP_VAR_IS) {
          write = TRUE;
/*
          SV* tmp_sv = sv;

          while (1) {
            if (sv_isobject(tmp_sv) || 
                SvTYPE(tmp_sv) == SVt_PVAV || 
                SvTYPE(tmp_sv) == SVt_PVHV) {
              write = TRUE;
              break;
            } else if (SvTYPE(tmp_sv) != SVt_RV) {
              break;
            }
            tmp_sv = SvRV(tmp_sv);
          }
*/
        }
      }
    } else {
      zend_error(E_WARNING, "[perl] Object is not a hash");
    }
  }
  
  if (sv != NULL) {
    if (write && !sv_isobject(sv)) {
      zend_object_value new_value;
      php_perl_object *obj = (php_perl_object*)emalloc(sizeof(php_perl_object));

      obj->zo.ce = perl_class_entry;
      obj->sv = newRV(sv);
      obj->properties = NULL;
      obj->context = PERL_SCALAR;

      ALLOC_INIT_ZVAL(retval);
      retval->refcount = 0;
      retval->is_ref = 1;
      retval->type = IS_OBJECT;
      Z_OBJ_HT_P(retval) = &php_perl_proxy_handlers;
      Z_OBJ_HANDLE_P(retval) = zend_objects_store_put(obj, php_perl_destructor, NULL, NULL TSRMLS_CC);        
    } else {
      retval = php_perl_sv_to_zval(my_perl, sv, retval TSRMLS_CC);
      /* ensure we're creating a temporary variable */
      if (retval) {retval->refcount = 0;}
    }
  }

  if (member == &tmp_member) {
    zval_dtor(member);
  }
  if (retval == NULL) {
    return EG(uninitialized_zval_ptr);
  } else {
    return retval;
  }
}

/* Sets propery of hash based Perl's object */
static void php_perl_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;
  zval tmp_member;

  if (member->type != IS_STRING) {
    tmp_member = *member;
    zval_copy_ctor(&tmp_member);
    convert_to_string(&tmp_member);
    member = &tmp_member;
  }
  if (sv == NULL) {
    if (obj->context == PERL_ARRAY) {
      AV *av = get_av(Z_STRVAL_P(member), TRUE);
      if (Z_TYPE_P(value) == IS_ARRAY) {
        HashTable *ht = Z_ARRVAL_P(value);
        zval** zv_ptr;
        HashTable var_hash;

        zend_hash_init(&var_hash, 0, NULL, NULL, 0);
        for (zend_hash_internal_pointer_reset(ht);
             zend_hash_get_current_data(ht, (void **) &zv_ptr) == SUCCESS;
             zend_hash_move_forward(ht)
            ) {
          char *key;
          ulong index;

          if (zend_hash_get_current_key(ht, &key, &index, 0) != HASH_KEY_IS_STRING) {
            av_store(av, index,
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, &var_hash TSRMLS_CC));
          }
        }
        zend_hash_destroy(&var_hash);
      } else {
        zend_error(E_NOTICE, "[perl] array required");
      }
    } else if (obj->context == PERL_HASH) {
      HV *hv = get_hv(Z_STRVAL_P(member), TRUE);
      if (Z_TYPE_P(value) == IS_ARRAY) {
        HashTable *ht = Z_ARRVAL_P(value);
        zval** zv_ptr;
        HashTable var_hash;

        zend_hash_init(&var_hash, 0, NULL, NULL, 0);
        for (zend_hash_internal_pointer_reset(ht);
             zend_hash_get_current_data(ht, (void **) &zv_ptr) == SUCCESS;
             zend_hash_move_forward(ht)
            ) {
          char* key;
          int key_len;
          ulong index;

          if (zend_hash_get_current_key_ex(ht, &key, &key_len, &index, 0, NULL) != HASH_KEY_IS_STRING) {
            char xkey[16];
            zend_sprintf(xkey, "%ld", index);
            hv_store(hv, xkey, strlen(xkey),
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, &var_hash TSRMLS_CC), 0);
          } else {
            hv_store(hv, key, key_len-1,
              php_perl_zval_to_sv_ref(my_perl, *zv_ptr, &var_hash TSRMLS_CC), 0);
          }
        }
        zend_hash_destroy(&var_hash);
      } else {
        zend_error(E_NOTICE, "[perl] array required");
      }
    } else {
      SV *sv = get_sv(Z_STRVAL_P(member), TRUE);
      sv_setsv(sv, php_perl_zval_to_sv(my_perl, value TSRMLS_CC));
    }
  } else {
    while (SvTYPE(sv) == SVt_RV) {
      sv = SvRV(sv);
    }

    if (SvTYPE(sv) == SVt_PVHV) {
      HV* hv = (HV*)sv;

      hv_store(hv, Z_STRVAL_P(member), Z_STRLEN_P(member),
               php_perl_zval_to_sv(my_perl, value TSRMLS_CC), 0);
    } else {
      zend_error(E_WARNING, "[perl] Object is not a hash");
    }
  }
  if (member == &tmp_member) {
    zval_dtor(member);
  }
}

/* Checks if propery of hash based Perl's object isset or empty */
static int php_perl_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
  int ret = 0;
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;
  zval tmp_member;

  if (member->type != IS_STRING) {
    tmp_member = *member;
    zval_copy_ctor(&tmp_member);
    convert_to_string(&tmp_member);
    member = &tmp_member;
  }

  if (sv == NULL) {
    SV* sv;
    if (obj->context == PERL_ARRAY) {
      sv = (SV*)get_av(Z_STRVAL_P(member), FALSE);
      if (sv && !AvARRAY(sv)) {sv = NULL;}
    } else if (obj->context == PERL_HASH) {
      sv = (SV*)get_hv(Z_STRVAL_P(member), FALSE);
      if (sv && !HvARRAY(sv)) {sv = NULL;}
    } else {
      sv = get_sv(Z_STRVAL_P(member), FALSE);
      if (sv && !SvOK(sv)) {sv = NULL;}
    }
    if (sv) {
      if (check_empty) {
        zval *zv;
        ALLOC_INIT_ZVAL(zv);
        php_perl_sv_to_zval(my_perl, sv, zv TSRMLS_CC);
        ret = zend_is_true(zv);
        zval_ptr_dtor(&zv);
      } else {
        ret = 1;
      }
    }
  } else {
    while (SvTYPE(sv) == SVt_RV) {
      sv = SvRV(sv);
    }
    if (SvTYPE(sv) == SVt_PVHV) {
      HV*   hv = (HV*)sv;

      if (check_empty) {
        /* empty() */
        SV** prop_val = hv_fetch(hv, Z_STRVAL_P(member), Z_STRLEN_P(member), 0);
        if (prop_val != NULL) {
          zval *zv;
          ALLOC_INIT_ZVAL(zv);
          php_perl_sv_to_zval(my_perl, *prop_val, zv TSRMLS_CC);
          ret = zend_is_true(zv);
          zval_ptr_dtor(&zv);
        }
      } else {
        /* isset() */
        if (hv_exists(hv, Z_STRVAL_P(member), Z_STRLEN_P(member))) {
          ret = 1;
        }
      }
    } else {
      zend_error(E_WARNING, "[perl] Object is not a hash");
    }
  }
  if (member == &tmp_member) {
    zval_dtor(member);
  }
  return ret;
}

/* Deletes propery of hash based Perl's object */
static void php_perl_unset_property(zval *object, zval *member TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;
  zval tmp_member;

  if (member->type != IS_STRING) {
    tmp_member = *member;
    zval_copy_ctor(&tmp_member);
    convert_to_string(&tmp_member);
    member = &tmp_member;
  }

  if (sv == NULL) {
    if (obj->context == PERL_ARRAY) {
      AV *av = get_av(Z_STRVAL_P(member), FALSE);
      av_undef(av);
    } else if (obj->context == PERL_HASH) {
      HV *hv = get_hv(Z_STRVAL_P(member), FALSE);
      hv_undef(hv);
    } else {
      SV *sv = get_sv(Z_STRVAL_P(member), FALSE);
      sv_setsv(sv, &PL_sv_undef);
    }
  } else {
    while (SvTYPE(sv) == SVt_RV) {
      sv = SvRV(sv);
    }
    if (SvTYPE(sv) == SVt_PVHV) {
      HV*   hv = (HV*)sv;
      hv_delete(hv, Z_STRVAL_P(member), Z_STRLEN_P(member), G_DISCARD);
    } else {
      zend_error(E_WARNING, "[perl] Object is not a hash");
    }
  }
  if (member == &tmp_member) {
    zval_dtor(member);
  }
}

/* Constructs Perl object by calling constructor */
static void php_perl_constructor_handler(INTERNAL_FUNCTION_PARAMETERS)
{
  char* perl_class_name;
  int   perl_class_name_len;
  char* constructor     = "new";
  int   constructor_len = 3;
  int   argc = ZEND_NUM_ARGS();
  zval *object = this_ptr;

  if (argc == 0) {
    php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
    obj->sv = NULL;
    obj->properties = NULL;
  } else if (zend_parse_parameters((argc>2?2:argc) TSRMLS_CC, "s|s",
                            &perl_class_name, &perl_class_name_len,
                            &constructor, &constructor_len) != FAILURE) {
    PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
    php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
    zval*** argv = NULL;

    if (argc > 2) {
      argv = (zval***)safe_emalloc(sizeof(zval **), argc, 0);
      if (zend_get_parameters_array_ex(argc, argv) == FAILURE) {
        efree(argv);
        RETURN_FALSE;
      }
    }

    obj->sv = php_perl_call_constructor(my_perl,
                                        perl_class_name, perl_class_name_len,
                                        constructor, constructor_len,
                                        argc-2, argv+2 TSRMLS_CC);

    if (argc > 2) {
      efree(argv);
    }

    if(SvTRUE(ERRSV)) {
      STRLEN na;
      zval_ptr_dtor(&object);
      zend_throw_exception_ex(perl_exception_class_entry, 0 TSRMLS_CC,
        "[perl] constructor error: %s", SvPV(ERRSV, na));
      return;
    }

    php_perl_remember_object(obj->sv, object->value.obj.handle TSRMLS_CC);
  }
  zval_ptr_dtor(&object);
}

/* get_constructor handler for overloaded Perl objects */
static zend_function *php_perl_get_constructor(zval *object TSRMLS_DC)
{
  return (zend_function *)&php_perl_constructor_function;
}

/* get_method handler for overloaded Perl objects */
static zend_function *php_perl_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
  zend_internal_function *f;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);

  if (obj->sv == NULL) {
    zend_function *f = zend_get_std_object_handlers()->get_method(object, method, method_len TSRMLS_CC);
    if (f) {
      return f;
    }
  }
  f = emalloc(sizeof(zend_internal_function));
  memset(f, 0, sizeof(zend_internal_function));
  f->type = ZEND_OVERLOADED_FUNCTION_TEMPORARY;
/*???FIXME: Some tests fail with following code enabled
    f->scope = perl_class_entry;
*/
  f->function_name = estrndup(method, method_len);
  return (zend_function *)f;
}

/* Calls method of overloaded Perl's object */
static int php_perl_call_function_handler(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  zval *object = this_ptr;
  int argc = ZEND_NUM_ARGS();
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  zval ***argv = NULL;

  if (argc > 0) {
    argv = (zval***)safe_emalloc(sizeof(zval**), argc, 0);
    zend_get_parameters_array_ex(argc, argv);
  }

  if (obj->sv == NULL) {
    php_perl_call(my_perl, method, argc, argv, obj->context,
                  return_value_used?return_value:NULL  TSRMLS_CC);
  } else {
    php_perl_call_method(my_perl, obj->sv, method, argc, argv, obj->context,
                         return_value_used?return_value:NULL TSRMLS_CC);
  }

  zval_ptr_dtor(&object);
  if (argc > 0) {
    efree(argv);
  }

  if(SvTRUE(ERRSV)) {
    STRLEN na;
    zend_throw_exception_ex(perl_exception_class_entry, 0 TSRMLS_CC,
      "[perl] call error: %s", SvPV(ERRSV, na));
    return FAILURE;
  }

  return SUCCESS;
}

/* Returns all properties of Perl's object */
static HashTable* php_perl_get_properties(zval *object TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;
  HashTable *ht;

  if (sv == NULL) {
    return NULL;
  }
  if (obj->properties != NULL) {
    ht = obj->properties;

    if (ht->nApplyCount > 0) {
      return ht;
    } else if (active_opline != NULL) {
      /* each() support */
#define ZSTRCMP(zv,str) (zend_binary_strcmp(Z_STRVAL(zv),Z_STRLEN(zv),str,sizeof(str)-1)==0)
//#define ZSTRCMP(zv,str) ((Z_STRLEN(zv) == sizeof(str)-1) && \
//                         (memcmp(Z_STRVAL(zv), str, sizeof(str)-1) == 0))
      if (active_opline->opcode == ZEND_DO_FCALL &&
          active_opline->extended_value == 1 &&
          active_opline->op1.op_type == IS_CONST &&
          active_opline->op1.u.constant.type == IS_STRING &&
          (ZSTRCMP(active_opline->op1.u.constant, "each") ||
           ZSTRCMP(active_opline->op1.u.constant, "next") ||
           ZSTRCMP(active_opline->op1.u.constant, "prev") ||
           ZSTRCMP(active_opline->op1.u.constant, "key")  ||
           ZSTRCMP(active_opline->op1.u.constant, "current"))) {
        return ht;
      }
    }

    /* rebuild HashTable for properties */
    zend_hash_clean(ht);
  } else {
    ALLOC_HASHTABLE(ht);
    zend_hash_init(ht, 0, NULL, ZVAL_PTR_DTOR, 0);
    obj->properties = ht;
  }

  while (SvTYPE(sv) == SVt_RV) {
    sv = SvRV(sv);
  }
  if (SvTYPE(sv) == SVt_PVHV) {
    HV*   hv = (HV*)sv;
    SV*   el_sv;
    char* key;
    I32   key_len;

    hv_iterinit(hv);
    while ((el_sv = hv_iternextsv(hv, &key, &key_len)) != NULL) {
      zval* el_zv;
      ALLOC_INIT_ZVAL(el_zv);
      php_perl_sv_to_zval(my_perl, el_sv, el_zv TSRMLS_CC);
      zend_hash_add(ht, key, key_len+1, (void*)&el_zv, sizeof(zval *), NULL);
    }
  } else if (SvTYPE(sv) == SVt_PVAV) {
    AV* av = (AV*)sv;
    I32 len = av_len(av);
    I32 i;

    for (i = 0; i <= len; i++) {
      SV** el_sv = av_fetch(av, i, 0);
      if (el_sv != NULL && *el_sv != NULL) {
        zval* el_zv;
        ALLOC_INIT_ZVAL(el_zv);
        php_perl_sv_to_zval(my_perl, *el_sv, el_zv TSRMLS_CC);
        zend_hash_index_update(ht, i, (void*)&el_zv, sizeof(zval *), NULL);
      }
    }
  }
  return ht;
}

/* Returns class name of overloaded Perl's object */
static int php_perl_get_class_name(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  SV* sv = obj->sv;
  if (sv == NULL) {
    *class_name = emalloc(sizeof("Perl"));
    strcpy(*class_name,"Perl");
    *class_name_len = sizeof("Perl")-1;
    return SUCCESS;
  } else {
    HV* stash;
    while (SvTYPE(sv) == SVt_RV) {
     sv = SvRV(sv);
    }
    if ((stash = SvSTASH(sv)) != NULL) {
      char *name = HvNAME(stash);
      int len = strlen(name);
      *class_name = emalloc(len+sizeof("Perl::"));
      strcpy(*class_name,"Perl::");
      strcpy((*class_name)+sizeof("Perl::")-1,name);
      *class_name_len = len+sizeof("Perl::")-1;
      return SUCCESS;
    } else if (SvTYPE(sv) == SVt_PVAV) {
      *class_name = emalloc(sizeof("Perl::array"));
      strcpy(*class_name,"Perl::array");
      *class_name_len = sizeof("Perl::array")-1;
      return SUCCESS;
    } else if (SvTYPE(sv) == SVt_PVHV) {
      *class_name = emalloc(sizeof("Perl::hash"));
      strcpy(*class_name,"Perl::hash");
      *class_name_len = sizeof("Perl::hash")-1;
      return SUCCESS;
    } else {
      *class_name = emalloc(sizeof("Perl::scalar"));
      strcpy(*class_name,"Perl::scalar");
      *class_name_len = sizeof("Perl::scalar")-1;
      return SUCCESS;
    }
  }
  return FAILURE;
}

/* Returns class_entry of overloaded Perl's objects */
static zend_class_entry* php_perl_get_class_entry(zval *object TSRMLS_DC)
{
  return perl_class_entry;
}

/* Destructor for overloaded Perl's objects */
static void php_perl_destructor(void *perl_object, zend_object_handle handle TSRMLS_DC)
{
  if (perl_object) {
    php_perl_object *obj = (php_perl_object*)perl_object;
    PerlInterpreter* my_perl = PERLG(perl);
    if (obj->properties) {
      /* removing properties */
      zend_hash_destroy(obj->properties);
      FREE_HASHTABLE(obj->properties);
    }
    if (my_perl != NULL && obj->sv != NULL) {
      php_perl_forget_object(obj->sv TSRMLS_CC);
      /* removing perl object */
      sv_free(obj->sv);
    }
    efree(perl_object);
  }
}

static void php_perl_cleaner(void *perl_object, zend_object_handle handle TSRMLS_DC)
{
  if (perl_object) {
    php_perl_object *obj = (php_perl_object*)perl_object;
    if (obj->properties) {
      /* removing properties */
      zend_hash_destroy(obj->properties);
      FREE_HASHTABLE(obj->properties);
    }
    efree(perl_object);
  }
}

/* Makes a copy of overloaded perl object.
   It copies only properties or elements of object, but does'nt copy nested
   arrays, hashes or objects */
static zend_object_value php_perl_clone(zval *object TSRMLS_DC)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
  zend_object_value new_value;

  php_perl_object *old = zend_object_store_get_object(object TSRMLS_CC);
  php_perl_object *obj = (php_perl_object*)emalloc(sizeof(php_perl_object));

  if (old->sv != NULL) {
    SV* old_sv = SvRV(old->sv);
    SV* new_sv = NULL;

    if (SvTYPE(old_sv) == SVt_PVAV) {
      /* array */
      I32 len = av_len((AV*)old_sv);
      I32 i;

      new_sv = (SV*)newAV();
      for (i = 0; i <= len; i++) {
        SV** el_sv = av_fetch((AV*)old_sv, i, 0);
        if (el_sv != NULL && *el_sv != NULL) {
          av_push((AV*)new_sv, newSVsv(*el_sv));
        }
      }
    } else if (SvTYPE(old_sv) == SVt_PVHV) {
      /* hash */
      new_sv = (SV*)newHVhv((HV*)old_sv);
    } else if (SvOK(old_sv)) {
      /* scalar */
      new_sv = newSVsv(old_sv);
    } else {
      /* unknown */
      zend_error(E_ERROR, "[perl] Can't clone perl object (type [%ld])", SvTYPE(old_sv));
    }
    obj->sv = sv_bless(newRV_noinc(new_sv), SvSTASH(old_sv));
  } else {
    obj->sv = NULL;
  }
  obj->zo.ce = old->zo.ce;
  obj->properties = NULL;
  obj->context = PERL_SCALAR;

  new_value.handlers = &php_perl_object_handlers;
  new_value.handle =
    zend_objects_store_put(obj, php_perl_destructor, NULL, NULL TSRMLS_CC);

  if (obj->sv) {
    php_perl_remember_object(obj->sv, new_value.handle TSRMLS_CC);
  }

  return new_value;
}

/* Creates overloaded Perl's object */
static zend_object_value php_perl_create_object(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value new_value;

  php_perl_object *obj = (php_perl_object*)emalloc(sizeof(php_perl_object));
  obj->zo.ce = class_type;
  obj->sv = NULL;
  obj->properties = NULL;
  obj->context = PERL_SCALAR;

  new_value.handlers = &php_perl_object_handlers;
  new_value.handle =
    zend_objects_store_put(obj, php_perl_destructor, NULL, NULL TSRMLS_CC);

  return new_value;
}

/****************************************************************************/

static void php_perl_iterator_dtor(zend_object_iterator *iter TSRMLS_DC);
static int php_perl_iterator_valid(zend_object_iterator *iter TSRMLS_DC);
static void php_perl_iterator_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int php_perl_iterator_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void php_perl_iterator_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void php_perl_iterator_rewind(zend_object_iterator *iter TSRMLS_DC);

zend_object_iterator_funcs php_perl_iterator_funcs = {
  php_perl_iterator_dtor,
  php_perl_iterator_valid,
  php_perl_iterator_current_data,
  php_perl_iterator_current_key,
  php_perl_iterator_move_forward,
  php_perl_iterator_rewind,
};

static void php_perl_iterator_dtor(zend_object_iterator *iterator TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  zval_ptr_dtor(&object);
  efree(iterator);
}

static int php_perl_iterator_valid(zend_object_iterator *iterator TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  if (obj->properties) {
    return zend_hash_has_more_elements(obj->properties);
  }
  return FAILURE;
}

static void php_perl_iterator_current_data(zend_object_iterator *iterator, zval ***data TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  if (obj->properties) {
    zend_hash_get_current_data(obj->properties, (void**)data);
  }
}

static int php_perl_iterator_current_key(zend_object_iterator *iterator, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  if (obj->properties) {
    return zend_hash_get_current_key_ex(obj->properties, str_key, str_key_len, int_key, 1, NULL);
  }
  return HASH_KEY_NON_EXISTANT;
}

static void php_perl_iterator_move_forward(zend_object_iterator *iterator TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  if (obj->properties) {
    zend_hash_move_forward(obj->properties);
  }
}

static void php_perl_iterator_rewind(zend_object_iterator *iterator TSRMLS_DC)
{
  zval* object =(zval*)iterator->data;
  php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(object TSRMLS_CC);
  if (obj->properties) {
    /* removing properties */
    zend_hash_destroy(obj->properties);
    FREE_HASHTABLE(obj->properties);
    obj->properties = NULL;
  }
  php_perl_get_properties(object TSRMLS_CC);
  if (obj->properties) {
    zend_hash_internal_pointer_reset(obj->properties);
  }
}

zend_object_iterator *php_perl_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
  zend_object_iterator *iterator = emalloc(sizeof(zend_object_iterator));
  object->refcount++;
  iterator->data = (void*)object;
  iterator->funcs = &php_perl_iterator_funcs;

  return iterator;
}
/****************************************************************************/

static void php_perl_init_globals(zend_perl_globals *perl_globals)
{
  perl_globals->perl = NULL;
}

PHP_MINIT_FUNCTION(perl)
{
  zend_class_entry perl_ce;
  ZEND_INIT_MODULE_GLOBALS(perl, php_perl_init_globals, NULL);

  INIT_CLASS_ENTRY(perl_ce, "Perl", perl_functions);
  perl_ce.create_object = php_perl_create_object;
  perl_class_entry = zend_register_internal_class(&perl_ce TSRMLS_CC);
  perl_class_entry->get_iterator = php_perl_get_iterator;

  INIT_CLASS_ENTRY(perl_ce, "PerlException", NULL);
  perl_exception_class_entry = zend_register_internal_class_ex(&perl_ce, zend_exception_get_default(), NULL TSRMLS_CC);

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(perl)
{
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(perl)
{
  php_perl_destroy(TSRMLS_C);
  return SUCCESS;
}

PHP_MINFO_FUNCTION(perl)
{
  PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);

  php_info_print_table_start();
  php_info_print_table_header(2, "Perl support", "enabled");
  php_info_print_table_row(2, "Revision", "$Revision$");
  php_info_print_table_row(2, "Perl version", Perl_form(aTHX_ "%vd",PL_patchlevel));
  php_info_print_table_end();
}

/* perl_require($perl_file_name)
   Loads and executes Perl file. Produces a error if file does not exist or
   has Perl's errors */
PHP_METHOD(Perl, require)
{
  char* perl_filename;
  int   perl_filename_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &perl_filename, &perl_filename_len) == FAILURE) {
    return;
  } else {
    PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
    require_pv(perl_filename);
    if(SvTRUE(ERRSV)) {
      STRLEN na;

      zend_throw_exception_ex(perl_exception_class_entry, 0 TSRMLS_CC,
        "[perl] require error: %s", SvPV(ERRSV, na));
    }
  }
}

/* perl_eval($perl_code)
   Evaluates Perl code and returns result.
   Produces a error if code has Perl's errors */
PHP_METHOD(Perl, eval)
{
  char* perl_code;
  int   perl_code_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &perl_code, &perl_code_len) != FAILURE) {
    PerlInterpreter* my_perl = php_perl_init(TSRMLS_C);
    php_perl_object *obj = (php_perl_object*)zend_object_store_get_object(this_ptr TSRMLS_CC);
    SV* sv;
    dSP;
    sv = newSVpv(perl_code, perl_code_len);
    if (return_value_used) {
      if (obj->context != PERL_SCALAR) {
        int count, i;
        I32 ax;
        HashTable var_hash;

        count = eval_sv(sv, G_ARRAY|G_KEEPERR);
        SPAGAIN;
        sp -= count;
        ax = (sp - PL_stack_base) + 1;
        zend_hash_init(&var_hash, 0, NULL, NULL, 0);
        array_init(return_value);
        if (obj->context == PERL_ARRAY) {
          for (i = 0; i < count; i++) {
            add_next_index_zval(return_value,
              php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(i), NULL, &var_hash TSRMLS_CC));
          }
        } else {
          for (i = 0; i < count; i++) {
            int  key_len;
            char *key = SvPV(ST(i), key_len);
            if (i != count-1) {
              add_assoc_zval_ex(return_value, key, key_len+1,
                php_perl_sv_to_zval_ref(my_perl, (SV*)(SV*)ST(++i), NULL, &var_hash TSRMLS_CC));
            } else {
              add_assoc_null_ex(return_value, key, key_len+1);
            }
          }
        }
        zend_hash_destroy(&var_hash);
      } else {
        eval_sv(sv, G_SCALAR|G_KEEPERR);
        SPAGAIN;
        php_perl_sv_to_zval(my_perl, POPs, return_value TSRMLS_CC);
      }
      PUTBACK;
    } else {
      eval_sv(sv, G_DISCARD|G_KEEPERR);
    }
    sv_free(sv);

    if (SvTRUE(ERRSV)) {
      STRLEN na;
      zend_throw_exception_ex(perl_exception_class_entry, 0 TSRMLS_CC,
        "[perl] eval error: %s", SvPV(ERRSV, na));
    }
  }
}

zend_module_entry perl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "perl",
  NULL,
  PHP_MINIT(perl),
  PHP_MSHUTDOWN(perl),
  NULL,
  PHP_RSHUTDOWN(perl),
  PHP_MINFO(perl),
#if ZEND_MODULE_API_NO >= 20010901
  "0.7-dev",
#endif
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PERL
ZEND_GET_MODULE(perl)
#endif

#endif
