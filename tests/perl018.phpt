--TEST--
Test 18: calling internal Perl function
--EXTENSIONS--
perl
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
--XFAIL--
Was never implemented