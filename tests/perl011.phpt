--TEST--
Test 11: accessing undefined perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
$perl = new Perl();
var_dump($perl->tmpvar);
?>
--EXPECTF--
Notice: [perl] Undefined variable: '$tmpvar' in %s on line %d
NULL
