--TEST--
Test 37: cloning perl objects
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
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
$y = $x->__clone();
$x->x = 1;
$y->x = 2;
var_dump($x);
var_dump($y);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  ["x"]=>
  int(1)
}
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(2)
}
ok