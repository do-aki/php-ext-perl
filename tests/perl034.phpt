--TEST--
Test 34: passing object argument to user function
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
package main;
  sub f {
    \$var = shift(@_);
  }
PERL_END
);
perl_call("f", new Perl('Foo'));
var_dump(perl_var('$var'));
perl_call_array("f", array(new Perl('Foo')));
var_dump(perl_var('$var'));
echo "ok\n";
?>
--EXPECT--
object(Perl::Foo)#1 (1) {
  ["x"]=>
  int(12)
}
object(Perl::Foo)#1 (1) {
  ["x"]=>
  int(12)
}
ok
