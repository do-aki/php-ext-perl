#include <EXTERN.h>               /* from the Perl distribution */
#include <perl.h>                 /* from the Perl distribution */
#include <perliol.h>              /* from the Perl distribution */
#include <perlapi.h>              /* from the Perl distribution */
#undef END_EXTERN_C               /* bypass macros redeclaration warning */
#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include "zend_smart_str.h"
#include "SAPI.h"

#include "php_perl.h"
#include "sv_to_zv.h"
#include "zv_to_sv.h"

static void php_perl_zval_to_sv_noref(
    PerlInterpreter* my_perl,
    zval*            zv,
    SV*              sv,
    HashTable*       var_hash,
    HashTable*       stack
) {
    DEBUG_PRINT("(%p) entering", zv, sv);
    switch (Z_TYPE_P(zv)) {
    case IS_REFERENCE:
        DEBUG_PRINT("(%p) Dereferencing");
        php_perl_zval_to_sv_ref(my_perl, Z_REFVAL_P(zv), sv, var_hash, stack);
        return;
    case IS_NULL:
        DEBUG_PRINT("(%p) NULL->undef", zv);
        sv_set_undef(sv);
        return;
    case IS_LONG:
        DEBUG_PRINT("(%p) long %d -> iv", zv, Z_LVAL_P(zv));
        sv_setiv(sv, Z_LVAL_P(zv));
        return;
    case IS_DOUBLE:
        DEBUG_PRINT("(%p) double -> nv", zv);
        sv_setnv(sv, Z_DVAL_P(zv));
        return;
    case IS_STRING:
        DEBUG_PRINT("(%p) string -> pv", zv);
        sv_setpvn(sv, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
        return;
    case IS_ARRAY: {
        DEBUG_PRINT("(%p) array -> av or hv", zv);
        HashTable* ht = Z_ARR_P(zv);
        int is_hash  = 0;

        // checking if 'hv' is array or hash
        zend_hash_internal_pointer_reset(ht);
        while (1) {
            int key_type = zend_hash_get_current_key_type(ht);
            if (key_type == HASH_KEY_NON_EXISTENT) {
                break;
            }
            if (key_type == HASH_KEY_IS_STRING) {
                is_hash = 1;
                break;
            }
            zend_hash_move_forward(ht);
        }

        if (is_hash) {
            DEBUG_PRINT("(%p) doing hash conv...", zv);
            if (SvTYPE(sv) == SVt_PVAV) {
                DEBUG_PRINT("(%p) Passed an SV %p that already has AV value, but array is associative.", zv, sv);
                zend_error(E_WARNING, "[perl] Array assignment ignored because passed array is associatve. Use a hash instead.");
                return;
            }
            // converting to Perl's hash
            HV* hv;
            if (SvTYPE(sv) == SVt_PVHV) {
                DEBUG_PRINT("(%p) An SV which is already a hash was passed, using that.");
                hv = (HV*)sv;
            } else {
                hv = newHV();
                SV* ret = newRV_inc((SV*) hv);
                SvSetSV(sv, ret);
            }
            zend_hash_str_add_ptr(var_hash, (char*)zv, sizeof(zv), sv);

            zval* el_zv = malloc(sizeof(zval));
            zend_hash_internal_pointer_reset(ht);
            while ((el_zv = zend_hash_get_current_data(ht)) != NULL) {
                zend_string* key;
                long idx;
                SV* el_sv = newSV(0);
                php_perl_zval_to_sv_ref(my_perl, el_zv, el_sv, var_hash, stack);
                // PHP associative arrays can have mixed numeric and string keys
                // so we need to check for numeric keys and convert them to
                // strings.
                if (zend_hash_get_current_key(ht, &key, &idx) == HASH_KEY_IS_STRING) {
                    DEBUG_PRINT("(%p) idx %s", zv, ZSTR_VAL(key));
                    hv_store(hv, ZSTR_VAL(key), ZSTR_LEN(key), el_sv, 0);
                } else {
                    DEBUG_PRINT("(%p) idx %d", zv, idx);
                    char xkey[16];
                    zend_sprintf(xkey, "%ld", idx);
                    hv_store(hv, xkey, strlen(xkey), el_sv, 0);
                }
                zend_hash_move_forward(ht);
            }
            return;
        }
        DEBUG_PRINT("(%p) doing array conv...", zv);

        // converting to Perl's array
        AV* av;
        if (SvTYPE(sv) == SVt_PVAV) {
            DEBUG_PRINT("(%p) passed SV %p already is an array", zv, sv);
            av = (AV*)sv;
        } else {
            DEBUG_PRINT("(%p) passed SV is not an AV, creating a new array", zv, sv);
            av = newAV();
            SV* ret = newRV((SV*)av);
            SvSetSV(sv, ret);
        }
        // Update location in var hash for cyclical refs to be made
        zend_hash_str_add_ptr(var_hash, (char*)zv, sizeof(zv), sv);

        zval* el_zv = malloc(sizeof(zval));
        zend_hash_internal_pointer_reset(ht);
        while ((el_zv = zend_hash_get_current_data(ht)) != NULL) {
            long idx;
            zend_hash_get_current_key(ht, NULL, &idx);
            DEBUG_PRINT("(%p) idx %d", zv, idx);
            SV* el_sv = newSV(0);
            php_perl_zval_to_sv_ref(my_perl, el_zv, el_sv, var_hash, stack);
            av_store(av, idx, el_sv);
            zend_hash_move_forward(ht);
        }
        return;
    }
    case IS_OBJECT: {
        DEBUG_PRINT("(%p) is an object", zv);
        if (!zval_is_perl_obj(zv)) {
            zend_error(E_ERROR, "[perl] Can't pass non perl object to Perl");
            return;
        }
        zend_object* obj = Z_OBJ_P(zv);
        php_perl_t* perlobj = php_perl_fetch_object(obj);
        SV* objsv = perlobj->sv;
        if (objsv == NULL) {
            sv = &PL_sv_undef;
            zend_error(E_NOTICE, "[perl] Can't pass non proxy object to Perl");
            return;
        }
        DEBUG_PRINT("(%p) perlobj zval has SV value %p, putting into %p", zv, objsv, sv);
        SvSetSV(sv, perlobj->sv);
        return;
    }
    case IS_TRUE:
        SvSetSV(sv, &PL_sv_yes);
        return;
    case IS_FALSE:
        SvSetSV(sv, &PL_sv_no);
        return;
    default:
        break;
    }
    zend_error(E_ERROR, "[perl] Can't convert PHP type (%d) to Perl",
                Z_TYPE_P(zv));
    sv = &PL_sv_undef;
    return;
}

static void php_perl_zval_to_sv_ref(
    PerlInterpreter* my_perl,
    zval*            zv,
    SV*              sv,
    HashTable*       var_hash,
    HashTable*       stack
) {
    DEBUG_PRINT("(%p) Starting conversion to SV %p", zv, sv);
    SV* lookup;
    // We check if it's an array or object here because two different zvals with
    // the same contents would have the same hash, maybe?
    if (
        (Z_ISREF_P(zv) || Z_TYPE_P(zv) == IS_OBJECT || Z_TYPE_P(zv) == IS_ARRAY)
        && (lookup = zend_hash_str_find_ptr(var_hash, (char*)zv, sizeof(zv))) != NULL
    ) {
        DEBUG_PRINT("(%p) Was previously seen as %p, incrementing ref count and returning", zv, lookup);
        SV* ret = newRV_inc(lookup);
        SvSetSV(sv, ret);
        return;
    }
    php_perl_zval_to_sv_noref(my_perl, zv, sv, var_hash, stack);

    if ((Z_ISREF_P(zv) || Z_TYPE_P(zv) == IS_OBJECT || Z_TYPE_P(zv) == IS_ARRAY)) {
        DEBUG_PRINT("(%p) updating var_hash", zv, sv);
        zend_hash_str_update_ptr(var_hash, (char*)zv, sizeof(zv), sv);
    }
    DEBUG_PRINT("(%p) Finished conversion to SV %p", zv, sv);
}

/* Converts PHP's value to perl's equivalent */
void php_perl_zval_to_sv(PerlInterpreter* my_perl, zval* zv, SV* sv)
{
    HashTable var_hash;
    HashTable stack;
    zend_hash_init(&var_hash, 0, NULL, NULL, 0);
    zend_hash_init(&stack, 0, NULL, NULL, 0);
    php_perl_zval_to_sv_ref(my_perl, zv, sv, &var_hash, &stack);
    zend_hash_destroy(&var_hash);
    zend_hash_destroy(&stack);
}