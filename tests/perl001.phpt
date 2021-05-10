--TEST--
Test 1: require of existing file
--EXTENSIONS--
perl
--FILE--
<?php
error_reporting(E_ALL);
$perl = new Perl();
$perl->require(dirname(__FILE__)."/perl001.pl");
echo "ok from php\n";
?>
--EXPECT--
ok from perl
ok from php
