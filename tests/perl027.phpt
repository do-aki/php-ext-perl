--TEST--
Test 27: reading object object's property
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Bar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    bless \$self, \$type;
    return \$self;
  }
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'bar'} = Bar->new;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo->bar);
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Bar)#3 (0) {
}
object(Perl::Foo)#2 (1) {
  ["bar"]=>
  object(Perl::Bar)#3 (0) {
  }
}
ok
