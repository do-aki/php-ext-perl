--TEST--
Test 82: sibling objects
--EXTENSIONS--
perl
--FILE--
<?php
// Occasionally this will pass :-)
$perl = new Perl();
$perl->eval(<<<PERL_END
package Baz;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'foo'} = shift;
    bless \$self, \$type;
    return \$self;
  }
package Bar;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'baz'} = "hello";
    bless \$self, \$type;
    return \$self;
  }
package Foo;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = {};
    \$self->{'foobar'} = Bar->new;
    \$self->{'mybaz'} = Baz->new(\\\$self->{'foobar'});
    \$self->{'mybaz2'} = \\\$self->{'mybaz'};
    bless \$self, \$type;
    return \$self;
  }
package main;
PERL_END
);
$bar = new Perl('Bar');
$x = new Perl('Foo');
$x->foo = $x;
$x->bar = $bar;
$x->baz = $bar;
var_dump($x);
$x->x = 1;
$proxy = &$x->x;
$x->bar->y = $proxy;
var_dump($x);
$x->foo->x = 2;
$x->bar->baz = "goodbye";
$x->mybaz->foo->hi = $x;
$x->foobaz = $x->foobar;
var_dump($x);
$x->foobar->baz = "test";
var_dump($x);
$x->mybaz3 = $x->mybaz2;
$x->mybaz4 = $x->mybaz;
$x->mybaz5 = $x->mybaz3;
var_dump($x);
echo "ok\n";

$hi = $perl->eval(<<<PERL_END
package ArrayBased;
  sub new {
    my \$this = shift;
    my \$type = ref(\$this) || \$this;
    my \$self = [];
    \$self->[0] = 2;
    \$self->[1] = 0;
    bless \$self, \$type;
    return \$self;
  }
package main;
sub makeref
{
  \$to = shift;
  \$from = shift;
  \$to = \\\$from;
}
\$hi = Bar->new;
return \\\$hi;
PERL_END
);

$arrayobj = new Perl("ArrayBased");
$arrayobj[2] = $x;
$x->foobar->myarrayobj = $arrayobj;
$x->foobar->refarrayobj = $hi;
$arrayobj[3] = $arrayobj;
var_dump($arrayobj);
$aor = &$arrayobj;
$perl->hi = $aor;
var_dump($hi);

