#########
Reference
#########

This chapter describes the Scaly Programming Language.
The Scaly Programming Language is defined by the structure and semantics of the
:ref:`statements <statements>` which can be used to write a Scaly
:ref:`program <program>`.

On the lowest level, a Scaly program consists of a sequence of characters which
are read from :ref:`files <files>`. The :ref:`lexical structure <lexical>`
of the Scaly Programming Language describes how the characters of the
program code are combined to form a sequence of :ref:`tokens <tokens>`
and :ref:`whitespaces <whitespace>`.

The structure of the :ref:`statements <statements>` is defined by the
:ref:`grammar <grammar>` of the Scaly Programming Language.

The documentation you are reading sometimes refers to the reference
implementation of the Scaly Programming Language, but an alternative
implementation can choose to do things in another way, or to provide
additional functionality like a command line playground which the
reference implementation does not have.

Particularly, the :ref:`standard library <standardlibrary>` which comes with
the reference implementation is not part of the Scaly Programming Language
in a strict sense. Nevertheless, some elements like basic types, operators,
and functions of this standard library like basic types are used in the
code samples that are provided.

.. _lexical:

*****************
Lexical structure
*****************

On the lowest level, a Scaly program is made up of a sequence of *characters*
which are read from one or more files. For the Scaly Programming Language,
some characters have a special meaning, which means that they control the
forming of the conversion of the character sequence into a sequence of
*:ref:`tokens`* and *:ref:`whitespace`*. These tokens are then parsed into
expressions which make up a Scaly program or library.

.. _whitespace:

Whitespaces
===========

Whitespaces are sequences of characters which have no meaning by themselves,
apart from being used for separating :ref:`tokens <tokens>`
if no :ref:`punctuation <punctuationandkeywords>` can be used.

Whitespace character sequences
==============================

Outside of comments or strings, the following characters form
whitespace character sequences:

* space,

* tab,

* line break, and

* line feed.

Single line comments
====================

Two forward slashes start a single line comment which spans all following
characters up to and including a line break::

  // This is a single line comment.

Multiple line comments
======================

A forward slash followed immediately by an asterisk starts a multiple line
comment which spans all following characters up to and including an asterisk
and a forward slash immediately following::

  /* This is a multi-line comment.
  Continued comment text
  The comment ends now:*/

Multiple line comments can be nested:

  /* This comment is /*nested*/.*/

.. _tokens:

Tokens
======

`Tokens` form the building blocks of expressions. The syntactic grammar of the
Scaly Programming Language is expressed in terms of tokens. Tokens can be

* :ref:`literals <literals>`,
* :ref:`punctuation <punctuationandkeywords>`,
* :ref:`keywords <punctuationandkeywords>`, and
* :ref:`identifiers <identifiers>`.

.. _literals:

Literals
========

Literals are used to write constant values directly into the program.

A numeric literal starts with a digit, followed by zero or more digits,
optionally one decimal point, optional fraction digits, an optional exponent
consisting of the exponent character (`E` or `e`) and exponent digits.

If the first two characters are `0x`, the literal is a hexadecimal literal.
The digits that may follow may include the characters `a`, `b`, `c`, `d`, `e`,
`f`, `A`, `B`, `C`, `D`, `E`, and `F`. No decimal point or exponent is allowed
for hexadecimal literals.

Some lexically valid numeric literals:

  42
  1.
  0.22e4567
  1E6
  0xFB04
  0x123abc

(A minus sign is not part of a number literal. It is typically implemented as
a unary operator.)

String literals start with a double quote and end with a double quote:

  "This is a string"

All white space characters can be directly embedded in strings:

  "A string with a
  line break"

Tabs, carriage returns, line feeds, and NUL characters can be escaped by a
back slash and the characters `t`, `r`, `n`, `0` respectively. The back slash
escapes itself, and the double quote is escaped by a back slash as well.

  "A line feed\n, an \"escaped\" string, an escaped \\ backslash,
  a \ttab, and a carriage \rreturn."

Character literals start with a single quote, continue with the
character whose value is to be written, and end with a single quote:

  'a'
  '0' // The zero digit character

The characters that can be escaped in string literals, and the single quote
must be escaped in a character literal.

  '\"'
  '\''
  '\0' // The NUL character

