--TEST--
Test 35: checking isset() and empty() on object properies
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
    \$self->{'p1'} = 12;
    \$self->{'p2'} = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump(isset($x->p1));
var_dump(isset($x->p2));
var_dump(isset($x->p3));
var_dump(empty($x->p1));
var_dump(empty($x->p2));
var_dump(empty($x->p3));
echo "ok\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
ok
