--TEST--
Test 9: accessing array perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('@a = (4, 2.5, "str");');
var_dump($perl->array->a);
echo "ok\n";
?>
--EXPECT--
array(3) {
  [0]=>
  int(4)
  [1]=>
  float(2.5)
  [2]=>
  string(3) "str"
}
ok
