--TEST--
Test 29: setting array object's property
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
    \$self->{'a1'} = [1,2];
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
$foo->a1 = array(2,1);
$foo->a2 = array(1,2);
var_dump($foo->geta1());
var_dump($foo->geta2());
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
ok
