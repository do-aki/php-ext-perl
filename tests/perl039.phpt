--TEST--
Test 39: creating array based perl objects
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
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  [0]=>
  int(0)
}
ok
