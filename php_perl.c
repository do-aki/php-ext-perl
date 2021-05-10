/**
 * php-ext-perl
 *
 * Embeds perl interpreter into PHP and allows execution and manipulation of
 * Perl from within PHP code
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#if HAVE_PERL

#include <EXTERN.h>               /* from the Perl distribution */
#include <perl.h>                 /* from the Perl distribution */
#include <perliol.h>              /* from the Perl distribution */
#include <perlapi.h>              /* from the Perl distribution */

#undef END_EXTERN_C               /* bypass macros redeclaration warning */

#include "php.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_smart_str.h"
#include "SAPI.h"

#include "php_perl.h"
#include "perl_arginfo.h"
#include "sv_to_zv.h"
#include "zv_to_sv.h"

ZEND_DECLARE_MODULE_GLOBALS(perl)

/****************************************************************************/
/* This code was produced by `perl -MExtUtils::Embed -e xsinit`             */

EXTERN_C void xs_init (pTHX);

EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);

EXTERN_C void
xs_init(pTHX)
{
    static const char file[] = __FILE__;
    dXSUB_SYS;
    PERL_UNUSED_CONTEXT;

    /* DynaLoader is a special case */
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}

/****************************************************************************/

/** Perl global management and extension definition below */

// PerlIO functions - redirects perl stdout to zend output buffer
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
    sapi_flush();
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

/* Creates or returns perl interpreter */
static PerlInterpreter *php_perl_init()
{
    PerlInterpreter *my_perl = PERL_G(my_perl);
    if (my_perl == NULL) {
        char *embedding[] = { "", "-e", "0" };
        my_perl = perl_alloc();
        perl_construct(my_perl);
        PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
        perl_parse(my_perl, xs_init, 3, embedding, NULL);
        PerlIO_define_layer(aTHX_ &PerlIO_PHP);
        PerlIO_apply_layers(aTHX_ PerlIO_stdout(), "w", ":PHP");
        PERL_G(my_perl) = my_perl;

        zend_hash_init(&PERL_G(perl_objects), 0, NULL, NULL, 0);
    }
    return my_perl;
}

static inline SV* php_perl_dereference_sv(SV* sv)
{
    while (SvROK(sv)) {
        sv = SvRV(sv);
    }
    return sv;
}

/* Destroys perl interpreter */
static void php_perl_destroy()
{
    PerlInterpreter *my_perl = PERL_G(my_perl);

    if (my_perl != NULL) {
        zend_hash_destroy(&PERL_G(perl_objects));
        perl_destruct(my_perl);
        perl_free(my_perl);
        PERL_G(my_perl) = NULL;
    }
}

/**
 * Allocates and returns a zend_object of the Perl class
 */
zend_object* _php_perl_new(zend_class_entry *ce)
{
    DEBUG_PRINT("php_perl_new");
    php_perl_t *perl_class = zend_object_alloc(sizeof(php_perl_t), ce);
    perl_class->sv = NULL;
    perl_class->properties = NULL;
    perl_class->context = PERL_SCALAR;
    zend_object_std_init(&perl_class->std, ce);
    object_properties_init(&perl_class->std, ce);
    perl_class->std.handlers = &php_perl_object_handlers;
    return &perl_class->std;
}

zend_object* php_perl_new()
{
    return _php_perl_new(perl_ce);
}

bool zval_is_perl_obj(zval* zv)
{
    return
        zv != NULL
        && Z_TYPE_P(zv) == IS_OBJECT
        && Z_OBJCE_P(zv) == perl_ce;
}

static zval* php_perl_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot)
{
    // Since all our properties are proxies, the VM can't ever do direct r/w
    // access, so we always return null.
    return NULL;
}

/* Returns all properties of Perl's object */
static HashTable* php_perl_get_properties(zend_object *obj)
{
    DEBUG_PRINT("get_properties. zo %p", obj);
    HashTable *props;
    php_perl_t *perlobj;

    perlobj = php_perl_fetch_object(obj);
    props = zend_array_dup(zend_std_get_properties(obj));
    SV* sv = perlobj->sv;

    if (sv == NULL) {
        return props;
    }

    PerlInterpreter* my_perl = php_perl_init();

    sv = php_perl_dereference_sv(sv);
    // Non hash or array type SVs don't have properties. e.g. someone's made a
    // proxy to a string and tried to var_dump it.
    if (SvTYPE(sv) != SVt_PVHV && SvTYPE(sv) != SVt_PVAV) {
        return props;
    }

    zval* zv = malloc(sizeof(zval));
    php_perl_sv_to_zval(my_perl, sv, zv);
    props = Z_ARRVAL_P(zv);
    perlobj->properties = props;
    DEBUG_PRINT("finished: get_properties. zo %p. props in %p", obj, props);
    return props;
}

/** Add perl internal object name to var dumps **/
static zend_string* php_perl_get_class_name(const zend_object *obj)
{
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(obj);
    PerlInterpreter* my_perl = php_perl_init();
    SV* sv = perlobj->sv;

    smart_str class_name = {0};
    smart_str_appends(&class_name, "Perl");

    if (sv == NULL) {
        smart_str_0(&class_name);
        return class_name.s;
    }
    smart_str_appends(&class_name, "::");

    if (!sv_isobject(sv)) {
        smart_str_appends(&class_name, "proxy::");
        if (SvTYPE(sv) == SVt_PVAV) {
            smart_str_appends(&class_name, "array");
        } else if (SvTYPE(sv) == SVt_PVHV) {
            smart_str_appends(&class_name, "hash");
        } else {
            smart_str_appends(&class_name, "scalar");
        }
        smart_str_0(&class_name);
        return class_name.s;
    }

    sv = php_perl_dereference_sv(sv);
    HV* stash;
    if ((stash = SvSTASH(sv)) != NULL) {
        smart_str_appends(&class_name, HvNAME(stash));
    } else {
        smart_str_appends(&class_name, "unknown");
    }
    smart_str_0(&class_name);
    return class_name.s;
}

