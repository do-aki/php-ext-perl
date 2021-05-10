--TEST--
Test 47: foreach() on Perl object
--EXTENSIONS--
perl
--FILE--
<?php
$perl = new Perl();
$perl->eval(<<<PERL_END
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{x} = 1;
    \$self->{y} = 2;
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$x = new Perl('Foo');
$i = 0;
$output = [];
foreach($x as $var => $val) {
    ob_start();
    echo "$var = ";
    var_dump($val);
    $output[] = ob_get_clean();
    if (++$i > 5) break;
}
sort($output);
echo implode("", $output);
echo "ok\n";
?>
--EXPECT--
x = int(1)
y = int(2)
ok
