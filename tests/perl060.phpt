--TEST--
Test 60: Calling perl method in array context
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval(<<<END
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
END
);
$foo = new Perl("Foo");
$a = $foo->f();
var_dump($a);
perl_wantarray();
$a = $foo->f();
var_dump($a);
$a = $foo->f();
var_dump($a);
echo "ok\n";
?>
--EXPECT--
int(0)
array(2) {
  [1]=>
  string(5) "Hello"
  [2]=>
  string(5) "World"
}
int(0)
ok
