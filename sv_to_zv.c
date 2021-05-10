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

static void php_perl_sv_to_zval_noref(
    PerlInterpreter *my_perl,
    SV              *sv,
    zval            *zv,
    HashTable       *var_hash,
    HashTable       *stack
) {
    if (!sv)
        return;
    DEBUG_PRINT("(%p) noref enter", sv);
    if (SvTYPE(sv) == SVt_NULL) {    /* null */
        DEBUG_PRINT("(%p) SVt_NULL", sv);
        ZVAL_NULL(zv);
    } else if (SvIOK(sv)) {          /* integer */
        DEBUG_PRINT("(%p) SvIOK", sv);
        ZVAL_LONG(zv, SvIV(sv));
    } else if (SvNOK(sv)) {          /* double */
        DEBUG_PRINT("(%p) SvNOK", sv);
        ZVAL_DOUBLE(zv, SvNV(sv));
    } else if (SvPOK(sv)) {          /* string */
        DEBUG_PRINT("(%p) SvPOK", sv);
        STRLEN len;
        char *str = SvPV(sv, len);
        ZVAL_STRINGL(zv, str, len);
    } else if (sv_isobject(sv)) {    /* object */
        DEBUG_PRINT("(%p) sv_isobject", sv);
        // sv is a RV (reference) to a blessed object. We therefore return a
        // new instance of the Perl class with sv in the struct, so that PHP
        // can directly interact with the object via this extension.

        // If we receive a pointer we already received before, we should return
        // the same zval pointer again so that PHP will detect recursion. To
        // achieve that we store created zval objects in a hash with the sv as
        // the key.
        zval* lookup;
        if ((lookup = zend_hash_str_find_ptr(&PERL_G(perl_objects), (char*)SvRV(sv), sizeof(SV*))) != NULL) {
            DEBUG_PRINT("(%p) Found existing zval %p in lookup", sv, lookup);
            Z_ADDREF_P(lookup);
            ZVAL_NEW_REF(zv, lookup);
        } else {
            DEBUG_PRINT("(%p) Making new object", sv);
            zend_object* new_obj = php_perl_new();
            php_perl_t* new_class = php_perl_fetch_object(new_obj);
            new_class->sv = sv;
            SvREFCNT_inc(sv);
            ZVAL_OBJ(zv, new_obj);
            Z_ADDREF_P(zv);
            zend_hash_str_add_ptr(&PERL_G(perl_objects), (char*)SvRV(sv), sizeof(SV*), zv);
        }
    } else if (SvROK(sv)) {                        /* reference */
        DEBUG_PRINT("(%p) dereferencing type %ld", sv, SvTYPE(sv));
        php_perl_sv_to_zval_ref(my_perl, SvRV(sv), zv, var_hash, stack);
    } else if (SvTYPE(sv) == SVt_PVAV) {           /* array */
        I32 i   = 0;
        I32 len = av_len((AV*)sv);

        array_init(zv);
        DEBUG_PRINT("(%p) array", sv);
        zend_hash_str_update_ptr(stack, (char*)sv, sizeof(SV*), zv);
        for (i = 0; i <= len; i++) {
            SV** el_sv = av_fetch((AV*)sv, i, 0);
            if (el_sv != NULL && *el_sv != NULL) {
                DEBUG_PRINT("(%p->%p) doing el %d", sv, *el_sv, i);
                // We recursively call this function here to resolve the child
                // SV into a new zval.
                zval* el_zv = malloc(sizeof(zval));
                php_perl_sv_to_zval_ref(my_perl, *el_sv, el_zv, var_hash, stack);
                zval* newloc = zend_hash_index_update(Z_ARRVAL_P(zv), i, el_zv);
                // zend_hash_index_update does a copy rather than a reference,
                // so we have to update var_hash with the location of the zv
                // we'll want to edit later if converting to a reference.
                zend_hash_str_update_ptr(var_hash, (char*)*el_sv, sizeof(SV*), newloc);
                DEBUG_PRINT("(%p->%p) updated zv ref to %p", sv, el_sv, newloc);
            }
        }
        zend_hash_str_del(stack, (char*)sv, sizeof(SV*));
    } else if (SvTYPE(sv) == SVt_PVHV) {           /* hash */
        SV*   el_sv;
        char* key;
        I32   key_len;

        array_init(zv);
        hv_iterinit((HV*)sv);
        DEBUG_PRINT("(%p) sv is hash", sv);
        zend_hash_str_update_ptr(stack, (char*)sv, sizeof(SV*), zv);
        while ((el_sv = hv_iternextsv((HV*)sv, &key, &key_len)) != NULL) {
            // The same considerations as the array section above apply here.
            zval* el_zv = malloc(sizeof(zval));
            DEBUG_PRINT("(%p->%p) doing element %s", sv, el_sv, key);
            php_perl_sv_to_zval_ref(my_perl, el_sv, el_zv, var_hash, stack);
            zval* newloc = zend_hash_str_update(Z_ARRVAL_P(zv), key, key_len, el_zv);
            zend_hash_str_update_ptr(var_hash, (char*)el_sv, sizeof(SV*), newloc);
            DEBUG_PRINT("(%p->%p) updated zv ref to %p", sv, el_sv, newloc);
        }
        zend_hash_str_del(stack, (char*)sv, sizeof(SV*));
    } else if (SvTYPE(sv) == SVt_PVIV && !SvOK(sv)) { /* pointer to undef? */
        ZVAL_NULL(zv);
    } else {
        zend_error(E_ERROR, "[perl] Can't convert Perl type (%ld) to PHP", SvTYPE(sv));
    }
}

