--TEST--
Test 58: Proxy test
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
    \$self->{'a1'} = 1;;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$x = & $foo->a1;
$x = 2;
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  ["a1"]=>
  int(2)
}
ok
