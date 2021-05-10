--TEST--
Test 25: reading array object's property
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
    \$self->{'x'} = [1, 2.5, "str"];
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo->x);
echo "ok\n";
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  float(2.5)
  [2]=>
  string(3) "str"
}
ok
