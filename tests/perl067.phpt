--TEST--
Test 67: Eval code in scalar/array/hash context
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('("a","b","c")')); // G_SCALAR - expect "c" scalar back
var_dump($perl->array->eval('("a","b","c")')); // G_ARRAY - expect list back, assign to an array.
$h = $perl->hash->eval('("a","b","c")'); // G_ARRAY - expect list back, assign to a hash.
ksort($h);
var_dump($h);
// Also check that doing broken stuff has the same behaviour as perl.
// my @ar = ("my", "list")
// my %h = \@ar;
// Dump(%h);
// SV = PVHV(0x55c2288e5040) at 0x55c22890a9d0
//   REFCNT = 1
//   FLAGS = (SHAREKEYS)
//   ARRAY = 0x55c2289501c0  (0:7, 1:1)
//   hash quality = 100.0%
//   KEYS = 1
//   FILL = 1
//   MAX = 7
//   Elt "ARRAY(0x55c22890a970)" HASH = 0xf442eeeb
//   SV = NULL(0x0) at 0x55c2288df550
//     REFCNT = 1
//     FLAGS = ()
var_dump($perl->hash->eval('{"int"=>1,"double"=>3.5,"string"=>"string"};')); // G_ARRAY, do what perl does which is use the reference received as a hash key.
echo "ok\n";
?>
--EXPECTF--
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
array(1) {
  ["HASH(%s)"]=>
  NULL
}
ok
