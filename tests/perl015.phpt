--TEST--
Test 15: passing hash argument to user function
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
perl_call("f", array("a"=>1,"b"=>2.5,"c"=>"str","d"=>array(1)));
var_dump(perl_var('$var'));
perl_call_array("f", array(array("a"=>1,"b"=>2.5,"c"=>"str","d"=>array(1))));
var_dump(perl_var('$var'));
echo "ok\n";
?>
--EXPECT--
array(4) {
  ["c"]=>
  string(3) "str"
  ["a"]=>
  int(1)
  ["b"]=>
  float(2.5)
  ["d"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(4) {
  ["c"]=>
  string(3) "str"
  ["a"]=>
  int(1)
  ["b"]=>
  float(2.5)
  ["d"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
ok