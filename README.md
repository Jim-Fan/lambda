lambda: Interpreter for untyped lambda calculus
===============================================

Summary
-------
Intended to acquire better understanding of shift-reduce parsing
after reading <i>Compilers - Principles, Techniques, and Tools</i>
(authors: Aho, Sethi and Ullman), a parser for lambda calculus is
implemented.

Due to simplicity of lambda calculus' syntax, the parser rule
is indeed simple. Bison's shift-by-default behaviour explains
right-associativity of expression evaluation.

Rudimentary evaluation has been attempted, of which the behaviour
is incomplete, and may even lead to unexpected non-termination during
pretty-printing. See <i>Limitations</i> section below.

Signal handling and readline have once been incorporated. Correct
behaviour could not be trivially achieved, thus the code has been
removed.


Example
-------
```
$ ./lambda
> (/x. /f. f x) (/i.i) 1990;
<syntax tree>
| APP(9017)
  | LAMBDA(9011)
    | VAR(9010, bound to 9010) x
    | LAMBDA(9009)
      | VAR(9008, bound to 9008) f
      | APP(9007)
        | VAR(9005, bound to 9008) f
        | VAR(9006, bound to 9010) x
  | APP(9016)
    | LAMBDA(9014)
      | VAR(9013, bound to 9013) i
      | VAR(9012, bound to 9013) i
    | NUMBER(9015) 1990

<after>
| LAMBDA(9009)
  | VAR(9008, bound to 9008) f
  | APP(9007)
    | VAR(9005, bound to 9008) f
        | NUMBER(9015) 1990

<binding>
var id: 9010    bound to: 9015
var id: 9013    bound to: 9015
```

Note:
* Top-level expression is terminated with ";"
* "/" acts as keyword "lambda" in abstraction
* Unlike traditional lambda notation, evaluation is right-associative
* Interpreter responds by showing:
  1. Syntax tree of top-level expression entered
  2. Syntax tree of expression after evaluation
  3. Value binding of lambda variable to its value, if any
* See /example and /test for more


Limitations
-----------
* If an expression is lambda abstraction, body of lambda is not
  reduced. Example: /x.(/i.i) x evaluates to /x.(/i.i) x instead
  of /x.x
* If an expression is application of form E1 E2, and if E1 is
  free variable, E1 E2 is not evaluated at all. Example:
  (f g 10) evaluates to (f g 10), because f and g are free variable
* Non-termination could arise due to cyclic pprint. For example,
  applying [S combinator](https://en.wikipedia.org/wiki/SKI_combinator_calculus#Recursive_parameter_passing_and_quoting) to itself, see
  [this example](https://github.com/Jim-Fan/lambda/tree/master/example)
  for detail


Building and Dependencies
-------------------------
Development is done in Cygwin environment with:

* GNU Make 4.2.1
* gcc 5.4.0
* GNU bison 3.0.4
* flex 2.6.4
* Exuberant Ctags 5.8   (optional)

To compile, simple run 'make' at shell. Build has been done successfully
on Termux using clang as well. Detailed tool version is not documented
here.


Learned Topics
--------------
* LR(1) parsing
* Shift/reduce and reduce/reduce conflicts:
  1. Why do they arise
  2. How to resolve (if possible)
* Syntax error handling with "error" token of bison


About Testing
-------------
Inspired by flex the lexer generator also on github, maintaining
test code as well as their output could help tracking parser behaviour
and is especially useful when there is change in syntax. Output is
essentially bison trace which needs to be carefully verified before
committing.

See /test and /example for parser and evaluation tests.
