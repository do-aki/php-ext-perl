--TEST--
Test 19: die() in user function
--EXTENSIONS--
perl
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
Fatal error: Uncaught PerlException: [perl] method call error: Bye bye! at (eval 1) line 2.
 in %sperl019.php:10
Stack trace:
#0 %sperl019.php(10): Perl->f()
#1 {main}
  thrown in %sperl019.php on line 10
