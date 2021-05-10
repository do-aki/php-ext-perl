--TEST--
Test 5: returning array value from eval()
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
var_dump($perl->eval('[1,2.5,"string"];'));
echo "ok\n";
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  float(2.5)
  [2]=>
  string(6) "string"
}
ok