/* Used to check if a property of the object exists */
/* param has_set_exists:
 * 0 (has) whether property exists and is not NULL
 * 1 (set) whether property exists and is true
 * 2 (exists) whether property exists
 */
static int php_perl_has_property(zend_object *object, zend_string *member, int has_set_exists, void **cache_slot)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    DEBUG_PRINT("has_property for %s, %d", ZSTR_VAL(member), has_set_exists);

    if (sv == NULL) {
        // Check for glob in perl context
        if (perlobj->context == PERL_ARRAY) {
            sv = (SV*)get_av(ZSTR_VAL(member), FALSE);
            if (sv && !AvARRAY(sv))
                return FALSE;
        } else if (perlobj->context == PERL_HASH) {
            sv = (SV*)get_hv(ZSTR_VAL(member), FALSE);
            if (sv && !HvARRAY(sv))
                return FALSE;
        } else {
            sv = get_sv(ZSTR_VAL(member), FALSE);
            if (sv && !SvOK(sv))
                return FALSE;
        }
    } else {
        sv = php_perl_dereference_sv(sv);
        if (SvTYPE(sv) == SVt_PVHV) {
            HV *hv = (HV*)sv;
            SV** prop_val = hv_fetch(hv, ZSTR_VAL(member), ZSTR_LEN(member), 0);
            if (prop_val == NULL)
                return FALSE;
            sv = *prop_val;
        } else {
            // TODO: Should we do this? PHP doesn't seem to even throw a notice
            zend_error(E_WARNING, "[perl] Object is not a hash");
            return FALSE;
        }
    }

    if (sv == NULL)
        return FALSE;

    // Exists - is true if we got this far
    if (has_set_exists == ZEND_PROPERTY_EXISTS)
        return TRUE;

    zval* zv = malloc(sizeof(zval));
    php_perl_sv_to_zval(my_perl, sv, zv);

    // has - property exists and is not NULL
    if (has_set_exists == ZEND_PROPERTY_ISSET)
        return !ZVAL_IS_NULL(zv);

    // set - property exists and is true
    if (has_set_exists == ZEND_PROPERTY_NOT_EMPTY)
        return zval_is_true(zv);
}

/** Convert perlObject->sv to zval **/
static zval* php_perl_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t* perl_obj = php_perl_fetch_object(object);
    SV* sv = NULL;
    zend_bool write =
        perl_obj->context != PERL_SCALAR &&
        type != BP_VAR_R &&
        type != BP_VAR_IS;

    DEBUG_PRINT("[%p] getting member %s (call type %d)", object, ZSTR_VAL(member), type);
    if (perl_obj->context == PERL_SCALAR) {
        perl_context new_context;
        if (
            (zend_string_equals_literal(member, "array") && (new_context = PERL_ARRAY))
            || (zend_string_equals_literal(member, "hash") && (new_context = PERL_HASH))
            || (zend_string_equals_literal(member, "scalar") && (new_context = PERL_SCALAR))
        ) {
            DEBUG_PRINT("[%p] getting proxy obj type %d", object, new_context);
            // Create new proxy object
            zend_object* new_obj = php_perl_new();
            php_perl_t* new_class = php_perl_fetch_object(new_obj);
            new_class->context = new_context;
            if (perl_obj->sv != NULL) {
                new_class->sv = perl_obj->sv;
            }
            SvREFCNT_inc(new_class->sv);
            ZVAL_OBJ(rv, new_obj);
            return rv;
        }
    }

    if (perl_obj->sv == NULL) {
        if (perl_obj->context == PERL_ARRAY) {
            sv = (SV*)get_av(ZSTR_VAL(member), write);
            if (sv && !AvARRAY(sv)) {if (write) {av_clear((AV*)sv);} else {sv = NULL;}}
        } else if (perl_obj->context == PERL_HASH) {
            sv = (SV*)get_hv(ZSTR_VAL(member), write);
            if (sv && !HvARRAY(sv)) {if (write) {hv_clear((HV*)sv);} else {sv = NULL;}}
        } else {
            sv = get_sv(ZSTR_VAL(member), FALSE);
            if (sv && !SvOK(sv)) {sv = NULL;}
        }
        if (sv == NULL) {
            if (perl_obj->context == PERL_ARRAY) {
                zend_error(E_NOTICE, "[perl] Undefined variable: '@%s'", ZSTR_VAL(member));
            } else if (perl_obj->context == PERL_HASH) {
                zend_error(E_NOTICE, "[perl] Undefined variable: '%%%s'", ZSTR_VAL(member));
            } else {
                zend_error(E_NOTICE, "[perl] Undefined variable: '$%s'", ZSTR_VAL(member));
            }
        }
    } else {
        sv = perl_obj->sv;
        sv = php_perl_dereference_sv(sv);
        if (SvTYPE(sv) == SVt_PVHV) { /** Hash **/
            HV* hv = (HV*)sv;
            SV** prop_val;

            prop_val = hv_fetch(hv, ZSTR_VAL(member), ZSTR_LEN(member), 0);
            if (prop_val != NULL) {
                sv = *prop_val;
                write = FALSE;
                if (sv != NULL && type != BP_VAR_R && type != BP_VAR_IS) {
                    write = TRUE;
                    SV* tmp_sv = sv;
                    while (1) {
                        if (sv_isobject(tmp_sv) ||
                            SvTYPE(tmp_sv) == SVt_PVAV ||
                            SvTYPE(tmp_sv) == SVt_PVHV) {
                            write = TRUE; // Return proxy object
                            break;
                        } else if (!SvROK(tmp_sv)) {
                            break; // Return directly translated zval
                        }
                        tmp_sv = SvRV(tmp_sv);
                    }
                }
            }
        } else {
            // TODO: We need to reimplement proxy objects here. Look for special
            // "data" prop being set and use it to assign to the underlying SV.
            // This would be the same idea as FFI::cdata.
            zend_error(E_WARNING, "[perl] Object is not a hash");
        }
    }

    if (sv != NULL) {
        if (write && !sv_isobject(sv)) {
            DEBUG_PRINT("[%p] getting proxy obj", object);
            // Create new proxy object
            zend_object* new_obj = php_perl_new();
            php_perl_t* new_class = php_perl_fetch_object(new_obj);
            new_class->sv = newRV(sv);
            DEBUG_PRINT("[%p] proxy obj has SvRV %p", object, new_class->sv);
            ZVAL_OBJ_COPY(rv, new_obj);
            return rv;
        } else {
            php_perl_sv_to_zval(my_perl, sv, rv);
            return rv;
        }
    }
    ZVAL_NULL(rv);
    return rv;
}

