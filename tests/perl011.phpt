--TEST--
Test 11: accessing undefined perl variables (perl_var)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
var_dump(perl_var('$tmpvar'));
?>
--EXPECTF--
Notice: [perl] Undefined variable: $tmpvar in %s on line %d
NULL
