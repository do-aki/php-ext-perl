--TEST--
Test 51: PHP's references support
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
$x[1] =& $x[0];
var_dump($x);
$x[0] = 2;
var_dump($x);
$x = $perl->f($x);
var_dump($x);
$x[1] = 4;
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  &int(1)
  [1]=>
  &int(1)
}
array(2) {
  [0]=>
  &int(2)
  [1]=>
  &int(2)
}
array(2) {
  [0]=>
  &int(3)
  [1]=>
  &int(3)
}
array(2) {
  [0]=>
  &int(4)
  [1]=>
  &int(4)
}
ok
