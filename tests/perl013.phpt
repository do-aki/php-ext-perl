--TEST--
Test 13: passing scalar argument to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  return shift(@_);
}
PERL_END
);
var_dump($perl->f(5));
var_dump($perl->f(2.5));
var_dump($perl->f("str"));
echo "ok\n";
?>
--EXPECT--
int(5)
float(2.5)
string(3) "str"
ok
