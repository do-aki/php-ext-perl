--TEST--
Test 46: modifying element of array based object (not implemented by ZE2)
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
$x[0]++;
++$x[0];
var_dump($x[0]);
echo "ok\n";
?>
--EXPECT--
int(2)
ok
