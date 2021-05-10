--TEST--
Test 69: Method call in scalar/array/hash context
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
    bless \$self, \$type;
    return \$self;
  }
  sub f {
    return ("a","b","c");
  }
package main;
PERL_END
);

$obj = new Perl("Foo");
var_dump($obj->f());
var_dump($obj->array->f());
$hash = $obj->hash->f();
ksort($hash);
var_dump($hash);
echo "ok\n";
?>
--EXPECT--
string(1) "c"
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(2) {
  ["a"]=>
  string(1) "b"
  ["c"]=>
  NULL
}
ok