.. _punctuationandkeywords:

Punctuation and Keywords
========================

Punctuation characters are used (alongside keywords) for building the structure
of expressions.

Keywords are used (alongside punctuation characters) for building the structure
of expressions.

The complete list of punctuation characters and keywords is contained in the
:ref:`grammar reference <scalygrammar>`.

.. _identifiers:

Identifiers
===========

All character combinations which are not white space, literals, punctuation,
or keywords, are identifiers. Some examples:

  WindyShore
  foo*bar
  baz

Identifiers are used as names.

.. _operatortokens:

Operator
========

All combinations of operator characters are operators. The operator characters
are `+`, `-`, `*`, `/`, `=`, `%`, `&`, `|`, `^`, `~`, `<`, and `>`.
Some examples::

  +
  **
  <
  `

Operators are used as names like :ref:`identifiers,<identifiers>`.

******************
Language Structure
******************

.. _program:

The Program
===========

The Scaly compiler processes a *program* which contains all code which is to be
compiled in a single compiler run to an executable program, a library, or code
which is compiled just-in-time (JITted). A program consists of zero or more
:ref:`files <files>`.

.. _files:

Files
=====

A Scaly :ref:`program <program>` consists of one or more :ref:`files <files>`
which contain the program code. The Scaly compiler compiles a Scaly program
to a piece of executable code. How this code can be executed depends
on the implementation of the Scaly compiler.

The reference implementation of the Scaly Programming Language, which the
documentation you are reading is part of, compiles a Scaly
program into an `LLVM assembly language <https://llvm.org/docs/LangRef.html>`_
module which can be processed further using the
`LLVM tools <https://llvm.org/docs/CommandGuide/index.html>`_.

A file is an object from which a Scaly compiler can read a sequence of
characters and parse them into :ref:`statements <statements>`.
A file must contain zero or more complete statements.

.. _statements:

Statements
==========

A Scaly program consists of *statements*. Statements are the building blocks
of a Scaly program. A statement can perform computation work and either
return the result of the computation to the enclosing expression or bind it
to an identifier which can be referred to in the current scope.

.. _expressions:

Expressions
===========

An expression performs actual computation work and usually returns a value
as a result of that work. There are numerous forms of expressions in Scaly
like *literals*, *function calls*, *operations*, and many more.

Expressions can be optionally terminated by a semicolon. Line breaks are not
significant for expression termination.

.. _literalvalues:

Literal Values
==============

The most basic expressions in Scaly are *literal values*. A literal value
expression evaluates to the value that is written literally into a program.

  1.602E-19
  "baz"
  'a'

There is no such thing as a boolean literal as a part of the language.
Boolean constants can be defined by a runtime library.

.. _objectexpressions:

Object expressions
==================

An object expression is an expression which combines zero or more expressions,
the so-called components, to an object. An object is a combination of data
which are used together::

  () // The empty object which contains no components.
  (42,"The Answer") // Contains the number 42 and a string
  (()) // A non-empty object which contains the empty object.

An object consisting of a single component is semantially equivalent to the
component contained by that object::

  (5) // => 5

The components of an object can be accessed by its index, starting with 1
at its first component. The component index must be known at compile time,
it cannot be computed::

  (7).1 // => 7
  (1, (2, (3, (4)))).2.2.2 // => 4

The components of an object can be given a name which can be used to
access them::

  (brand = "IFA", model = "F9", year = 1952).year // => 1952


.. _arraysliterals:

Array literals
==============

An array literal combines zero or more expressions of the same type::

  [2, 3, 5, 7] // An array with four components

A component of an array can be accessed by appending brackets which contain the
index. The index is zero-based::

  [1, 2][1] // => 2

.. _declarations:

Declarations
============

An declaration evaluates an expression and binds the value which was returned
by that expression to a constant or variable::

  let a = 2
  // b cannot be used here
  let b = 3
  a // 2
  var c = b
  b // 3

The constant or variable can be used in every expression which follows its
declaration in the current scope. A scope is either the global scope
or the scope of a :ref:`blocks`.

.. _blocks:


***********
Code Blocks
***********