static zval* php_perl_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t* perl_obj = php_perl_fetch_object(object);
    SV* sv = perl_obj->sv;
    char* property = ZSTR_VAL(member);

    DEBUG_PRINT("Writing property %s on perl_obj %p with context type %d", property, perl_obj, perl_obj->context);

    if (sv == NULL && perl_obj->context == PERL_ARRAY) {
        DEBUG_PRINT("perl_obj is an array proxy, sv is NULL");
        if (Z_TYPE_P(value) != IS_ARRAY) {
            zend_error(E_WARNING, "[perl] array assignment ignored, array required");
            // "You must return the final value of the assigned property."
            zval* ret = malloc(sizeof(ret)); ZVAL_NULL(ret); return ret;
        }
        AV* av = get_av(property, TRUE);
        php_perl_zval_to_sv(my_perl, value, (SV*)av);
        return value;
    } else if (sv == NULL && perl_obj->context == PERL_HASH) {
        DEBUG_PRINT("perl_obj is a hash proxy, sv is NULL");
        if (Z_TYPE_P(value) != IS_ARRAY) {
            zend_error(E_WARNING, "[perl] hash assignment ignored, array required");
            zval* ret = malloc(sizeof(ret)); ZVAL_NULL(ret); return ret;
        }
        HV *hv = get_hv(property, TRUE);
        php_perl_zval_to_sv(my_perl, value, (SV*)hv);
        return value;
    } else if (sv == NULL) {
        DEBUG_PRINT("perl_obj is not a proxy, sv is NULL");
        SV *sv = get_sv(property, TRUE);
        php_perl_zval_to_sv(my_perl, value, sv);
        return value;
    } else {
        DEBUG_PRINT("perl_obj is a scalar proxy, sv is %p", sv);
        SV* origSv = sv;
        sv = php_perl_dereference_sv(sv);
        if (SvTYPE(sv) == SVt_PVHV) {
            DEBUG_PRINT("sv is hash %p", sv);
            HV* hv = (HV*)sv;
            // Special check for recursion
            if (zval_is_perl_obj(value) && Z_PHPPERL_P(value)->sv == perl_obj->sv) {
                DEBUG_PRINT("detected set_property recursion, reacting accordingly");
                hv_store(hv, property, ZSTR_LEN(member), origSv, 0);
                SvREFCNT_inc(origSv);
                return value;
            }
            SV* el_sv = newSV(0);
            php_perl_zval_to_sv(my_perl, value, el_sv);
            hv_store(hv, property, ZSTR_LEN(member), el_sv, 0);
            return value;
        } else {
            zend_error(E_WARNING, "[perl] Object is not a hash");
            zval* ret = malloc(sizeof(ret)); ZVAL_NULL(ret); return ret;
        }
    }
}

static void php_perl_unset_property(zend_object *object, zend_string *member, void **cache_slot)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    if (sv == NULL) {
        if (perlobj->context == PERL_ARRAY) {
            AV *av = get_av(ZSTR_VAL(member), FALSE);
            av_undef(av);
        } else if (perlobj->context == PERL_HASH) {
            HV *hv = get_hv(ZSTR_VAL(member), FALSE);
            hv_undef(hv);
        } else {
            SV *sv = get_sv(ZSTR_VAL(member), FALSE);
            sv_setsv(sv, &PL_sv_undef);
        }
    } else {
        sv = php_perl_dereference_sv(sv);
        if (SvTYPE(sv) == SVt_PVHV) {
            HV*   hv = (HV*)sv;
            hv_delete(hv, ZSTR_VAL(member), ZSTR_LEN(member), G_DISCARD);
        } else {
            // TODO: PHP doesn't seem to throw a notice or anything in equiv
            // circumstances, should we just ignore this?
            zend_error(E_WARNING, "[perl] Object is not a hash");
        }
    }
}

static int php_perl_has_dimension(zend_object *object, zval *member, int check_empty)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    DEBUG_PRINT("has_dimension for %d, %d", Z_LVAL_P(member), check_empty);

    if (sv == NULL) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non proxy object");
        return FALSE;
    }
    sv = php_perl_dereference_sv(sv);
    if (SvTYPE(sv) == SVt_PVHV) {
        // Old lib doesn't do this convert to string step but I think it should?
        convert_to_string(member);
        return php_perl_has_property(object, zval_get_string(member), check_empty ? ZEND_PROPERTY_NOT_EMPTY : ZEND_PROPERTY_ISSET, NULL);
    }
    if (SvTYPE(sv) == SVt_PVAV) {
        AV* av = (AV*)sv;

        convert_to_long(member);

        int exists = av_exists(av, Z_LVAL_P(member));
        if (exists && !check_empty)
            return TRUE;
        if (!exists)
            return FALSE;

        // It exists, but we need to check "emptiness" of the value
        SV** el_sv = av_fetch(av, Z_LVAL_P(member), 0);
        if (el_sv != NULL) {
            zval *el_zv = malloc(sizeof(zval));
            php_perl_sv_to_zval(my_perl, *el_sv, el_zv);
            int ret = zend_is_true(el_zv);
            zval_ptr_dtor(el_zv);
            return ret;
        }
        return FALSE;
    }
    // TODO: What sort of error is most like PHP here?
    zend_error(E_WARNING, "[perl] Can't do array access on a scalar proxy object");
    return FALSE;
}

