--TEST--
Test 9: accessing array perl variables (perl_var)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval('@a = (4, 2.5, "str");');
var_dump(perl_var('@a'));
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