In all but the simplest programs, you structure your code using blocks.
Like in commonplace languages of C descent, a block starts with a
left curly brace, followed by zero or more statements, and ends with a
right curly brace. An example::

  {
      let a: int = 2
      let b: int = 3
      a + b
  }

But there are important differences to C and friends:

First, *code blocks are expressions* - they may return a value.
Because of this, if the last statement of a code block is an expression,
the value which is returned by that statement is the value which the
code block returns. Obviously, our example block returns an ``int``
whose value is ``5``, and therefore is a valid Scaly program.

Second, the statements of a block can be *executed in any order*
as far as data dependencies and the pureness of called functions allow.
That is the main selling point of Scaly - it automatically schedules
parallel and even distributed computation wherever possible.

Blocks and scopes
=================

A block is an expression which combines zero or more expressions in a
local scope. The last expression of the block is returned::

  { 99 } // => 99

A scope gives a block a name::

  scope A {
    let b = 2
  }

From a scope, a constant can be used after the scope declaration::

  A.b // => 2

.. _functionexpressions:

Function Expressions
====================

A function expression evaluates to a function value.
It consists of the `function` keyword, an object literal, and a block::

  function (a) { a } // => function(a){a}

.. _functioncalls:

Function calls
==============

A function (an expression which evaluates to a function) can be called
by combining it with an object to be used as an input to the function::

  let getItself = function (a) { a };
  getItself(2) // => 2

.. _operatorliterals:

Operator Expressions
====================

For the Scaly programming language, an operator is a function which receives
two objects as input. An operator expression consists of the
`operator` keyword, two object literals, and a block::

  operator (a) (b) { (a, b) } // => operator(a)(b){(a,b)}

Scaly knows no binary operator precedence, execution is left to right::

  2 + 3 * 4 // => 20

Operator precedence can be done by putting binary operation into parentheses
(which are technically :ref:`objectexpressions` with one component which simply
expose the containing operation after evaluation)::

  2 + (3 * 4) // => 14

.. _operatorcalls:

Operator Calls
==============

An operator can be called by combining an object expression with the operator
and a second object expression. The following example declares the
`><` operator which combines two expressions to an object::

  let >< = operator (a) (b) { (a,b) }
  2 >< 3 // => (2,3)

.. _arrayops:

Array Operators
===============

A special variant of the operator expression is one that combines an
array literal with a block. It can combine multiple operads using
one operation call::

  let with = operator [a] { (a[0], a[1], a[2], a[3]) }
  1 with 2 with 3 with 4 with 5 // => (1,2,3,4,5)

The order of evaluation of the operator arguments is not specified.

.. _bindingexpressions:

Binding and Assignment Expressions
==================================

Binding expressions bind an expression to a pattern. A pattern is commonly an
identifier expression which is a name of the object to which the expression
is bound. Binding expressions can be constant (using `let`),
variable (using `mutable`), or inferred (using `var`)::

  var a 2
  var b 3
  var c a + b
  c // => 5

Writing an equals sign after let, mutable, or var is not required by the
language, but since the standard library provides the unary = operator,
the binding expressions can use them. Together with optional semicolons,
the code might be more readable::

  var a = 2;
  var b = 3;
  var c = a + b;
  c // => 5

An object bound to a `mutable` name can be altered by setting it to a new value
using the assignment expression::

  mutable d 6
  set d d + 1
  d // => 7

.. _functiondeclarations:

Function and Operator Declarations
==================================

Instead of declaring a function or using `let`, a shorter and more syntax
can be used::

  function getItself(a) { a }
  operator >< (a)(b) { (a, b) }
  operator with [a] { (a[0], a[1], a[2], a[3]) }

.. _externalfunctions:

External function declarations
==============================

External functions are functions that are provided by the runtime environment.
If external functions are to be made accessible from a Scaly program,
they must be declared using the following syntax::

  external _fopen(filename: pointer, mode: pointer): pointer
  external _fclose(file: pointer)

.. _intrinsicfunctions:

Intrinsic function declarations
===============================

Intrinsic functions are functions that are provided by the compiler
infrastructure. They must be declared using the following syntax::

  intrinsic function llvm.sin.f64(value: double): double

.. _typesystem:

The Scaly Type System
=====================

Types describe kinds of data which can be processed by a Scaly program.
Most importantly, a type determines which values a variable or constant
can have, and the compiler chooses a representation of the data on the hardware
on which the code of a Scaly program runs. There are primitive data types
like the void object, functions, and operators, integral and floating point
numbers, enumerations, bit masks, characters, pointers,and complex data types
like objects, arrays, and variants which combine primitive data types.

.. _void:

The Void data type
==================

The void data type has no value. It can be used as an object that has no data,
and it can be written as an empty :ref:`objectexpressions`::

  ()

They are used as empty input or output of :ref:`functions <functions>`
or :ref:`operators` or as an option of variant data types.

.. _functions:

Functions
=========

Functions are objects that contain executable code which can be called, and
which receive one input object (which can be :ref:`void <void>`) and return
one output object (which can be void as well). The type of a function is
uniquely identified by its signature, i.e., the types of its input and output.
Two functions which have the same signature are of the same type.

.. _operators:

Operators
=========

Operators are objects that contain executable code which can be called, and
which receive two input objects (which can be :ref:`void <void>`) and return
one output object (which can be void as well). The type of an operator is
uniquely identified by its signature, i.e., the types of its two input objects
and its output object. Two operators which have the same signature are of the
same type.

.. _pointers:

Pointers
========

The `pointer` type represents an address in the address space of the machine.
It can point to any kind of data, and the type or semantics of the data
are not defined. A pointer can be converted to any data item without any
runtime or compile time checks. The author of the code is responsible
to guarantee that any instance of a pointer which is used carrys a valid
address, and that the pointer is only converted to a constant or variable
of the type of data which lives at that address.

Pointers should only be used when external functions are called by Scaly code
or by Scaly functions that are called from external code.

.. _inttypes:

Integer types
=============

The :ref:`standard library <standardlibrary>` which comes with a Scaly compiler
may define integer types which are defined by their bit width and the presence
or absence of a sign. These types typically define type conversion and other
functions.

Numeric literals written into a Scaly program have no type by themselves.
The type of the value that is generated by the compiler is either inferred
from the usage of the literal, and if that is not possible, the smallest
possible integer type is assumed if the literal is an integer.

.. _floattypes:

Floating point types
====================

The :ref:`standard library <standardlibrary>` which comes with a Scaly compiler
may define floting point types which are defined by their bit width.
These types typically define type conversion and other functions.

Numeric literals written into a Scaly program which do not represent an
integral number are given a type that is inferred from the usage of that
literal. If this is not possible, a floating point type is assumed that the
compiler sees fit for the runtime environment for which the compiler was
provided.

.. _charactertype:

The character type
==================

The `char` type represents all possible character values which are possible
for the runtime environment for which the Scaly compiler is implemented.
The Scaly Programming Language makes no assumptions about the storage format
of the character value.

The :ref:`standard library <standardlibrary>` which comes with a Scaly compiler
may define utility functions for conversion and other tasks.

.. _arraytype:

The array type
==============

An array is a sequence of objects of the same type in memory. The type of the
array is defined by its length and the type of the objects that are contained
in the array.

An array can have either a fixed size if the length of the array is known at
compile time, or a variable size if the length of the array is not known at
compile time.

In the latter case, the count of the objects is stored as a packed integer
in front of the sequence of the objects in memory.

The packed integer format stores stores a number to be encoded as a sequence
of digits to the base of 127. Each digit is stored in one byte, the lowest
digit coming first. The highest digit has its highest bit set to 0 which
signals the end of the sequence of digits of the number. The highest bit of the
lower digits is set to 1. The lower 7 bits of all bytes encode the value of the
digit as an unsigned byte number.

Thus, array lengths from 0 to 127 are encoded in just one byte, and lengths
from 128 to 16128 are encoded in two bytes, lengths from 16128 to about
2 millions take three bytes and so on.

.. _stringtype:

The string type
===============

The `string` type is technically an :ref:`array <arraytype>` of bytes.
This means that the length of the string which is stored along with the bytes
is the length of the byte sequence which represents the string and *not*
the count of characters which the string contains. The difference to a
byte array is the fact that the runtime library can interpret the stored
byte sequence as a stream of characters and, based on that interpretation,
can implement string access and manipulation functionality.