static void php_perl_write_dimension(zend_object *object, zval *offset, zval *value)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    DEBUG_PRINT("write_dimension for %d, sv %p", Z_LVAL_P(offset), sv);

    if (sv == NULL) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non proxy object");
        return;
    }
    sv = php_perl_dereference_sv(sv);
    if (SvTYPE(sv) == SVt_PVHV) {
        // Old lib doesn't do this convert to string step but I think it should?
        convert_to_string(offset);
        php_perl_write_property(object, zval_get_string(offset), value, NULL);
        return;
    }
    if (SvTYPE(sv) == SVt_PVAV) {
        AV* av = (AV*)sv;
        convert_to_long(offset);
        SV* el_sv = newSV(0);
        php_perl_zval_to_sv(my_perl, value, el_sv);
        av_store(av, Z_LVAL_P(offset), el_sv);
        return;
    }
    zend_error(E_WARNING, "[perl] Object is not an array");
}

static zval* php_perl_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    zend_bool write =
        type != BP_VAR_R &&
        type != BP_VAR_IS;

    DEBUG_PRINT("read_dimension for %d, write = %d", Z_LVAL_P(offset), write);

    if (sv == NULL) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non proxy object");
        ZVAL_NULL(rv);
        return rv;
    }
    sv = php_perl_dereference_sv(sv);
    if (SvTYPE(sv) == SVt_PVHV) {
        // Old lib doesn't do this convert to string step but I think it should?
        convert_to_string(offset);
        return php_perl_read_property(object, zval_get_string(offset), type, NULL, rv);
    }
    if (SvTYPE(sv) != SVt_PVAV) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non-array proxy object");
        ZVAL_NULL(rv);
        return rv;
    }

    convert_to_long(offset);
    long idx = Z_LVAL_P(offset);

    AV* av = (AV*)sv;
    SV** el_sv;
    el_sv = av_fetch(av, idx, 0);

    // We want to emulate PHP's behaviour, where a set but undef array access
    // does NOT throw a warning, but an unset array access does. We could pass
    // "write" as the third param to av_fetch above but we wouldn't be able to
    // tell the difference between `$self->[0] = undef` and something freshly
    // created
    if (el_sv && !SvOK(*el_sv)) {
        DEBUG_PRINT("existing but undef array key %d was accessed (el_sv %p)", *el_sv);
        if (write) {
            DEBUG_PRINT("accessed for write, so setting IV = 0");
            sv_setiv(*el_sv, 0);
            // Continue down to the proxy object stuff
        } else {
            DEBUG_PRINT("accessed for read, returning null");
            ZVAL_NULL(rv);
            return rv;
        }
    }
    if (!el_sv) {
        // Was not set, throw a warning
        DEBUG_PRINT("non existing array key %d was accessed, throwing a warning", idx);
        zend_error(E_WARNING, "[perl] Undefined array key %d", idx);
        if (write) {
            DEBUG_PRINT("accessed for write, so creating");
            SV* newsv = newSViv(0);
            el_sv = av_store(av, idx, newsv);
            // Continue down to proxy object stuff
        } else {
            DEBUG_PRINT("accessed for read, returning null");
            ZVAL_NULL(rv);
            return rv;
        }
    }

    if (write && !sv_isobject(sv)) {
        DEBUG_PRINT("[%p] getting proxy obj for el_sv %p", object, *el_sv);
        // Create new proxy object
        zend_object* new_obj = php_perl_new();
        php_perl_t* new_class = php_perl_fetch_object(new_obj);
        new_class->sv = newRV(*el_sv);
        ZVAL_OBJ(rv, new_obj);
        return rv;
    } else {
        DEBUG_PRINT("Converting el_sv %p into rv %p", *el_sv, rv);
        php_perl_sv_to_zval(my_perl, *el_sv, rv);
        return rv;
    }
}

static void php_perl_unset_dimension(zend_object *object, zval *offset)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    SV* sv = perlobj->sv;

    DEBUG_PRINT("unset_dimension %d", Z_LVAL_P(offset), write);

    if (sv == NULL) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non proxy object");
        return;
    }
    sv = php_perl_dereference_sv(sv);
    if (SvTYPE(sv) == SVt_PVHV) {
        // Old lib doesn't do this convert to string step but I think it should?
        convert_to_string(offset);
        php_perl_unset_property(object, zval_get_string(offset), NULL);
        return;
    }
    if (SvTYPE(sv) != SVt_PVAV) {
        zend_error(E_ERROR, "[perl] Can't do array access on a non-array proxy object");
        return;
    }
    AV* av = (AV*)sv;
    convert_to_long(offset);
    av_delete(av, Z_LVAL_P(offset), G_DISCARD);
}

