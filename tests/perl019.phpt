--TEST--
Test 19: die() in user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  die("Bye bye!");
}
PERL_END
);

$perl->f();
echo "ok\n";
?>
--EXPECTF--
Fatal error: [perl] call error: 	(in cleanup) Bye bye! at (eval %d) line %d.
 in %s on line %d
