--TEST--
Test 66: Modifying scalar Perl variables
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->x = 123;
$perl->eval('print $x,"\n"');
var_dump(isset($perl->x));
var_dump(empty($perl->x));
var_dump($perl->x);
unset($perl->x);
var_dump(isset($perl->x));
var_dump(empty($perl->x));
echo "ok\n";
?>
--EXPECT--
123
bool(true)
bool(false)
int(123)
bool(false)
bool(true)
ok
