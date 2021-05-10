--TEST--
Test 75: Perl's cyclic structures support: multiple references
--EXTENSIONS--
perl
xdebug
--FILE--
<?php
$perl = new Perl();
$x = $perl->eval(<<<PERL_END
  \$x = [1];
  \$x->[1] = \\\$x;
  \$x->[2] = \\\$x;
  return \$x;
PERL_END
);
xdebug_debug_zval('x');
xdebug_debug_zval('x[1]');
xdebug_debug_zval('x[1][1]');
xdebug_debug_zval('x[1][2]');
xdebug_debug_zval('x[2]');
xdebug_debug_zval('x[2][1]');
xdebug_debug_zval('x[2][2]');
$x[0] = 2;
xdebug_debug_zval('x[1]');
xdebug_debug_zval('x[1][1]');
xdebug_debug_zval('x[1][2]');
xdebug_debug_zval('x[2]');
xdebug_debug_zval('x[2][1]');
xdebug_debug_zval('x[2][2]');
echo "ok\n";
?>
--EXPECTF--
x: (refcount=1, is_ref=0)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1][1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1][2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2][1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2][2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1][1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[1][2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2][1]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
x[2][2]: (refcount=1, is_ref=1)=array (0 => (refcount=0, is_ref=0)=2, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...)
ok