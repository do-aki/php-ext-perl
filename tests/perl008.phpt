--TEST--
Test 8: accessing scalar perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('$a = 5;');
var_dump($perl->a);
$perl->eval('$a = 2.5;');
var_dump($perl->a);
$perl->eval('$a = "str";');
var_dump($perl->a);
echo "ok\n";
?>
--EXPECT--
int(5)
float(2.5)
string(3) "str"
ok
