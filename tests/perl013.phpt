--TEST--
Test 13: passing scalar argument to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
sub f {
  return shift(@_);
}
PERL_END
);
var_dump(perl_call("f", 5));
var_dump(perl_call_array("f", array(5)));
var_dump(perl_call("f", 2.5));
var_dump(perl_call_array("f", array(2.5)));
var_dump(perl_call("f", "str"));
var_dump(perl_call_array("f", array("str")));
echo "ok\n";
?>
--EXPECT--
int(5)
int(5)
float(2.5)
float(2.5)
string(3) "str"
string(3) "str"
ok
