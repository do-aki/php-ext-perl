--TEST--
Test 1: require of existing file
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
$perl = new Perl();
$perl->require(dirname(__FILE__)."/perl001.pl");
echo "ok\n";
?>
--EXPECT--
ok
ok
