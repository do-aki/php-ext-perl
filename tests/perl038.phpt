--TEST--
Test 38: cloning perl objects
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
    \$self->{'x'} = [0];
    bless \$self, \$type;
    return \$self;
  }
  sub modify {
    my \$self = shift;
    \$self->{'x'}->[0] = shift;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$y = clone $x;
$x->modify(1);
$y->modify(2);
var_dump($x->x);
var_dump($y->x);
echo "ok\n";
?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(2)
}
ok
