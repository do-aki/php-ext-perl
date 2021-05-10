--TEST--
Test 79: modifying existing but undef element of array based object
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
    \$self->[0] = undef;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
echo "perl:\n";
$x = new Perl('Foo');
var_dump($x[0]);
$x[0]++;
echo "here\n";
++$x[0];
var_dump($x[0]);

echo "ok\n\nphp:\n";
$y = [null];
var_dump($y[0]);
$y[0]++;
echo "here\n";
++$y[0];
var_dump($y[0]);
echo "ok\n";
?>
--EXPECT--
perl:
NULL
here
int(2)
ok

php:
NULL
here
int(2)
ok