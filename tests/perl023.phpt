--TEST--
Test 23: creating new Perl objects
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
    \$self->{'x'} = 11;
    bless \$self, \$type;
    return \$self;
  }
  sub new2 {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'x'} = 12;
    bless \$self, \$type;
    return \$self;
  }
  sub new3 {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'x'} = shift;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo);
$foo = new Perl('Foo','new2');
var_dump($foo);
$foo = new Perl('Foo','new3',13);
var_dump($foo);
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(11)
}
object(Perl::Foo)#3 (1) {
  ["x"]=>
  int(12)
}
object(Perl::Foo)#2 (1) {
  ["x"]=>
  int(13)
}
ok
