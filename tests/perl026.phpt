--TEST--
Test 26: reading hash object's property
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'x'} = {"int"=>1, "flost"=>2.5, "str"=>"str"};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$x = $foo->x;
ksort($x);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["flost"]=>
  float(2.5)
  ["int"]=>
  int(1)
  ["str"]=>
  string(3) "str"
}
ok
