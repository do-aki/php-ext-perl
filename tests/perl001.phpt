--TEST--
Test 1: perl_require on existing file
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
perl_require("perl001.pl");
echo "ok\n";
?>
--EXPECT--
ok
ok
