--TEST--
Test 70: Catching Perl errors
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
try {
	var_dump($perl->eval('$a = $s{$d}.'));
	echo "ok\n";
} catch (PerlException $exception) {
  echo "Perl error: " . $exception->getMessage() . "\n";
}
?>
--EXPECTF--
Perl error: [perl] eval error: syntax error at (eval %d) line %d, at EOF
