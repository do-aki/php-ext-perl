--TEST--
Test 18: calling internal Perl function (not implemented by pecl/perl)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('print "Hello\n"');
$perl->print("Hello\n");
echo "ok\n";
?>
--EXPECT--
Hello
Hello
ok
