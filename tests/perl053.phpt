--TEST--
Test 53: PHP's cyclic structures support
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  \$x = shift(@_);
  \$x->[0] = 3;
  return \$x;
}
PERL_END
);

$x = array(1);
$x[1] =& $x;
$x[0] = 2;
var_dump($x);
$y = $perl->f($x);
var_dump($y);
$x[0] = 4;
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    *RECURSION*
  }
}
array(2) {
  [0]=>
  int(3)
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    *RECURSION*
  }
}
array(2) {
  [0]=>
  int(4)
  [1]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    *RECURSION*
  }
}
ok
