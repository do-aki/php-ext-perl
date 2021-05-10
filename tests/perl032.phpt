--TEST--
Test 32: calling objects methods (without arguments)
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
    \$self->{'int'} = 12;
    \$self->{'float'} = 2.5;
    \$self->{'str'} = "str";
    bless \$self, \$type;
    return \$self;
  }

  sub getInt {
    my \$self = shift;
    return \$self->{'int'};
  }

  sub getFloat {
    my \$self = shift;
    return \$self->{'float'};
  }

  sub getStr {
    my \$self = shift;
    return \$self->{'str'};
  }

package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo->getInt());
var_dump($foo->getFloat());
var_dump($foo->getStr());
echo "ok\n";
?>
--EXPECT--
int(12)
float(2.5)
string(3) "str"
ok
