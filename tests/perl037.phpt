--TEST--
Test 37: cloning perl objects
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
    \$self->{'x'} = 12;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$y = clone $x;
$x->x = 1;
$y->x = 2;
var_dump($x);
var_dump($y);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(1)
}
object(Perl::Foo)#3 (1) {
  ["x"]=>
  int(2)
}
ok
