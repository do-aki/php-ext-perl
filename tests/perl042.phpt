--TEST--
Test 42: checking isset() and empty() on elements of array based perl object
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [];
    \$self->[0] = 2;
    \$self->[1] = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
var_dump(isset($x[0]));
var_dump(isset($x[1]));
var_dump(isset($x[2]));
var_dump(empty($x[0]));
var_dump(empty($x[1]));
var_dump(empty($x[2]));
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
