#ifndef PHP_PERL_SV_TO_ZV_H
#define PHP_PERL_SV_TO_ZV_H

static void php_perl_sv_to_zval_noref(
    PerlInterpreter *my_perl,
    SV              *sv,
    zval            *zv,
    HashTable       *var_hash,
    HashTable       *stack
);

static void php_perl_sv_to_zval_ref (
    PerlInterpreter *my_perl,
    SV              *sv,
    zval            *zv,
    HashTable       *var_hash,
    HashTable       *stack
);

void php_perl_sv_to_zval(PerlInterpreter* my_perl, SV* sv, zval* zv);

#endif /* PHP_PERL_SV_TO_ZV_H */