--TEST--
Test 56: Perl's cyclic object structures support
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
sub f {
  my \$x = shift(@_);
  return \$x;
}
PERL_END
);
$x = new Perl('Foo');
$x->foo = $x;
var_dump($x);
$x = $perl->f($x);
var_dump($x);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["foo"]=>
  *RECURSION*
}
object(Perl::Foo)#2 (1) {
  ["foo"]=>
  *RECURSION*
}
ok
