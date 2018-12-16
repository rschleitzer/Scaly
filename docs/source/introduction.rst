############
Introduction
############

***********
Hello World
***********

As it is tradition with language tutorials, our first program will be the
famous *Hello World* program.

Scaly comes with a standard library which among other stuff
contains a function which prints a string to the standard output (which is the
output of your terminal, or a debug console, depending on the environment
in which your program was started into). The function has the name ``print``
and accepts a string, returning nothing to the caller. We use it to write up
the *Hello World* program in Scaly::

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

Getting the command line arguments
==================================

The arguments passed to a command line program are bound to a variable which
is visible at the top code level whose name is ``arguments``.
This parameter is a string iterator, which we can iterate on to fetch
each argument from the command line.


