--TEST--
Test 62: Modifying object element with += operator
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
    my \$self = [];
    \$self->[0] = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$x[0] += 2;
$x[0] += 2;
var_dump($x[0]);
echo "ok\n";
?>
--EXPECT--
int(4)
ok
