--TEST--
Test 61: Modifying object property with += operator
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
    \$self->{'x'} = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$x->x += 2;
$x->x += 2;
var_dump($x->x);
echo "ok\n";
?>
--EXPECT--
int(4)
ok
