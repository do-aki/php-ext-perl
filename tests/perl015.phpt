--TEST--
Test 15: passing hash argument to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub f {
  \$var = shift(@_);
}
PERL_END
);
$perl->f(array("a"=>1,"b"=>2.5,"c"=>"str","d"=>array(1)));
var_dump($perl->var);
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
ok