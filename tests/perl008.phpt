--TEST--
Test 8: accessing scalar perl variables (perl_var)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval('$a = 5;');
var_dump(perl_var('$a'));
perl_eval('$a = 2.5;');
var_dump(perl_var('$a'));
perl_eval('$a = "str";');
var_dump(perl_var('$a'));
echo "ok\n";
?>
--EXPECT--
int(5)
float(2.5)
string(3) "str"
ok
