--TEST--
Test 16: passing many arguments to user function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
sub sum {
  my \$x = shift(@_);
  foreach my \$y (@_) {
    \$x += \$y;
  }
  return \$x;
}

PERL_END
);

var_dump($perl->eval("sum(1,2,3,4,5);"));
var_dump($perl->sum(1,2,3,4,5));
echo "ok\n";
?>
--EXPECT--
int(15)
int(15)
ok
