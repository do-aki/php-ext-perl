--TEST--
Test 57: Perl and PHP output buffering
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function xxx($str) {
  return "buf[\n".$str."]\n";
}
ob_start("xxx");

echo "PHP echo 1\n";
perl_eval(<<<END
format STDOUT =
Perl's write
.
write;
END
);
echo "PHP echo 2\n";
perl_eval("print \"Perl's print\n\";");
echo "PHP echo 3\n";
perl_eval("printf \"Perl's printf\n\";");
echo "PHP echo 4\n";
?>
--EXPECT--
buf[
PHP echo 1
Perl's write
PHP echo 2
Perl's print
PHP echo 3
Perl's printf
PHP echo 4
]
