--TEST--
Test 41: setting elements of array based perl object
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
$x[0] = 3;
$x[2] = "ok";
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (3) {
  [0]=>
  int(3)
  [1]=>
  float(1.5)
  [2]=>
  string(2) "ok"
}
ok
