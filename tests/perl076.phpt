--TEST--
Test 76: Perl's cyclic structures support: mixed array and hash
--EXTENSIONS--
perl
xdebug
--FILE--
<?php
$perl = new Perl();
$y = $perl->eval(<<<'PERL_END'
    $x = [1];
    $x->[1] = \$x;
    $x->[2] = \$x;

    my %y = (
      "myarray" => \$x,
      "string" => "string"
    );
    $y{myhash} = \%y;

    return \%y;
PERL_END
);
ksort($y);
var_dump($y);
xdebug_debug_zval('y');
echo "ok\n";
?>
--EXPECTF--
%sperl076.php:18:
array(3) {
  'myarray' =>
  array(3) {
    [0] =>
    int(1)
    [1] =>
    &array
    [2] =>
    &array
  }
  'myhash' =>
  &array
  'string' =>
  string(6) "string"
}
y: (refcount=1, is_ref=1)=array ('myarray' => (refcount=1, is_ref=0)=array (0 => (refcount=0, is_ref=0)=1, 1 => (refcount=1, is_ref=1)=..., 2 => (refcount=1, is_ref=1)=...), 'myhash' => (refcount=1, is_ref=1)=..., 'string' => (refcount=1, is_ref=0)='string')
ok
