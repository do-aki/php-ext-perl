--TEST--
Test 44: cloning array based perl objects
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [];
    \$self->[0] = 0;
    bless \$self, \$type;
    return \$self;
  }
  sub modify {
    my \$self = shift;
    \$self->[0] = shift;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$y = clone $x;
$x->modify(1);
$y->modify(2);
var_dump($x);
var_dump($y);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  [0]=>
  int(1)
}
object(Perl::Foo)#2 (1) {
  [0]=>
  int(2)
}
ok
