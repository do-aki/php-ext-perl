--TEST--
Test 40: accessing elements of array based perl object
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [];
    \$self->[0] = 0;
    \$self->[1] = 1.5;
    \$self->[2] = "str";
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump($x[0]);
var_dump($x[1]);
var_dump($x[2]);
echo "ok\n";
?>
--EXPECT--
int(0)
float(1.5)
string(3) "str"
ok
