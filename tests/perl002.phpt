--TEST--
Test 2: require of non existing file
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
$perl = new Perl();
$perl->require(dirname(__FILE__)."/perl002.pl");
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] require error: Can't locate %sperl002.pl in @INC (@INC contains: %s) at (eval %d) line %d.
 in %s on line %d