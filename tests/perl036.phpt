--TEST--
Test 36: clearing object properies with unset()
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
    \$self->{'p1'} = 1;
    \$self->{'p2'} = 2;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
unset($x->p2);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  ["p1"]=>
  int(1)
}
ok
