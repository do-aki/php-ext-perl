--TEST--
Test 68: Function call in scalar/array/hash context
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('sub f() {return ("a","b","c");}');
var_dump($perl->f());
var_dump($perl->array->f());
var_dump($perl->hash->f());
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
