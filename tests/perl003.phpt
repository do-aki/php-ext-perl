--TEST--
Test 3: perl_eval
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval('print "ok\n";');
echo "ok\n";
?>
--EXPECT--
ok
ok
