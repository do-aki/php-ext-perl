--TEST--
Test 34: passing object argument to user function
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
  sub f {
    \$var = shift(@_);
  }
PERL_END
);
$perl->f(new Perl('Foo'));
var_dump($perl->var);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(12)
}
ok
