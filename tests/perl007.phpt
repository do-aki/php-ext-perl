--TEST--
Test 7: Evaluating invalid Perlcode (eval)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('$a = $s{$d}.'));
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] eval error: syntax error at (eval %d) line %d, at EOF
 in %s on line %d