--TEST--
Test 20: die() in perl_eval()
--EXTENSIONS--
perl
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
Fatal error: Uncaught PerlException: [perl] eval error: Bye bye! at (eval 1) line 1. in %sperl020.php:4
Stack trace:
#0 %sperl020.php(4): Perl->eval('  die("Bye bye!...')
#1 {main}
  thrown in %sperl020.php on line 4