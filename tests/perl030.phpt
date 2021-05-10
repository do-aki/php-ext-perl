--TEST--
Test 30: setting hash object's property
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
    \$self->{'a1'} = {"one"=>1,"two"=>2};
    bless \$self, \$type;
    return \$self;
  }
  sub geta1 {
    my \$self = shift;
    return \$self->{'a1'};
  }
  sub geta2 {
    my \$self = shift;
    return \$self->{'a2'};
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->a1 = array("one"=>2,"two"=>1);
$foo->a2 = array("one"=>1,"two"=>2);
$la1 = $foo->geta1();
ksort($la1);
var_dump($la1);
$la2 = $foo->geta2();
ksort($la2);
var_dump($la2);
echo "ok\n";
?>
--EXPECT--
array(2) {
  ["one"]=>
  int(2)
  ["two"]=>
  int(1)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
ok
