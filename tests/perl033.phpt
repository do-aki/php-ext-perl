--TEST--
Test 33: calling objects methods with arguments
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
    \$self->{'sum'} = 0;
    bless \$self, \$type;
    return \$self;
  }

  sub sum {
    my \$self = shift;
    foreach my \$x (@_) {
      \$self->{'sum'} += \$x;
    }
    return \$self->{'sum'};
  }

package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo->sum(1,2,3,4,5));
var_dump($foo->sum(1,2,3,4,5));
echo "ok\n";
?>
--EXPECT--
int(15)
int(30)
ok
