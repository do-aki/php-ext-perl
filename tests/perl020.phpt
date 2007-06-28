--TEST--
Test 20: die() in perl_eval()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
  die("Bye bye!");
PERL_END
);
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught exception 'PerlException' with message '[perl] eval error: 	(in cleanup) Bye bye! at (eval %d) line %d.
' in %sperl020.php:%d
Stack trace:
#0 %sperl020.php(6): Perl->eval('%s')
#1 {main}
  thrown in %sperl020.php on line %d
