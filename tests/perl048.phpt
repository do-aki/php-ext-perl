--TEST--
Test 48: foreach() on array based Perl object
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
    \$self->[0] = 1;
    \$self->[1] = 2;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$i = 0;
foreach($x as $val) {
  var_dump($val);
  if (++$i > 5) break;
}
echo "ok\n";
?>
--EXPECT--
int(1)
int(2)
ok
