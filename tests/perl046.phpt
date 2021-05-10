--TEST--
Test 46: modifying element of array based object
--EXTENSIONS--
perl
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
var_dump($x[0]);
$x[0]++;
var_dump($x[0]);
++$x[0];
var_dump($x[0]);
$x[0]--;
var_dump($x[0]);
--$x[0];
var_dump($x[0]);
echo "ok\n";
?>
--EXPECT--
int(0)
int(1)
int(2)
int(1)
int(0)
ok
