--TEST--
Test 14: passing array argument to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
sub f {
  \$var = shift(@_);
}
PERL_END
);
perl_call("f", array(1,2.5,"str",array(1)));
var_dump(perl_var('$var'));
perl_call_array("f", array(array(1,2.5,"str",array(1))));
var_dump(perl_var('$var'));
echo "ok\n";
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  float(2.5)
  [2]=>
  string(3) "str"
  [3]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  float(2.5)
  [2]=>
  string(3) "str"
  [3]=>
  array(1) {
    [0]=>
    int(1)
  }
}
ok
