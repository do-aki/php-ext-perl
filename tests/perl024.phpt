--TEST--
Test 24: reading scalar object's properties
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
    \$self->{'int'} = 12;
    \$self->{'float'} = 2.5;
    \$self->{'str'} = 'str';
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
var_dump($foo->int);
var_dump($foo->float);
var_dump($foo->str);
echo "ok\n";
?>
--EXPECT--
int(12)
float(2.5)
string(3) "str"
ok
