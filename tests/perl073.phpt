--TEST--
Test 73: Array/hash property proxy test
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
    \$self->{'a'} = [1];
    \$self->{'h'} = {'a',1};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->a[0] = 2;
$foo->h['a'] = 2;
var_dump($foo->a);
var_dump($foo->h);
echo "ok\n";
?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(1) {
  ["a"]=>
  int(2)
}
ok