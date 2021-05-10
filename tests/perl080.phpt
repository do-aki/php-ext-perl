--TEST--
Test 80: unary operations on array based perl object
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
    \$self->[1] = "hello";
    bless \$self, \$type;
    return \$self;
  }
  sub printString {
    my \$self = shift;
    print \$self->[1], "\\n";
  }
  sub printInt {
    my \$self = shift;
    print \$self->[0], "\\n";
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump($x[0]);
$x[0] += 2;
var_dump($x[0]);
$x[0] *= 1.5;
var_dump($x[0]);
$x[0] -= 2;
var_dump($x[0]);

var_dump($x[1]);
$x[1] .= " world";
var_dump($x[1]);

$x[0] .= "9";
var_dump($x[0]);

$x->printString();
$x->printInt();

?>
--EXPECT--
int(0)
int(2)
float(3)
float(1)
string(5) "hello"
string(11) "hello world"
string(2) "19"
hello world
19