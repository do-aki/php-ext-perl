--TEST--
Test 59: Calling perl subroutine in array context
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?
perl_eval(<<<END
sub f {
  return ("Hello", "World");
}
END
);
$a = perl_call("f");
var_dump($a);
perl_wantarray();
$a = perl_call("f");
var_dump($a);
$a = perl_call("f");
var_dump($a);
perl_wantarray();
$a = perl_call_array("f", array());
var_dump($a);
echo "ok\n";
?>
--EXPECT--
string(5) "World"
array(2) {
  [1]=>
  string(5) "Hello"
  [2]=>
  string(5) "World"
}
string(5) "World"
array(2) {
  [1]=>
  string(5) "Hello"
  [2]=>
  string(5) "World"
}
ok
