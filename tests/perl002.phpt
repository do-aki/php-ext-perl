--TEST--
Test 2: perl_require on non existing file
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
perl_require("perl002.pl");
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] perl_require error: Can't locate perl002.pl in @INC (@INC contains: %s) at (eval %d) line %d.
 in %s on line %d