--TEST--
Test 60: Calling perl method in array context
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
    my \$self = [];
    bless \$self, \$type;
    return \$self;
  }
  
  sub f {
    my \$self = shift;
    if (wantarray) {
      return ("Hello", "World");
    } else {
      return 0;
    }
  }
PERL_END
);
$foo = new Perl("Foo");
$a = $foo->f();
var_dump($a);
$a = $foo->array->f();
var_dump($a);
$a = $foo->f();
var_dump($a);
echo "ok\n";
?>
--EXPECT--
int(0)
array(2) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(5) "World"
}
int(0)
ok
