--TEST--
Test 65: Modifying hash Perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->hash->x = array("a"=>1,"b"=>2);
$perl->eval('print %x,"\n"');
var_dump(isset($perl->hash->x));
var_dump(empty($perl->hash->x));
var_dump($perl->hash->x);
unset($perl->hash->x);
var_dump(isset($perl->hash->x));
var_dump(empty($perl->array->x));
echo "ok\n";
?>
--EXPECT--
a1b2
bool(true)
bool(false)
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
bool(false)
bool(true)
ok
