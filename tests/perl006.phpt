--TEST--
Test 6: returning hash value from eval()
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$array = $perl->eval('{"int"=>1,"double"=>3.5,"string"=>"string"};');
ksort($array);
var_dump($array);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["double"]=>
  float(3.5)
  ["int"]=>
  int(1)
  ["string"]=>
  string(6) "string"
}
ok
