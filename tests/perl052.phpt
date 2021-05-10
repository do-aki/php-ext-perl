--TEST--
Test 52: Perl's cyclic structures support
--EXTENSIONS--
perl
xdebug
--FILE--
<?php
$perl = new Perl();
$x = $perl->eval(<<<PERL_END
  \$x = [1];
  \$x->[1] = \\\$x;
  return \$x;
PERL_END
);
xdebug_debug_zval('x');
$x[0] = 2;
xdebug_debug_zval('x[1]');
xdebug_debug_zval('x[1][1]');
echo "ok\n";
?>
--EXPECT--
x: (refcount=1, is_ref=0)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=...)
x[1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=...)
x[1][1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=...)
ok
