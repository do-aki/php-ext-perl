--TEST--
Test 22: returning object from perl function
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
    \$self->{'x'} = 12;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
var_dump($perl->{'Foo::new'}('Foo'));
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(12)
}
ok
