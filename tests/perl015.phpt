--TEST--
Test 15: passing hash argument to user function
--EXTENSIONS--
perl
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
$res = $perl->var;
ksort($res);
var_dump($res);
echo "ok\n";
?>
--EXPECT--
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  float(2.5)
  ["c"]=>
  string(3) "str"
  ["d"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
ok