String constants in memory are stored in the
`UTF-8 <https://en.wikipedia.org/wiki/UTF-8>`_ format.

.. _standardlibrary:

**************************
The Scaly standard library
**************************

This section describes the types of the standard library that comes with the
reference implementation of the Scaly Programming Language. You can use the
Scaly Programming Language without the standard library, and provide your
own basic types.

.. _bool:

Boolean Values
==============

There is one boolean value type, ``bool``. It can have two values, ``false``
and ``true``.

.. _ints:

Integral number types
=====================

An integral number type is determined by the range of integral numbers which
the number can take on. Scaly defines a set of integral number types which are
characterized by their bit width, and whether they carry a sign or not:

* ``byte`` and ``sbyte`` (8 bits unsigned and signed)
* ``short`` and ``ushort`` (16 bits signed and unsigned)
* ``int`` and ``uint`` (32 bits signed and unsigned)
* ``long`` and ``ulong`` (64 bits signed and unsigned)

.. _char:

Characters
==========

The ``char`` type represents exactly one character. A character is uniquely
identified by its code point which is a non-negative integral number. Which
code points are valid and the semantics of the individual characters are
determined by the character set used by the operating system and/or the
runtime library.

.. _grammar:

*******
Grammar
*******

The grammar of the expressions of the Scaly Programming Language is defined as
an `SGML <https://en.wikipedia.org/wiki/Standard_Generalized_Markup_Language>`_
document.

The reference implementation of the Scaly Programming Language actually
generates its complete parser and all AST classes directly out of the grammar
description given in the follwoing sections.

.. _metagrammar:

Metagrammar
===========

Since SGML itself is a meta-language, the language in which the grammar is
formulated is expressed as the following SGML DTD which is explained below::

.. literalinclude:: ../../scalysh/grammar.dtd

A ``grammar`` contains at least one ``syntax`` rule, zero or more ``keyword``
elementss, and zero or more ``punctuation`` elements.

A ``syntax`` contains zero or more ``content`` elements.

A ``syntax`` rule can be ``abstract`` or ``concrete``, with ``concrete``
as the default. An `abstract` syntax rule is a superset of other syntax rules.
An example for an abstract syntax is an ``Expression``. As a convention,
an abstract syntax contains only links to other syntax rules which indicate
what the syntax can be. An ``Expression``, for instance, can be a
``SimpleExpression`` (being abstract itself), or a ``Block``
(which is concrete), or one of a number of other expressions.

A concrete syntax rule contains content elements which describe the contents
of that syntax. A ``Block``, for instance, contains a ``leftCurly``
punctuation, ``multiple`` ``Expression`` elements, and a ``rightCurly``
punctuation. Other content can be an ``identifier``, a ``keyword``,
a ``literal``, or an ``eof`` (the latter one signals the end of the file.)

A concrete syntax rule which is an instance of an abstract syntax rule,
needs to indicate its ``base`` syntax rule.

The top-level syntax rule of a module needs the ``top`` attribute.

The root syntax rule of the grammar needs the ``program`` attribute.

A ``content`` item can ``link`` to a ``syntax``, ``keyword``,
or ``punctuation`` element. The ``property`` attribute is the name of the
syntax member variable name in the AST. If a ``content`` links to a
``syntax``, the ``multiple`` attribute indicates that this syntax can occur
multiple times in that context, and the ``optional`` attribute indicates that
this syntax is optional in that context.

The ``keyword`` has its value as its ``id``.

The ``punctuation`` has its value in the ``value`` attribute.

.. _scalygrammar:

Scaly Grammar
=============

Below the grammar of the Scaly programming language is defined in terms of the
meta grammar given in the :ref:`metagrammar`. Please note that
a character sequence complying to the grammar is not necessarily a valid
Scaly program. All valid Scaly programs, howver, comply with this grammar.
The semantic requirements for the expressions to form a valid Scaly program
are described in the :ref:`program` section and the sections that follow.

.. literalinclude:: ../../scalysh/scaly.sgm

**********
Object Age
**********

