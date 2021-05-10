--TEST--
Test 10: accessing hash perl variables
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval('%a = ("int" => 4, "double" => 2.5, "str" => "str");');
$a = $perl->hash->a;
ksort($a);
var_dump($a);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["double"]=>
  float(2.5)
  ["int"]=>
  int(4)
  ["str"]=>
  string(3) "str"
}
ok
