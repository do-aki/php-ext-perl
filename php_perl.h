/* perl extension for PHP */

#ifndef PHP_PERL_H
#define PHP_PERL_H

extern zend_module_entry perl_module_entry;
#define phpext_perl_ptr &perl_module_entry

#define PHP_PERL_VERSION "2.0.0"

# if defined(ZTS) && defined(COMPILE_DL_PERL)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

ZEND_BEGIN_MODULE_GLOBALS(perl)
    PerlInterpreter *my_perl;
    HashTable       perl_objects; /* this hash is used to make one to one mapping between Perl and PHP objects */
ZEND_END_MODULE_GLOBALS(perl)

#ifdef ZTS
#define PERL_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(perl, v)
#else
#define PERL_G(v) (perl_globals.v)
#endif


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
    ZEND_PARSE_PARAMETERS_START(0, 0) \
    ZEND_PARSE_PARAMETERS_END()
#endif

ZEND_EXTERN_MODULE_GLOBALS(perl)

static zend_object_handlers php_perl_object_handlers;
static zend_class_entry *perl_ce = NULL;
static zend_class_entry *perl_exception_ce = NULL;

typedef enum _perl_context {
    PERL_UNDEF,
    PERL_SCALAR,
    PERL_ARRAY,
    PERL_HASH
} perl_context;

typedef struct php_perl_t {
    SV           *sv;           /* Perl's representation of object */
    HashTable    *properties;   /* Temporary collection of object properties */
    perl_context  context;      /* flag for next perl call, property */
    zend_object   std;
} php_perl_t;

static inline php_perl_t *php_perl_fetch_object(const zend_object* obj) {
    return (php_perl_t *)((char *)(obj) - XtOffsetOf(php_perl_t, std));
}

#define Z_PHPPERL_P(zv)  php_perl_fetch_object(Z_OBJ_P(zv))
#define Z_PHPPERL(zv)  php_perl_fetch_object(Z_OBJ(zv))

zend_object* php_perl_new();
bool zval_is_perl_obj(zval* zv);

#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
    #define DEBUG_PRINT(fmt, args...) \
        fprintf(stderr, "%s.%d(): " fmt "\n", __func__, __LINE__, ##args); \
        fflush(stderr);
#else
    #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif    /* PHP_PERL_H */
