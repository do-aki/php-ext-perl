--TEST--
Test 55: Perl's cyclic object structures support
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
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$x->foo = $x;
var_dump($x);
$x->x = 1;
var_dump($x);
$x->foo->x = 2;
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["foo"]=>
  *RECURSION*
}
object(Perl::Foo)#2 (2) {
  ["x"]=>
  int(1)
  ["foo"]=>
  *RECURSION*
}
object(Perl::Foo)#2 (2) {
  ["x"]=>
  int(2)
  ["foo"]=>
  *RECURSION*
}
ok
