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
Fatal error: Uncaught exception 'PerlException' with message '[perl] require error: Can't locate %sperl002.pl in @INC (@INC contains: %s) at (eval %d) line %d.
' in %sperl002.php:%d
Stack trace:
#0 {main}
  thrown in %sperl002.php on line %d
