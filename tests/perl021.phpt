--TEST--
Test 21: returning object from perl_eval()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'x'} = 12;
    bless \$self, \$type;
    return \$self;
  }
package Bar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'y'} = 8;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
var_dump(perl_eval('Foo->new()'));
var_dump(perl_eval('Bar::new("Bar")'));
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  ["x"]=>
  int(12)
}
object(Perl::Bar)#1 (1) {
  ["y"]=>
  int(8)
}
ok
