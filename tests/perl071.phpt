--TEST--
Test 71: Array proxy test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->array->x = array(1,1,1,1,1);
$perl->array->x[1] = 2;
$perl->array->x[2] += 2;
$x = &$perl->array->x;
$x[3] = 4;
unset($perl->array->x[4]);
var_dump($perl->array->x);
echo "ok\n";
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
ok
