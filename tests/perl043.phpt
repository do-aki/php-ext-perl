--TEST--
Test 43: clearing clearing elements of array based object with unset()
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
    \$self->[0] = 1;
    \$self->[1] = 2;
    \$self->[3] = 3;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
unset($x[1]);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (2) {
  [0]=>
  int(1)
  [3]=>
  int(3)
}
ok
