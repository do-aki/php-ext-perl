--TEST--
Test 19: die() in user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
sub f {
  die("Bye bye!");
}
PERL_END
);

perl_call("f");
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] perl_call error: 	(in cleanup) Bye bye! at (eval %d) line %d.
 in %s on line %d
