--TEST--
Test 50: Perl's references support
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$x = $perl->eval(<<<PERL_END
  \$x = [1];
  \$x->[1] = \\\$x->[0];
  return \$x;
PERL_END
);
var_dump($x);
$x[0] = 2;
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
ok
