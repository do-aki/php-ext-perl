--TEST--
Test 78: modifying non existing element of array based object
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
echo "perl:\n";
$x = new Perl('Foo');
var_dump($x[1]);
$x[1]++;
echo "here\n";
++$x[1];
var_dump($x[1]);

echo "ok\n\nphp:\n";
$y = [0];
var_dump($y[1]);
$y[1]++;
echo "here\n";
++$y[1];
var_dump($y[1]);
echo "ok\n";
?>
--EXPECTF--
perl:

Warning: [perl] Undefined array key 1 in %sperl078.php on line 18
NULL

Warning: [perl] Undefined array key 1 in %sperl078.php on line 19
here
int(2)
ok

php:

Warning: Undefined array key 1 in %sperl078.php on line 26
NULL

Warning: Undefined array key 1 in %sperl078.php on line 27
here
int(2)
ok