--TEST--
Test 14: passing array argument to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  \$var = shift(@_);
}
PERL_END
);
$perl->f(array(1,2.5,"str",array(1)));
var_dump($perl->var);
echo "ok\n";
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  float(2.5)
  [2]=>
  string(3) "str"
  [3]=>
  array(1) {
    [0]=>
    int(1)
  }
}
ok
