/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 44c320fb7d0c579699ef3c2f8769ae1164dce2c1 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Perl___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, perlClass, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructor, IS_STRING, 1, "\"new\"")
	ZEND_ARG_VARIADIC_TYPE_INFO(0, constructorArgs, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Perl_require, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Perl_eval, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Perl, __construct);
ZEND_METHOD(Perl, require);
ZEND_METHOD(Perl, eval);


static const zend_function_entry class_PerlException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Perl_methods[] = {
	ZEND_ME(Perl, __construct, arginfo_class_Perl___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Perl, require, arginfo_class_Perl_require, ZEND_ACC_PUBLIC)
	ZEND_ME(Perl, eval, arginfo_class_Perl_eval, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
