--TEST--
Test 64: Modifying array Perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->array->x = array(1,2,3);
$perl->eval('print @x,"\n"');
var_dump(isset($perl->array->x));
var_dump(empty($perl->array->x));
var_dump($perl->array->x);
unset($perl->array->x);
var_dump(isset($perl->array->x));
var_dump(empty($perl->array->x));
echo "ok\n";
?>
--EXPECT--
123
bool(true)
bool(false)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(false)
bool(true)
ok