static void php_perl_call_method_or_pv(
    php_perl_t* obj, char* func, zval* argv, int argc, zval* return_value
) {
    DEBUG_PRINT("in php_perl_call_method_or_pv");
    PerlInterpreter* my_perl = php_perl_init();

    dSP;                            /* initialize stack pointer         */
    ENTER;                          /* everything created after here    */
    SAVETMPS;                       /* ...is a temporary variable.      */
    PUSHMARK(SP);                   /* remember the stack pointer       */

    if (obj->sv != NULL) {
        DEBUG_PRINT("call: object context -> %s", func);
        XPUSHs(obj->sv); // @_[0] = self
    } else {
        DEBUG_PRINT("call: main context -> %s", func);
    }

    for (int i = 0; i < argc; i++) {
        SV* el_sv = newSV(0);
        php_perl_zval_to_sv(my_perl, &argv[i], el_sv);
        XPUSHs(sv_2mortal(el_sv));
    }

    PUTBACK;                        /* make local stack pointer global  */

    int count;
    if (obj->sv == NULL)
        count = call_pv(func, (obj->context == PERL_SCALAR ? G_SCALAR : G_ARRAY) | G_EVAL); /* call the function */
    else
        count = call_method(func, (obj->context == PERL_SCALAR ? G_SCALAR : G_ARRAY) | G_EVAL); /* call the method */

    // Get updated stack pointer
    SPAGAIN;
    // Extend the stack to make room for the return SV before pushing the marks
    EXTEND(SP, 1);
    // Push the list boundaries to the mark stack
    PUSHMARK(SP - count);
    PUSHMARK(SP);
    // Make an SV for perl to assign the stacked list to
    SV* lelem;
    if (obj->context == PERL_ARRAY) {
        AV* av = newAV();
        lelem = (SV*) av;
    } else if (obj->context == PERL_HASH) {
        HV* hv = newHV();
        lelem = (SV*) hv;
    } else {
        lelem = newSV(0);
    }
    PUSHs(lelem);
    PUTBACK;                        /* make local stack pointer global  */

    // Make a fake assign op so we can call OP_AASSIGN and get back a hash or
    // array depending on what our caller wants.
    BINOP myop;
    Zero(&myop, 1, BINOP);
    myop.op_flags = (obj->context == PERL_SCALAR ? OPf_WANT_SCALAR : OPf_WANT_LIST ) | OPf_STACKED;
    myop.op_ppaddr = PL_ppaddr[OP_AASSIGN];
    myop.op_type = OP_AASSIGN;
    myop.op_next = NULL;
    myop.op_private = OPpASSIGN_TRUEBOOL;
    PL_op = (OP *) &myop;
    PL_ppaddr[OP_AASSIGN](aTHX);

    SPAGAIN;
    if (return_value != NULL) {
        php_perl_sv_to_zval(my_perl, lelem, return_value);
    }
    PUTBACK;
    FREETMPS;                       /* free that return value           */
    LEAVE;                          /* ...and the XPUSHed "mortal" args.*/

    if(SvTRUE(ERRSV)) {
        STRLEN na;
        zend_throw_exception_ex(
            perl_exception_ce,
            0,
            "[perl] method call error: %s",
            SvPV(ERRSV, na)
        );
        return;
    }
}

static PHP_FUNCTION(php_perl_overloaded_call)
{
    php_perl_t *obj = Z_PHPPERL_P(ZEND_THIS);
    DEBUG_PRINT("%s->%s() called!", ZSTR_VAL(php_perl_get_class_name(&obj->std)), ZSTR_VAL(EX(func)->common.function_name));

    zval* argv;
    int argc = 0;
    ZEND_PARSE_PARAMETERS_START(0, -1);
        Z_PARAM_OPTIONAL
        Z_PARAM_VARIADIC('+', argv, argc)
    ZEND_PARSE_PARAMETERS_END();

    php_perl_call_method_or_pv(obj, ZSTR_VAL(EX(func)->common.function_name), argv, argc, USED_RET() ? return_value : NULL);

    /* Cleanup trampoline */
    ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
    zend_string_release(EX(func)->common.function_name);
    zend_free_trampoline(EX(func));
    EX(func) = NULL;
}

static zend_function* php_perl_get_method(zend_object **object, zend_string *method_name, const zval *key)
{
    DEBUG_PRINT("in php_perl_get_method %s", ZSTR_VAL(method_name));
    zend_function *retval = NULL;
    if ((retval = zend_std_get_method(object, method_name, key)) != NULL) {

        return retval;
    }

    zend_internal_function *fptr;

    if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
        fptr = (zend_internal_function *) &EG(trampoline);
    } else {
        fptr = emalloc(sizeof(zend_internal_function));
    }
    memset(fptr, 0, sizeof(zend_internal_function));
    fptr->type = ZEND_INTERNAL_FUNCTION;
    fptr->num_args = 1;
    fptr->scope = (*object)->ce;
    fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
    fptr->function_name = zend_string_copy(method_name);
    fptr->handler = ZEND_FN(php_perl_overloaded_call);
    return (zend_function*)fptr;
}


static int php_perl_do_operation(zend_uchar opcode, zval *result, zval *op1, zval *op2)
{
    DEBUG_PRINT("do_operation: opcode %d", opcode);
    if (!zval_is_perl_obj(op1)) {
        zend_error(E_ERROR, "[perl] invalid operation on non perl object");
        return FAILURE;
    }
    php_perl_t* perlobj = Z_PHPPERL_P(op1);
    PerlInterpreter* my_perl = php_perl_init();
    SV* sv = perlobj->sv;
    if (sv == NULL) {
        zend_error(E_ERROR, "[perl] can't do overloaded operation on non proxy object");
        return FAILURE;
    }

    switch (opcode) {
    case ZEND_ADD:
    case ZEND_SUB:
        sv = php_perl_dereference_sv(sv);
        I32 currentValue;
        long resultValue;
        if (SvIOK(sv)) {
            currentValue = SvIV(sv);
        } else {
            zend_error(E_ERROR, "[perl] can't do arithmetic on sv type %d", SvTYPE(sv));
            return FAILURE;
        }
        DEBUG_PRINT("op2 of type %d", Z_TYPE_P(op2));
        convert_to_long(op2);
        DEBUG_PRINT("op2 has long val %d", Z_LVAL_P(op2));
        if (opcode == ZEND_ADD)
            resultValue = currentValue + Z_LVAL_P(op2);
        else
            resultValue = currentValue - Z_LVAL_P(op2);
        sv_setiv(sv, resultValue);
        ZVAL_LONG(result, resultValue);
        DEBUG_PRINT("result is set to %d", Z_LVAL_P(result));
        return SUCCESS;
    default:
        zend_error(E_ERROR, "[perl] can't handle opcode %d", opcode);
        return FAILURE;
    }
}

