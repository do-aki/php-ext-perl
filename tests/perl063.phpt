--TEST--
Test 63: Modifying scalar Perl variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->x = 12;
$perl->x++;
$perl->x -= 2;
var_dump($perl->x);
$perl->eval('print $x,"\n"');
echo "ok\n";
?>
--EXPECT--
int(11)
11
ok
