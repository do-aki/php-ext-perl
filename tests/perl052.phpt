--TEST--
Test 52: Perl's cyclic structures support
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$x = perl_eval(<<<PERL_END
  \$x = [1];
  \$x->[1] = \\\$x;
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
  int(1)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    *RECURSION*
  }
}
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
ok
