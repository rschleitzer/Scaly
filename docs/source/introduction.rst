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

Writing the Top Level Code
==========================

In this section, we will write the top level code of the tool which does two
things:

1. Call a function to convert the arguments passed to the tool into an options
   object, and
2. Pass this options object to the compiler.

The arguments passed to a command line program are bound to a variable which
is visible at the top code level whose name is ``arguments``. This parameter
is a string iterator, which we can iterate on to fetch each argument from the
command line.

Thus, our first task will be to write an options parser which grabs the
arguments and converts them into an ``Options`` object. This options object
will be used by the Scaly compiler. So we write the following code into the
``scalyc.scaly`` file::

  use scalyc.options
  use scalyc.compiler

This bring two modules into scope, ``scalyc.options`` and ``scalyc.compiler``,
which we will write next. Then add the (rather complex) statement to the
``scalyc.scaly`` file::

  let options = Options.parse_arguments(arguments)
    catch NullLengthArgument {
        print "Null length argument!"
        return
    }
    catch EmptyOption {
        print "Empty option!"
        return
    }
    catch InvalidOption(option) {
        print "Invalid option" option "!"
        return
    }
    catch UnknownOption(option) {
        print "Unknown option" option "!"
        return
    }

Here we assign an ``options`` object which will server as input to our
compiler. It is created by the ``parse_arguments`` method of the ``Options``
class which we will write in the next section and which will take the
``arguments`` iterator as an argument. Since the ``parse_arguments`` function
can throw some errors, we have to catch each of them. A ``catch`` clause has
3 options:

1. Convert the error to a value of the type which is returned by the function
   normally,
2. Let the current function return a value, or
3. Throw an error.

Here, all ``catch`` clauses choose the second option by returning nothing after
printing an error message. This is OK
since we are at the top-level scope which returns nothing. If no error was
returned by the ``parse_arguments`` function, the result is stored in the
``options`` variable. We use that variable by adding a last line to our
``scalyc.scaly`` file::

    Compiler.compile(options)

Here, we finally take the options object and feed it into the ``compile``
method of the ``Compiler`` class which we will write in one of the next
sections.

Processing command line arguments
=================================

We still have two classes to write, ``Options`` and ``Compiler``. In this
section we will write the first of them.

We create a new file ``options.scaly`` and enter the following code::

  namespace scalyc.options {

      class Options {

          files: Array[String]
          output_name: String
          directory: String

          function parse_arguments(var args: std.env.Args) : Options ! OptionsError {

              var output_name ""
              var directory ""
              var files: Array[String] []
              var first_argument true

              loop {
                  let arg args.next()
                      catch return Options(output_name, directory, files)

                  if first_argument {
                      set first_argument: false
                      continue
                  }

                  var char_iterator arg.chars()

                  let first_char char_iterator.next()
                      catch throw NullLengthArgument

                  if first_char <> '-' {
                      files.push(arg.clone())
                      continue
                  }

                  let second_char = char_iterator.next()
                      catch throw EmptyOption

                  switch second_char {
                      'o': set output_name args.next()
                          catch throw InvalidOption(arg.clone())

                      'd': set directory args.next()
                          catch throw InvalidOption(arg.clone())

                      default: throw UnknownOption(arg.clone()))
                  }
              }
          }
      }

      variant OptionsError {
          NullLengthArgument
          EmptyOption
          InvalidOption(string)
          UnknownOption(string)
      }
  }

Wow, that's quite a lot of code. We will go through it bit by bit while
discussing some features of the Scaly programmin language.

The first line tells us that everything which is embraced by the curly
brackets is defined in the ``scalyc.options`` namespace::

  namespace scalyc.options {

The next line begins to define a class ``Options`` in that namespace::

  class Options {

(Without the first line, we could have written the following line::

  class scalyc.options.Options {

which would have had the same effect). The next lines define the data members
of the class::

  files: Array[String]
  output_name: String
  directory: String

The ``files`` member is an array of strings, and the ``output_name`` and
``directory`` members are strings.

Then the member function ``parse_arguments`` of the ``Options`` class is
opened::

  function parse_arguments(var args: std.env.Args) : Options ! OptionsError {

This function takes exactly one argument ``args`` of the type ``Args`` class
in the ``std.env`` namespace. (We did not bother to ``use std.env`` so we
need to fully qualify the name here.)

It returns an ``Options`` object as indicated by the ``:`` clause, and the
``!`` clause tells us that in case of an error the function throws an
``OptionsError`` object.

Next, some local variables are defined and initialized::

  var output_name ""
  var directory ""
  var files: Array[String] []
  var first_argument true

The ``var`` keyword tells us that the items are *variables* which means that
their initial values can be changed later on, as we will see. After the name,
an type annotation can follow, like for the ``files`` variable. In fact,
the ``files`` variable needs the type because the ``[]`` expression does not
tell here what type of array we need. The type of the other three variables
can be inferred from their initialization values.

