--TEST--
Test 77: Perl's cyclic structures support: nested
--EXTENSIONS--
perl
xdebug
--FILE--
<?php
$perl = new Perl();
$y = $perl->eval(<<<'PERL_END'

    $x = [1];
    $x->[1] = \$x;

    $y = [2];
    $y->[1] = \$x;

    return $y;
PERL_END
);
var_dump($y);
xdebug_debug_zval('y');
echo "ok\n";
?>
--EXPECTF--
%sperl077.php:14:
array(2) {
  [0] =>
  int(2)
  [1] =>
  array(2) {
    [0] =>
    int(1)
    [1] =>
    &array
  }
}
y: (refcount=1, is_ref=0)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=0)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=...))
ok
