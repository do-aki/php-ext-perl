--TEST--
Test 18: calling internal Perl function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
perl_eval('print "Hello\n"');
perl_call('print', "Hello\n");
perl_call_array('print', array("Hello\n"));
echo "ok\n";
?>
--EXPECT--
Hello
Hello
Hello
ok