?>
--EXPECT--
object(Perl::Foo)#3 (6) {
  ["baz"]=>
  object(Perl::Bar)#2 (1) {
    ["baz"]=>
    string(5) "hello"
  }
  ["mybaz"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (1) {
      ["baz"]=>
      string(5) "hello"
    }
  }
  ["foo"]=>
  *RECURSION*
  ["mybaz2"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (1) {
      ["baz"]=>
      string(5) "hello"
    }
  }
  ["bar"]=>
  object(Perl::Bar)#2 (1) {
    ["baz"]=>
    string(5) "hello"
  }
  ["foobar"]=>
  object(Perl::Bar)#5 (1) {
    ["baz"]=>
    string(5) "hello"
  }
}
object(Perl::Foo)#3 (7) {
  ["foo"]=>
  *RECURSION*
  ["foobar"]=>
  object(Perl::Bar)#5 (1) {
    ["baz"]=>
    string(5) "hello"
  }
  ["mybaz2"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (1) {
      ["baz"]=>
      string(5) "hello"
    }
  }
  ["x"]=>
  int(1)
  ["baz"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(5) "hello"
  }
  ["bar"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(5) "hello"
  }
  ["mybaz"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (1) {
      ["baz"]=>
      string(5) "hello"
    }
  }
}
object(Perl::Foo)#3 (8) {
  ["mybaz"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(5) "hello"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["bar"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["x"]=>
  int(2)
  ["foobaz"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(5) "hello"
    ["hi"]=>
    *RECURSION*
  }
  ["baz"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["foobar"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(5) "hello"
    ["hi"]=>
    *RECURSION*
  }
  ["mybaz2"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(5) "hello"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["foo"]=>
  *RECURSION*
}
object(Perl::Foo)#3 (8) {
  ["mybaz"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["bar"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["x"]=>
  int(2)
  ["foobaz"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(4) "test"
    ["hi"]=>
    *RECURSION*
  }
  ["baz"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["foobar"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(4) "test"
    ["hi"]=>
    *RECURSION*
  }
  ["mybaz2"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["foo"]=>
  *RECURSION*
}
object(Perl::Foo)#3 (11) {
  ["foobar"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(4) "test"
    ["hi"]=>
    *RECURSION*
  }
  ["mybaz2"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["foo"]=>
  *RECURSION*
  ["mybaz4"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["x"]=>
  int(2)
  ["foobaz"]=>
  object(Perl::Bar)#5 (2) {
    ["baz"]=>
    string(4) "test"
    ["hi"]=>
    *RECURSION*
  }
  ["baz"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["mybaz5"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["mybaz3"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
  ["bar"]=>
  object(Perl::Bar)#2 (2) {
    ["y"]=>
    int(1)
    ["baz"]=>
    string(7) "goodbye"
  }
  ["mybaz"]=>
  object(Perl::Baz)#4 (1) {
    ["foo"]=>
    object(Perl::Bar)#5 (2) {
      ["baz"]=>
      string(4) "test"
      ["hi"]=>
      *RECURSION*
    }
  }
}
ok
object(Perl::ArrayBased)#9 (4) {
  [0]=>
  int(2)
  [1]=>
  int(0)
  [2]=>
  object(Perl::Foo)#3 (11) {
    ["foobar"]=>
    object(Perl::Bar)#5 (4) {
      ["hi"]=>
      *RECURSION*
      ["myarrayobj"]=>
      *RECURSION*
      ["baz"]=>
      string(4) "test"
      ["refarrayobj"]=>
      object(Perl::Bar)#8 (1) {
        ["baz"]=>
        string(5) "hello"
      }
    }
    ["mybaz2"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        object(Perl::Bar)#8 (1) {
          ["baz"]=>
          string(5) "hello"
        }
      }
    }
    ["foo"]=>
    *RECURSION*
    ["mybaz4"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        object(Perl::Bar)#8 (1) {
          ["baz"]=>
          string(5) "hello"
        }
      }
    }
    ["x"]=>
    int(2)
    ["foobaz"]=>
    object(Perl::Bar)#5 (4) {
      ["hi"]=>
      *RECURSION*
      ["myarrayobj"]=>
      *RECURSION*
      ["baz"]=>
      string(4) "test"
      ["refarrayobj"]=>
      object(Perl::Bar)#8 (1) {
        ["baz"]=>
        string(5) "hello"
      }
    }
    ["baz"]=>
    object(Perl::Bar)#2 (2) {
      ["y"]=>
      int(1)
      ["baz"]=>
      string(7) "goodbye"
    }
    ["mybaz5"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        object(Perl::Bar)#8 (1) {
          ["baz"]=>
          string(5) "hello"
        }
      }
    }
    ["mybaz3"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        object(Perl::Bar)#8 (1) {
          ["baz"]=>
          string(5) "hello"
        }
      }
    }
    ["bar"]=>
    object(Perl::Bar)#2 (2) {
      ["y"]=>
      int(1)
      ["baz"]=>
      string(7) "goodbye"
    }
    ["mybaz"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        object(Perl::Bar)#8 (1) {
          ["baz"]=>
          string(5) "hello"
        }
      }
    }
  }
  [3]=>
  *RECURSION*
}
object(Perl::ArrayBased)#8 (4) {
  [0]=>
  int(2)
  [1]=>
  int(0)
  [2]=>
  object(Perl::Foo)#3 (11) {
    ["foobar"]=>
    object(Perl::Bar)#5 (4) {
      ["hi"]=>
      *RECURSION*
      ["myarrayobj"]=>
      object(Perl::ArrayBased)#9 (4) {
        [0]=>
        int(2)
        [1]=>
        int(0)
        [2]=>
        *RECURSION*
        [3]=>
        *RECURSION*
      }
      ["baz"]=>
      string(4) "test"
      ["refarrayobj"]=>
      *RECURSION*
    }
    ["mybaz2"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        object(Perl::ArrayBased)#9 (4) {
          [0]=>
          int(2)
          [1]=>
          int(0)
          [2]=>
          *RECURSION*
          [3]=>
          *RECURSION*
        }
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
    }
    ["foo"]=>
    *RECURSION*
    ["mybaz4"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        object(Perl::ArrayBased)#9 (4) {
          [0]=>
          int(2)
          [1]=>
          int(0)
          [2]=>
          *RECURSION*
          [3]=>
          *RECURSION*
        }
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
    }
    ["x"]=>
    int(2)
    ["foobaz"]=>
    object(Perl::Bar)#5 (4) {
      ["hi"]=>
      *RECURSION*
      ["myarrayobj"]=>
      object(Perl::ArrayBased)#9 (4) {
        [0]=>
        int(2)
        [1]=>
        int(0)
        [2]=>
        *RECURSION*
        [3]=>
        *RECURSION*
      }
      ["baz"]=>
      string(4) "test"
      ["refarrayobj"]=>
      *RECURSION*
    }
    ["baz"]=>
    object(Perl::Bar)#2 (2) {
      ["y"]=>
      int(1)
      ["baz"]=>
      string(7) "goodbye"
    }
    ["mybaz5"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        object(Perl::ArrayBased)#9 (4) {
          [0]=>
          int(2)
          [1]=>
          int(0)
          [2]=>
          *RECURSION*
          [3]=>
          *RECURSION*
        }
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
    }
    ["mybaz3"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        object(Perl::ArrayBased)#9 (4) {
          [0]=>
          int(2)
          [1]=>
          int(0)
          [2]=>
          *RECURSION*
          [3]=>
          *RECURSION*
        }
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
    }
    ["bar"]=>
    object(Perl::Bar)#2 (2) {
      ["y"]=>
      int(1)
      ["baz"]=>
      string(7) "goodbye"
    }
    ["mybaz"]=>
    object(Perl::Baz)#4 (1) {
      ["foo"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        object(Perl::ArrayBased)#9 (4) {
          [0]=>
          int(2)
          [1]=>
          int(0)
          [2]=>
          *RECURSION*
          [3]=>
          *RECURSION*
        }
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
    }
  }
  [3]=>
  object(Perl::ArrayBased)#9 (4) {
    [0]=>
    int(2)
    [1]=>
    int(0)
    [2]=>
    object(Perl::Foo)#3 (11) {
      ["foobar"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
      ["mybaz2"]=>
      object(Perl::Baz)#4 (1) {
        ["foo"]=>
        object(Perl::Bar)#5 (4) {
          ["hi"]=>
          *RECURSION*
          ["myarrayobj"]=>
          *RECURSION*
          ["baz"]=>
          string(4) "test"
          ["refarrayobj"]=>
          *RECURSION*
        }
      }
      ["foo"]=>
      *RECURSION*
      ["mybaz4"]=>
      object(Perl::Baz)#4 (1) {
        ["foo"]=>
        object(Perl::Bar)#5 (4) {
          ["hi"]=>
          *RECURSION*
          ["myarrayobj"]=>
          *RECURSION*
          ["baz"]=>
          string(4) "test"
          ["refarrayobj"]=>
          *RECURSION*
        }
      }
      ["x"]=>
      int(2)
      ["foobaz"]=>
      object(Perl::Bar)#5 (4) {
        ["hi"]=>
        *RECURSION*
        ["myarrayobj"]=>
        *RECURSION*
        ["baz"]=>
        string(4) "test"
        ["refarrayobj"]=>
        *RECURSION*
      }
      ["baz"]=>
      object(Perl::Bar)#2 (2) {
        ["y"]=>
        int(1)
        ["baz"]=>
        string(7) "goodbye"
      }
      ["mybaz5"]=>
      object(Perl::Baz)#4 (1) {
        ["foo"]=>
        object(Perl::Bar)#5 (4) {
          ["hi"]=>
          *RECURSION*
          ["myarrayobj"]=>
          *RECURSION*
          ["baz"]=>
          string(4) "test"
          ["refarrayobj"]=>
          *RECURSION*
        }
      }
      ["mybaz3"]=>
      object(Perl::Baz)#4 (1) {
        ["foo"]=>
        object(Perl::Bar)#5 (4) {
          ["hi"]=>
          *RECURSION*
          ["myarrayobj"]=>
          *RECURSION*
          ["baz"]=>
          string(4) "test"
          ["refarrayobj"]=>
          *RECURSION*
        }
      }
      ["bar"]=>
      object(Perl::Bar)#2 (2) {
        ["y"]=>
        int(1)
        ["baz"]=>
        string(7) "goodbye"
      }
      ["mybaz"]=>
      object(Perl::Baz)#4 (1) {
        ["foo"]=>
        object(Perl::Bar)#5 (4) {
          ["hi"]=>
          *RECURSION*
          ["myarrayobj"]=>
          *RECURSION*
          ["baz"]=>
          string(4) "test"
          ["refarrayobj"]=>
          *RECURSION*
        }
      }
    }
    [3]=>
    *RECURSION*
  }
}