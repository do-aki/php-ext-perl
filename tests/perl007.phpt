--TEST--
Test 7: Evaluating invalid Perlcode (eval)
--EXTENSIONS--
perl
--INI--
log_errors_max_len=4096
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('$a = $s{$d}.'));
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught%sPerlException%s[perl] eval error: syntax error at (eval %d) line %d, at EOF%sin %sperl007.php:%d
Stack trace:
#0 %sperl007.php(3): Perl->eval('$a = $s{$d}.')
#1 {main}
  thrown in %sperl007.php on line %d