static zend_object* php_perl_clone(zend_object *oldObject)
{
    PerlInterpreter* my_perl = php_perl_init();
    php_perl_t* oldClass = php_perl_fetch_object(oldObject);
    zend_object* newObject = php_perl_new();
    php_perl_t* newClass = php_perl_fetch_object(newObject);

    if (oldClass->sv != NULL) {
        SV* old_sv = SvRV(oldClass->sv);
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
        newClass->sv = sv_bless(newRV_noinc(new_sv), SvSTASH(old_sv));
        SvREFCNT_inc(newClass->sv);
        // Aaaah.... fuck
        zval* newObjZ = malloc(sizeof(zval));
        // zend_clone called us and is just about to stick zend_object inside
        // a zval it already has, which is the one we probably want to return
        // later, but.. i don't know, now we just keep this shite hanging around
        ZVAL_OBJ(newObjZ, newObject);
        Z_ADDREF_P(newObjZ);
        zend_hash_str_add_ptr(&PERL_G(perl_objects), (char*)SvRV(newClass->sv), sizeof(SV*), newObjZ);
    }
    return newObject;
}

/* Destructor for overloaded Perl's objects */
static void php_perl_destructor(zend_object *object)
{
    php_perl_t *perlobj;
    perlobj = php_perl_fetch_object(object);
    DEBUG_PRINT("Freeing %p", perlobj);
    if (perlobj) {
        if (perlobj->properties) {
            /* removing properties */
            zend_hash_destroy(perlobj->properties);
            FREE_HASHTABLE(perlobj->properties);
        }
        PerlInterpreter* my_perl = PERL_G(my_perl);
        // Check SvOK - in certain circumstances we might have caused perl to
        // die and clean everything up already
        if (my_perl != NULL && perlobj->sv != NULL && SvOK(perlobj->sv)) {
            DEBUG_PRINT("Forgetting and freeing %p", perlobj->sv);
            zend_hash_str_del(&PERL_G(perl_objects), (char*)SvRV(perlobj->sv), sizeof(SV*));
            /* removing perl object */
            if (SvREFCNT(perlobj->sv) >= 1)
                SvREFCNT_dec(perlobj->sv);
        }
        efree(perlobj);
    }
}


static void php_perl_iterator_dtor(zend_object_iterator *iterator)
{
    zval_ptr_dtor(&iterator->data);
}

static int php_perl_iterator_valid(zend_object_iterator *iterator)
{
    php_perl_t* perl_obj = Z_PHPPERL(iterator->data);
    if (perl_obj->properties) {
        return zend_hash_has_more_elements(perl_obj->properties);
    }
    return FAILURE;
}

static zval* php_perl_iterator_current_data(zend_object_iterator *iterator)
{
    php_perl_t* perl_obj = Z_PHPPERL(iterator->data);
    if (perl_obj->properties) {
        return zend_hash_get_current_data(perl_obj->properties);
    }
    // todo: do we need to do this? try to think of a test
    zval* ret = malloc(sizeof(zval));
    ZVAL_NULL(ret);
    return ret;
}

static void php_perl_iterator_current_key(zend_object_iterator *iterator, zval *key)
{
    php_perl_t* perl_obj = Z_PHPPERL(iterator->data);
    if (perl_obj->properties) {
        zend_hash_get_current_key_zval(perl_obj->properties, key);
    }
}

static void php_perl_iterator_move_forward(zend_object_iterator *iterator)
{
    php_perl_t* perl_obj = Z_PHPPERL(iterator->data);
    if (perl_obj->properties) {
        zend_hash_move_forward(perl_obj->properties);
    }
}

static void php_perl_iterator_rewind(zend_object_iterator *iterator)
{
    php_perl_t* perl_obj = Z_PHPPERL(iterator->data);
    if (perl_obj->properties) {
        zend_hash_destroy(perl_obj->properties);
        FREE_HASHTABLE(perl_obj->properties);
        perl_obj->properties = NULL;
    }
    php_perl_get_properties(Z_OBJ(iterator->data));

    if (perl_obj->properties)
        zend_hash_internal_pointer_reset(perl_obj->properties);
}

static zend_object_iterator_funcs php_perl_iterator_funcs = {
    php_perl_iterator_dtor,
    php_perl_iterator_valid,
    php_perl_iterator_current_data,
    php_perl_iterator_current_key,
    php_perl_iterator_move_forward,
    php_perl_iterator_rewind
};

static zend_object_iterator* php_perl_get_iterator(zend_class_entry* ce, zval* object, int by_ref)
{
    zend_object_iterator *iterator = emalloc(sizeof(zend_object_iterator));
    zend_iterator_init(iterator);

    // TODO are there spl functions that we can use instead?
    // i.e.: ZVAL_ARR(&iterator->data, php_perl_get_properties())
    ZVAL_OBJ(&iterator->data, Z_OBJ_P(object));
    iterator->funcs = &php_perl_iterator_funcs;

    return iterator;
}

