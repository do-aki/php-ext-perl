--TEST--
Test 47: foreach() on Perl object
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
    \$self->{x} = 1;
    \$self->{y} = 2;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$i = 0;
foreach($x as $var => $val) {
  echo "$var = ";
  var_dump($val);
  if (++$i > 5) break;
}
echo "ok\n";
?>
--EXPECT--
y = int(2)
x = int(1)
ok
