--TEST--
Test 26: reading hash object's property
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
var_dump($foo->x);
echo "ok\n";
?>
--EXPECT--
array(3) {
  ["str"]=>
  string(3) "str"
  ["int"]=>
  int(1)
  ["flost"]=>
  float(2.5)
}
ok
