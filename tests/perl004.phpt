--TEST--
Test 4: returning scalar value from eval()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval(''));
var_dump($perl->eval('5+3;'));
var_dump($perl->eval('5.5+3.2;'));
var_dump($perl->eval('reverse "0123456789";'));
echo "ok\n";
?>
--EXPECT--
NULL
int(8)
float(8.7)
string(10) "9876543210"
ok