/****************************************************************************/


/** Class method definitions below... */

/**
 * Creates new Perl object
 *
 * Optionally, pass perlClass and constructor to initialise a Perl object
 */
PHP_METHOD(Perl, __construct)
{
    php_perl_t *obj = Z_PHPPERL_P(ZEND_THIS);
    DEBUG_PRINT("(%p) Perl::__construct perlobj %p has context prop %d", ZEND_THIS, obj, obj->context);
    char* perl_class = NULL;
    size_t   perl_class_len;
    char* constructor = "new";
    size_t   constructor_len = 3;
    zval* constructor_args;
    int constructor_args_len = 0;

    ZEND_PARSE_PARAMETERS_START(0, -1);
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(perl_class, perl_class_len)
        Z_PARAM_STRING(constructor, constructor_len)
        Z_PARAM_VARIADIC('+', constructor_args, constructor_args_len)
    ZEND_PARSE_PARAMETERS_END();

    if (ZEND_NUM_ARGS() == 0) {
        obj->sv = NULL;
        obj->properties = NULL;
        return;
    }

    smart_str constructor_func = {0};
    smart_str_appends(&constructor_func, perl_class);
    smart_str_appends(&constructor_func, "::");
    smart_str_appends(&constructor_func, constructor);
    smart_str_0(&constructor_func);
    char* ctor = ZSTR_VAL(smart_str_extract(&constructor_func));

    DEBUG_PRINT("calling perl ctor %s", ctor);

    PerlInterpreter *my_perl = php_perl_init();

    //php_perl_call_method_or_pv(obj, ctor, constructor_args, constructor_args_len, obj->sv);
    // Could we..? return_value is a zval, but we can't have a return value in
    // this function, I don't know if it'd be bad to fuck around with ZEND_THIS

    dSP;                            /* initialize stack pointer         */
    ENTER;                          /* everything created after here    */
    SAVETMPS;                       /* ...is a temporary variable.      */
    PUSHMARK(SP);                   /* remember the stack pointer       */

    XPUSHs(sv_2mortal(newSVpv(perl_class, perl_class_len)));
    for (int i = 0; i < constructor_args_len; i++) {
        SV* el_sv = newSV(0);
        php_perl_zval_to_sv(my_perl, &constructor_args[i], el_sv);
        XPUSHs(sv_2mortal(el_sv));
    }

    PUTBACK;                        /* make local stack pointer global  */
    call_pv(ctor, G_SCALAR | G_EVAL);        /* call the function                */
    SPAGAIN;                        /* refresh stack pointer            */

    if(SvTRUE(ERRSV)) {
        STRLEN na;
        zend_throw_exception_ex(
            perl_exception_ce,
            0,
            "[perl] constructor error: %s",
            SvPV(ERRSV, na)
        );
        return;
    }

    // AFTER checking for errors
    obj->sv = POPs;
    SvREFCNT_inc(obj->sv);

    zval* newzv = malloc(sizeof(zval));
    ZVAL_COPY_VALUE(newzv, ZEND_THIS);
    DEBUG_PRINT("(%p) remembering in ref", ZEND_THIS, newzv);
    zend_hash_str_add_ptr(&PERL_G(perl_objects), (char*)SvRV(obj->sv), sizeof(SV*), newzv);

    PUTBACK;
    FREETMPS;                       /* free that return value           */
    LEAVE;                          /* ...and the XPUSHed "mortal" args.*/
}

PHP_METHOD(Perl, eval)
{
    php_perl_t *obj = Z_PHPPERL_P(ZEND_THIS);
    char *code;
    size_t code_len;

    ZEND_PARSE_PARAMETERS_START(1, 1);
        Z_PARAM_STRING(code, code_len)
    ZEND_PARSE_PARAMETERS_END();

    // If the object is in PERL_ARRAY or PERL_HASH context, that means we expect
    // to be able to do things such as:
    //
    //     $perl->hash->x = $perl->hash->eval("('a', 'b', 'c')");
    //          which is equivalent to:
    //     %x = ('a', 'b', 'c');
    //
    // For that to work, $perl->hash->eval needs to return a zval assoc array
    // which is equivalent of the hash saved to %x by perl. But also ...
    //
    //     $perl->array->y = $perl->array->eval("('a', 'b', 'c')");
    //
    // ... should return a zval index array equivalent to the array saved to @y.
    //
    // There are a few different approaches which could work here. Previously,
    // dmitry's algorithm was just to emulate the behaviour of perl by fetching
    // the list items one by one and adding them directly to a hash or array
    // zval initialised in this function.
    //
    // I don't love that approach because it requires php_perl_sv_to_zval_ref to
    // be a "public" function when I'd prefer to run everything through the main
    // php_perl_sv_to_zval function. Although I couldn't find any, there could
    // also be edge cases where the behaviour differs from Perl and breaks.
    //
    // I consulted irc.perl.org and asked if there was a "correct" way to make
    // an XSUB or something which would end up invoking the same OPs as the perl
    // code I put above, and the answer seemed to be no, but that my hackish
    // plan to wrap everything in a sub, call the sub, assign the return value
    // to an arrayref or hashref, then return that, "might" work as "there's a
    // lot you can get away with in a subroutine".
    //
    // That seemed good enough to try it, and in any case the tests from the
    // previous version of php_ext_perl pass with the below code. And, since I
    // know that none of the code that I intend to run in production actually
    // uses this function and I will be the only guy on earth running this code,
    // I'm happy-ish.
    //
    // Update: call_method_or_pv does do the actual perl thing above, but this
    // also seems to work so it's fine to stay as is for now.
    if (USED_RET() && obj->context != PERL_SCALAR) {
        smart_str eval_code_s = {0};
        smart_str_appends(&eval_code_s, "my $_callPerlSub = sub { ");
        smart_str_appends(&eval_code_s, code);
        if (obj->context == PERL_ARRAY) {
            smart_str_appends(&eval_code_s, " }; my @retval = $_callPerlSub->(); return \\@retval;");
        } else if (obj->context == PERL_HASH) {
            smart_str_appends(&eval_code_s, " }; my %retval = $_callPerlSub->(); return \\%retval;");
        }
        smart_str_appends(&eval_code_s, code);
        smart_str_0(&eval_code_s);
        code = ZSTR_VAL(smart_str_extract(&eval_code_s));
    }

    // Load the existing interpreter object, create a perl string object and put
    // the passed code into that
    PerlInterpreter *my_perl = php_perl_init();
    SV* sv;
    sv = newSVpv(code, strlen(code));

    // Evaluate the code.
    // If we know that the return value will not be used, pass G_DISCARD.
    // Otherwise, we always expect a SCALAR, because if we expect an array or
    // hash, we've wrapped it with code to generate an arrayref or hashref.
    dSP;
    eval_sv(sv, (USED_RET() ? G_SCALAR : G_DISCARD));

    if (SvTRUE(ERRSV)) {
        STRLEN na;
        char* err = SvPV(ERRSV, na);
        err[strcspn(err, "\n")] = 0;
        zend_throw_exception_ex(
            perl_exception_ce,
            0,
            "[perl] eval error: %s", err
        );
    }

    // If the return value should be used, interpret it into a zval...
    if (USED_RET()) {
        SPAGAIN;
        php_perl_sv_to_zval(my_perl, POPs, return_value);
        PUTBACK;
    }
    sv_free(sv);
}

