--TEST--
Test 30: setting hash object's property
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
    \$self->{'a1'} = {"one"=>1,"two"=>2};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->a1 = array("one"=>2,"two"=>1);
$foo->a2 = array("one"=>1,"two"=>2);
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (2) {
  ["a2"]=>
  array(2) {
    ["one"]=>
    int(1)
    ["two"]=>
    int(2)
  }
  ["a1"]=>
  array(2) {
    ["one"]=>
    int(2)
    ["two"]=>
    int(1)
  }
}
ok
