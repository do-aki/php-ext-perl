--TEST--
Test 4: returning scalar value from perl_eval()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(perl_eval(''));
var_dump(perl_eval('5+3;'));
var_dump(perl_eval('5.5+3.2;'));
var_dump(perl_eval('reverse "0123456789";'));
echo "ok\n";
?>
--EXPECT--
NULL
int(8)
float(8.7)
string(10) "9876543210"
ok