In Scaly, *objects live on the stack*, either defined as local items, or owned
by or referenced by other objects, arrays, or dictionaries, which in turn live
somewhere on the stack directly or indirectly (held by other objects).

If a block is left, the memory of all objects which were created in this block
is recycled. Therefore, a reference must not be held by an item that outlives
the object it references::

  let a: Foo&
  {
      let b: Foo = Foo()
      // a = &b // If b goes out of scope, a would point to recycled memory
  }

To make `b` assignable to `a`, its declaration can be moved to the outer block::

  let a: Foo&
  let b: Foo
  {
      b = Foo()
      a = &b
  }

The lifetime of an object is determined by the place where a reference to it
is declared. The older an object, the longer it lives. Since older data live
at least as long as younger data, it can never happen that references to
dead data are accessible.

The age of data depends on where it is declared. Items declared in a
local block are younger than items in the enclosing block. Parameters that are
passed to a function are older than its local items::

  function f(a: Foo) {
      let b: Bar = Bar()
      {
          let c: Caf = Caf()
      }
  }

In this example, `a` is oldest, `b` is younger than `a`, and `c` is youngest.

A reference returned by a function is assumed to be *fresh* by default.
This means that the function creates the object (either by calling an object
constructor or another function which returns a reference to a fresh object).
The caller of such a function then assigns the returned reference to an item
whose location determines the age of the object::

  function g(): Foo {
      return Foo(42) // Fresh object created and returned
  }

  function h() {
      let k: Foo = g() // The object lives here, accessible via k
  }

If a function is to return an object which is not fresh, the age of such a
returned object must be made explicit by an *age tag* which is written after
the type of the return value.

An age tag starts with a single quote (`&`) and continues with digits which
form a nonnegative number. Leading zero digits are not allowed. `&0` is a
valid age tag, `&42` is a valid age tag as well, whereas `&01` is not a valid
age tag.

Since Scaly does not know global mutable data, there must be one or more
parameters from which to take the returned reference in some way,
age tag numbers are used to express age relations between the parameters of a
function. The higher the age tag value is, the younger is the tagged
reference::

  function superiorFoo(fooOld: Foo&1, fooYoung: Foo&2): Foo&2 {
      if fooOld.number > fooYoung
          fooOld
      else
          fooYoung
  }

In this example, the returned refrerence can be taken from any of the two
parameters, and so its age must be that of the youngest parameter.

The following example checks assignments for age validity::

  function bar(mutable foo: Foo&1, mutable bar: Bar&2) {
      bar.foo = foo // Valid because foo is declared older
      // foo.bar = bar // Invalid because bar is younger
  }

If age tags are omitted, the age of the parameters is irrelevant::

  function baz(p: Foo, q: Foo) -> bool {
      p.number > q
  }

The age of a member is assumed to be the same as the age of the object
containing it (even though the object it points to might be older). Similarly,
the age of an array element is assumed to be that of the array, and the age of
a dictionary key or value is assumed to be that of the dictionary.

******************
Parallel Execution
******************

Not only statements in a block are executed in parallel, but also
*function arguments* and *operands*, as long they are *pure*
and do not depend on earlier computations in the block.

A computation is called pure if it does not depend on anything else
than its input parameters. (With some care, even computations that obtain
information from external input can be declared pure by you if needed.)

That said, scheduling parallel computation comes at a cost - tasks have to be
created and scheduled for execution by a local worker thread pool, by a GPU,
or even by a cluster of remote machines. In the latter case, input data have
to be serialized and sent via the network to the remote node, where the data
are deserialized. When the computation is done, its results have to be
sent back. Last not least the parallel work has to be synchronized.

Therefore, a Scaly implementation has to justify parallel execution at least
by some heuristic reasoning, better by profiling a set of reference
computation workloads. Scheduling some single floating point additions which
might each take nanoseconds or less for parallel execution surely isn't worth
the overhead. Parsing a multitude of source files in contrast can be expected
to speed up compiling a program, and performing heavy number crunching needed
for fluid mechanics calculations in parallel would a safe bet.

Adjusting the granularity of parallel execution, however, is beyond the
Scaly language specification which only states what computations can
*potentially* be done in parallel, or to be exact, makes no statement
about the order in which independent computations are done.
