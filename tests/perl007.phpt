--TEST--
Test 7: Evaluating invalid Perlcode (perl_eval)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(perl_eval('$a = $s{$d}.'));
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] perl_eval error: syntax error at (eval %d) line %d, at EOF
 in %s on line %d