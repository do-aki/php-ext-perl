--TEST--
Test 49: creating and using perl objects from external module
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval('use Digest::MD5;');
$ctx = new Perl('Digest::MD5');
$ctx->add("Hello");
echo $ctx->hexdigest()."\n";
echo "ok\n";
?>
--EXPECT--
8b1a9953c4611296a827abf8c47804d7
ok
