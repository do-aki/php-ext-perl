--TEST--
Test 12: calling perl user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f_integer {
  return 5;
}
sub f_double {
  return 2.5;
}
sub f_string {
  return "str";
}
sub f_array {
  my @x=(2);
  return \\@x;
}
sub f_hash {
  my %x=('str'=>'str');
  return \\%x;
}
PERL_END
);
var_dump($perl->f_integer());
var_dump($perl->f_double());
var_dump($perl->f_string());
var_dump($perl->f_array());
var_dump($perl->f_hash());
echo "ok\n";
?>
--EXPECT--
int(5)
float(2.5)
string(3) "str"
array(1) {
  [0]=>
  int(2)
}
array(1) {
  ["str"]=>
  string(3) "str"
}
ok
