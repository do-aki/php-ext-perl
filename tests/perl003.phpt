--TEST--
Test 3: eval
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('print "ok\n";');
echo "ok\n";
?>
--EXPECT--
ok
ok