/**
 * Perl->require($file)
 *
 * Loads and executes a Perl file.
 */
PHP_METHOD(Perl, require)
{
    php_perl_t *obj = Z_PHPPERL_P(ZEND_THIS);

    char* file;
    size_t file_len;

    ZEND_PARSE_PARAMETERS_START(1, 1);
        Z_PARAM_STRING(file, file_len)
    ZEND_PARSE_PARAMETERS_END();

    PerlInterpreter* my_perl = php_perl_init();
    require_pv(file);

    if(SvTRUE(ERRSV)) {
        STRLEN na;
        char* err = SvPV(ERRSV, na);
        err[strcspn(err, "\n")] = 0;
        zend_throw_exception_ex(
            perl_exception_ce,
            0,
            "[perl] require error: %s",
            err
        );
    }
}

/** Extension definition below... **/

PHP_MINIT_FUNCTION(perl)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Perl", class_Perl_methods);
    perl_ce = zend_register_internal_class(&ce);
    perl_ce->create_object = _php_perl_new;
    perl_ce->serialize = zend_class_serialize_deny;
    perl_ce->unserialize = zend_class_unserialize_deny;
    perl_ce->get_iterator = php_perl_get_iterator;
    memcpy(&php_perl_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    php_perl_object_handlers.offset = XtOffsetOf(php_perl_t, std);
    php_perl_object_handlers.get_property_ptr_ptr = php_perl_get_property_ptr_ptr;
    php_perl_object_handlers.read_property = php_perl_read_property;
    php_perl_object_handlers.write_property = php_perl_write_property;
    php_perl_object_handlers.get_properties = php_perl_get_properties;
    php_perl_object_handlers.get_method = php_perl_get_method;
    php_perl_object_handlers.do_operation  = php_perl_do_operation;
    php_perl_object_handlers.has_property = php_perl_has_property;
    php_perl_object_handlers.unset_property = php_perl_unset_property;
    php_perl_object_handlers.free_obj  = php_perl_destructor;
    php_perl_object_handlers.clone_obj = php_perl_clone;
    php_perl_object_handlers.read_dimension = php_perl_read_dimension;
    php_perl_object_handlers.write_dimension = php_perl_write_dimension;
    php_perl_object_handlers.unset_property = php_perl_unset_property;
    php_perl_object_handlers.has_dimension = php_perl_has_dimension;
    php_perl_object_handlers.unset_dimension = php_perl_unset_dimension;
    // php_perl_object_handlers.get_constructor = php_perl_get_constructor;
    php_perl_object_handlers.get_class_name = php_perl_get_class_name;

    INIT_CLASS_ENTRY(ce, "PerlException", NULL);
    perl_exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);

    return SUCCESS;
}

PHP_RINIT_FUNCTION(perl)
{
    #if defined(ZTS) && defined(COMPILE_DL_PERL)
        ZEND_TSRMLS_CACHE_UPDATE();
    #endif

    // TODO: I don't think we need to do this actually as the
    // object constructor will do it as and when required
    //php_perl_init();
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(perl)
{
    php_perl_destroy();
    return SUCCESS;
}

PHP_MINFO_FUNCTION(perl)
{
    PerlInterpreter *my_perl = php_perl_init();

    php_info_print_table_start();
    php_info_print_table_header(2, "perl support", "enabled");
    php_info_print_table_row(2, "Extension version", PHP_PERL_VERSION);
    php_info_print_table_row(2, "Perl version", PERL_VERSION_STRING);
    php_info_print_table_end();
}

zend_module_entry perl_module_entry = {
    STANDARD_MODULE_HEADER,
    "perl",                /* Extension name */
    NULL,                  /* zend_function_entry */
    PHP_MINIT(perl),       /* PHP_MINIT - Module initialization */
    NULL,                  /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(perl),       /* PHP_RINIT - Request initialization */
    PHP_RSHUTDOWN(perl),   /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(perl),       /* PHP_MINFO - Module info */
    PHP_PERL_VERSION,      /* Version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PERL
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(perl)
#endif


#endif // HAVE_PERL