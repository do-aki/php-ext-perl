--TEST--
Test 67: Eval code in scalar/array/hash context
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('("a","b","c")'));
var_dump($perl->array->eval('("a","b","c")'));
var_dump($perl->hash->eval('("a","b","c")'));
echo "ok\n";
?>
--EXPECT--
string(1) "c"
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(2) {
  ["a"]=>
  string(1) "b"
  ["c"]=>
  NULL
}
ok
