--TEST--
Test 3: eval
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval('print "ok from perl\n";');
echo "ok from php\n";
?>
--EXPECT--
ok from perl
ok from php
