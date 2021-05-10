#ifndef PHP_PERL_ZV_TO_SV_H
#define PHP_PERL_ZV_TO_SV_H

static void php_perl_zval_to_sv_noref(
    PerlInterpreter *my_perl,
    zval            *zv,
    SV              *sv,
    HashTable       *var_hash,
    HashTable       *stack
);

static void php_perl_zval_to_sv_ref(
    PerlInterpreter *my_perl,
    zval            *zv,
    SV              *sv,
    HashTable       *var_hash,
    HashTable       *stack
);

void php_perl_zval_to_sv(PerlInterpreter* my_perl, zval* zv, SV* sv);

#endif /* PHP_PERL_ZV_TO_SV_H */