--TEST--
Test 31: setting object object's property
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Bar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'x'} = 1;
    bless \$self, \$type;
    return \$self;
  }
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    bless \$self, \$type;
    return \$self;
  }
  sub getbarx {
    my \$self = shift;
    print \$self->{'bar'}->{'x'}, "\n";
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->bar = $bar =new Perl('Bar');
$bar->x = 2;
var_dump($foo);
$foo->getbarx();
echo "ok\n";
?>
--EXPECTF--
object(Perl::Foo)#%d (1) {
  ["bar"]=>
  object(Perl::Bar)#%d (1) {
    ["x"]=>
    int(2)
  }
}
2
ok
