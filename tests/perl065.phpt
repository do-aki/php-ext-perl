--TEST--
Test 65: Modifying hash Perl variables
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->hash->x = array("a"=>1,"b"=>2);
$perl->eval('print %x,"\n"');
var_dump(isset($perl->hash->x));
var_dump(empty($perl->hash->x));
$x = $perl->hash->x;
ksort($x);
var_dump($x);
unset($perl->hash->x);
var_dump(isset($perl->hash->x));
var_dump(empty($perl->hash->x));
echo "ok\n";
?>
--EXPECTREGEX--
a1b2|b2a1
bool\(true\)
bool\(false\)
array\(2\) {
  \["a"\]=>
  int\(1\)
  \["b"\]=>
  int\(2\)
}
bool\(false\)
bool\(true\)
ok
