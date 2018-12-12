############
Introduction
############

*****************
The First Program
*****************

The most basic expressions in Scaly are *literals*. A literal simply returns
a value that is written literally into a program. So our first program is
not the famous *Hello World* version in Scaly (which comes later
because it is obligatory), instead we write up the shortest non-empty
Scaly program imaginable::

  0

Use your favorite text editor and type that lone zero digit into a file,
give it a name like ``shortest.scaly``, and compile it to an executable.
As you might expect, the program does exactly nothing useful. Immediately after
startup, it returns a ``0`` value to the operating system just to indicate
that everything went fine. If a program is to return ``0`` as the last action
at the end of it, that ``0`` literal can be omitted. That's why the shortest
Scaly program actually is the empty program which contains nothing and does
exactly the same as the above version - just returning ``0``.
Try it by deleting the zero digit, compiling and running the program.

There are also string literals like ``"Hello World!"``, the two bool literals
``true`` and ``false``, and more.

***********
Hello World
***********

Scaly comes with a standard library which among other stuff
contains a function which prints a string to the standard output (which is the
output of your terminal, or a debug console, depending on the environment
in which your program was started into). The function has the name ``print``
and accepts a string, returning nothing to the caller. We use it to write up
the inevitable *Hello World* program in Scaly::

  print "Hello World!"

When you run that program you will notice that no line break is printed.
The reason is that we did not include one in our string literal,
and the print function does not print a line break by itself.
We correct that by inserting a line break literally::

  print "Hello World!
  "

Scaly can contain all kinds of whitespace literally. If you don't like
to span string literals more than one line for readability reasons
or want more compact code, you can use an escape sequence as well::

  print "Hello World!\n"

A more convenient function of the standard library is the function
``print_line`` which prints the string with a trailing line break::

  print_line "Hello World!"

****************************
Building a command line tool
****************************

With the obligatory *Hello World* out of the way, we can now try to do
something useful.

Let's write a parser for a toy language (which happens to be a subset of the
Scaly programming language).  We create an empty file and give it the name
``scalyc.scaly``.

First, we want to read the command line arguments and parse them into compiler
arguments and options. We will have the following compiler options:

1. ``-o`` sets the name of the program which the compiler will produce.

2. ``-d`` sets the directory of the output of the compiler.

All other arguments
will supply the input files to the
compiler.

To return an ``int`` value (even an implict ``0``) is part of the
calling convention for stand-alone programs: Zero or more string
arguments are passed to the program, and an integer value is returned
which acts as an error code which might be useful in extreme error situations
if all other means of diagnosis like logging or tracing fail because they
do not work for some pathologic reason which can be indicated
by that error code.

Getting the length of an array
==============================

The arguments passed to the program are bound to one parameter which
is visible at the top code level whose name is ``arguments``.
This parameter is an array of strings, and we can get its ``length`` field.
So our next Scaly program (not much useful either)
returns the number of arguments passed at the command line::

  arguments.length

This expression actually consists of two parts: an item identified by the
``arguments`` identifier and the access of its ``length`` field via the dot.

