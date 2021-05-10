--TEST--
Test 59: Calling perl subroutine in array context
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  return ("Hello", "World");
}
PERL_END
);
$a = $perl->f();
var_dump($a);
$a = $perl->array->f();
var_dump($a);
echo "ok\n";
?>
--EXPECT--
string(5) "World"
array(2) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(5) "World"
}
ok
