--TEST--
Test 73: Array/hash property proxy test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (2) {
  ["h"]=>
  array(1) {
    ["a"]=>
    int(2)
  }
  ["a"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
ok
