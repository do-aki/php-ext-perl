--TEST--
Test 31: setting object object's property
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
    \$self->{'x'} = 1;
    bless \$self, \$type;
    return \$self;
  }
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->bar = $bar =new Perl('Bar');
$bar->x = 2;
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["bar"]=>
  object(Perl::Bar)#3 (1) {
    ["x"]=>
    int(2)
  }
}
ok
