--TEST--
Test 72: Hash proxy test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->hash->x = array('a'=>1,'b'=>1,'c'=>1,'d'=>1,'e'=>1);
$perl->hash->x['b'] = 2;
$perl->hash->x['c'] += 2;
$x = &$perl->hash->x;
$x['d'] = 4;
unset($perl->hash->x['e']);
var_dump($perl->hash->x);
echo "ok\n";
?>
--EXPECT--
array(4) {
  ["c"]=>
  int(3)
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["d"]=>
  int(4)
}
ok