static void php_perl_sv_to_zval_ref(
    PerlInterpreter *my_perl,
    SV              *sv,
    zval            *zv,
    HashTable       *var_hash,
    HashTable       *stack
) {
    DEBUG_PRINT("(%p) converting to zv %p", sv, zv);

    // we're using the "stack" hashmap to identify recursion - when the SV we
    // are currently converting is nested into an SV that is currently on the
    // recursion "stack", we want to set zv to a reference of initial.
    zval* nested = zend_hash_str_find_ptr(stack, (char*)sv, sizeof(SV*));
    if (nested != NULL) {
        DEBUG_PRINT("(%p) sv is nested. parent zv is %p. doing new ref.", sv, nested);
        ZVAL_NEW_REF(zv, nested);
        return;
    }
    // On the other hand, if the SV has multiple references, that may be to a
    // sibling element rather than a parent element, which we handle slightly
    // differently. (We ignore SV with ROK flag because we always want to deref)
    if (SvREFCNT(sv) > 1 && !SvROK(sv)) {
        // First we should check whether we have already seen it and convert
        // both the current result and the previous result to references. Note
        // that I've copied from the previous algorithm, which stores pointer
        // pointers in var_hash. Although they aren't a concept in the zend
        // engine anymore, we edit the memory of the allocated array below, so
        // since the non _ptr version of the hash functions would copy the zval
        // rather than save a reference, we would be updating the wrong thing.
        zval* initial = zend_hash_str_find_ptr(var_hash, (char*)sv, sizeof(SV*));
        if (initial != NULL && Z_TYPE_P(initial) != IS_OBJECT) {
            DEBUG_PRINT("(%p) ref>1 seen previously. initial zval is %p. doing copy.", sv, initial);
            if (!Z_ISREF_P(initial)) {
                ZVAL_NEW_REF(initial, initial);
            }
            ZVAL_COPY(zv, initial);
            return;
        } else if (initial != NULL && Z_TYPE_P(initial) == IS_OBJECT) {
            // The original code had this handled like this but I'm not really
            // sure how to reach it. I tried moderately hard - see perl082.phpt
            zend_error(E_ERROR, "[perl] unexpected value");
            ZVAL_NEW_REF(zv, initial);
            return;
        }
        // If this is the first time we've seen this SV, add it to the lookup
        // table and continue resolving.
        DEBUG_PRINT("(%p) ref>1 not seen previously %p", sv, initial);
        zend_hash_str_update_ptr(var_hash, (char*)sv, sizeof(SV*), zv);
    }

    php_perl_sv_to_zval_noref(my_perl, sv, zv, var_hash, stack);
    DEBUG_PRINT("(%p) converted to %p", sv, zv);
}

/* Converts Perl's value to PHP's equivalent */
void php_perl_sv_to_zval(PerlInterpreter* my_perl, SV* sv, zval* zv)
{
    DEBUG_PRINT("(%p) outer conversion to zval %p", sv, zv);
    HashTable var_hash;
    HashTable stack;
    zend_hash_init(&var_hash, 0, NULL, NULL, 0);
    zend_hash_init(&stack, 0, NULL, NULL, 0);
    php_perl_sv_to_zval_ref(my_perl, sv, zv, &var_hash, &stack);
    zend_hash_destroy(&var_hash);
    zend_hash_destroy(&stack);
    DEBUG_PRINT("(%p) outer finished conversion to zval %p", sv, zv);
}