--TEST--
Test 28: setting scalar object's properties
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<'PERL_END'
package Foo;
  sub new {
    my $this = shift;
    my $type = ref($this) || $this;
    my $self = {};
    $self->{'int'} = 12;
    bless $self, $type;
    return $self;
  }
  sub get_values {
    my $self = shift;
    print "int: ", $self->{'int'}, "\n";
    print "float: ", $self->{'float'}, "\n";
    print "str: ", $self->{'str'}, "\n";
  }
package main;
PERL_END
);
$foo = new Perl('Foo');
$foo->int   = 5;
$foo->float = 2.5;
$foo->str   = "str";
$foo->get_values();
echo "ok\n";
?>
--EXPECT--
int: 5
float: 2.5
str: str
ok
