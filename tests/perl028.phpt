--TEST--
Test 28: setting scalar object's properties
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
    \$self->{'int'} = 12;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->int   = 5;
$foo->float = 2.5;
$foo->str   = "str";
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (3) {
  ["float"]=>
  float(2.5)
  ["str"]=>
  string(3) "str"
  ["int"]=>
  int(5)
}
ok
