--TEST--
Test 45: modifying property of object
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
    my \$self = {};
    \$self->{x} = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump($x->x);
$x->x++;
var_dump($x->x);
++$x->x;
var_dump($x->x);
--$x->x;
var_dump($x->x);
$x->x--;
var_dump($x->x);
echo "ok\n";
?>
--EXPECT--
int(0)
int(1)
int(2)
int(1)
int(0)
ok
