--TEST--
Test 29: setting array object's property
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
    \$self->{'a1'} = [1,2];
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->a1 = array(2,1);
$foo->a2 = array(1,2);
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (2) {
  ["a2"]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  ["a1"]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(1)
  }
}
ok
