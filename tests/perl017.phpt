--TEST--
Test 17: calling function from external Perl module
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
use Digest::MD5 qw(md5_hex);
PERL_END
);

var_dump($perl->eval('md5_hex("Hello");'));
var_dump($perl->md5_hex('Hello'));
var_dump($perl->{'Digest::MD5::md5_hex'}('Hello'));
echo "ok\n";
?>
--EXPECT--
string(32) "8b1a9953c4611296a827abf8c47804d7"
string(32) "8b1a9953c4611296a827abf8c47804d7"
string(32) "8b1a9953c4611296a827abf8c47804d7"
ok
