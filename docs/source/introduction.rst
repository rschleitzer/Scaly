############
Introduction
############

***********
Hello World
***********

As it is tradition with language tutorials, our first program will be the
famous *Hello World* program.

So we create a file with the name ``hello.scaly`` and enter the following line
of code::

  print "Hello World!"

After saving the file, execute the following command from the command line
(assuming that the Scaly compiler can be started from the directory you are
in)::

  scalyc hello.scaly

The Scaly compiler takes the file which you currently have created, compiles
it into an executable program with the name ``hello`` which, as you might
have guessed) from the base name of your source file.

Then execute the program that you just have compiled::

  ./hello

When you run that program you will notice that no line break is printed.
The reason is that we did not include one in our string literal, and the print
function does not print a line break by itself.
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

We compile and run it with the following two commands::

  scalyc scalyc.scaly
  ./scalyc

The program does exactly nothing, as we have entered no code yet into the
``scalyc.scaly`` file.

Getting the command line arguments
==================================

The arguments passed to a command line program are bound to a variable which
is visible at the top code level whose name is ``arguments``. This parameter
is a string iterator, which we can iterate on to fetch each argument from the
command line.


