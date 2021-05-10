--TEST--
Test 81: can't serialize
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
    my \$self = [];
    \$self->[0] = 0;
    \$self->[1] = "hello";
    bless \$self, \$type;
    return \$self;
  }
  sub printString {
    my \$self = shift;
    print \$self->[1], "\\n";
  }
  sub printInt {
    my \$self = shift;
    print \$self->[0], "\\n";
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump(serialize($x));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'Perl' is not allowed in %sperl081.php:%d
Stack trace:
#0 %sperl081.php(%d): serialize(Object(Perl::Foo))
#1 {main}
  thrown in %sperl081.php on line %d