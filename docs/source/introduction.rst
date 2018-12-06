############
Introduction
############

*****************
The First Program
*****************

The most basic expressions in Scaly are *literals*. A literal simply returns a value 
that is written literally into a program. So our first program is not the famous 
*Hello World* version in Scaly (which comes later because it is obligatory), 
instead we write up the shortest non-empty Scaly program imaginable::

  0

Use your favorite text editor and type that lone zero digit into a file, give it a name 
like ``shortest.scaly``, and compile it to an executable. As you might expect, 
the program does exactly nothing useful. Immediately after startup, it returns a ``0`` value 
to the operating system just to indicate that everything went fine. If a program is to return ``0`` 
as the last action at the end of it, that ``0`` literal can be omitted. That's why the shortest 
Scaly program actually is the empty program which contains nothing and does exactly the same 
as the above version - just returning ``0``. Try it by deleting the zero digit, compiling and 
running the program.

There are also string literals like ``"Hello World!"``, the two bool literals ``true`` and ``false``, 
and more.

***********
Hello World
***********

Scaly comes with a standard library which among other stuff contains a function 
which prints a string to the standard output (which is the output of your terminal, 
or a debug console, depending on the environment in which your program was started into). 
The function has the name ``print`` and accepts a string, returning nothing to the caller. 
We use it to write up the inevitable *Hello World* program in Scaly::

  print "Hello World!"

When you run that program you will notice that no line break is printed. The reason is that we 
did not include one in our string literal, and the print function does not print a line break 
by itself. We correct that by inserting a line break literally::

  print "Hello World!
  "

Scaly can contain all kinds of whitespace literally. If you don't like to span string literals 
more than one line for readability reasons or want more compact code, you can use 
an escape sequence as well::

  print "Hello World!\n"

**********************
Command line arguments
**********************

To return an ``int`` value (even an implict ``0``) is part of the calling convention 
for stand-alone programs: Zero or more string arguments are passed to the program, 
and an integer value is returned which acts as an error code which might be useful 
in extreme error situations if all other means of diagnosis like logging or tracing fail 
because they do not work for some pathologic reason which can be indicated by that error code.

The arguments passed to the program are bound to one parameter which is visible at the \
top code level whose name is ``arguments``. This parameter is an array of strings, 
and we can get its ``length`` field. So our next Scaly program (not much useful either) 
returns the number of arguments passed at the command line::

  arguments.length

This expression actually consists of two parts: an item identified by the ``arguments`` identifier
and the access of its ``length`` field via the dot.

***********
Code Blocks
***********

In all but the simplest programs, you structure your code using blocks. 
Like in commonplace languages of C descent, a block starts with a left curly brace, 
followed by zero or more statements, and ends with a right curly brace. An example::

  {
      let a: int = 2
      let b: int = 3
      a + b
  }

But there are important differences to C and friends:

First, *code blocks are expressions* - they may return a value. Because of this, 
if the last statement of a code block is an expression, the value which is returned 
by that statement is the value which the code block returns. Obviously, our example block 
returns an ``int`` whose value is ``5``, and therefore is a valid Scaly program.

Second, the statements of a block can be *executed in any order* as far as data dependencies 
and the pureness of called functions allow. That is the main selling point of Scaly - 
it automatically schedules parallel and even distributed computation wherever possible. 

******************
Parallel Execution
******************


Not only statements in a block are executed in parallel, 
but also *function arguments* and *operands*, as long they are *pure* 
and do not depend on earlier computations in the block.

A computation is called pure if it does not depend on anything else than its input parameters. 
(With some care, even computations that obtain information from external input 
can be declared pure by you if needed.)

That said, scheduling parallel computation comes at a cost - tasks have to be created 
and scheduled for execution by a local worker thread pool, by a GPU, or even by a 
cluster of remote machines. In the latter case, input data have to be serialized 
and sent via the network to the remote node, where the data are deserialized. 
When the computation is done, its results have to be sent back. 
Last not least the parallel work has to be synchronized.

Therefore, a Scaly implementation has to justify parallel execution at least 
by some heuristic reasoning, better by profiling a set of reference computation workloads. 
Scheduling some single floating point additions which might each take nanoseconds or less 
for parallel execution surely isn't worth the overhead. Parsing a multitude of source files 
in contrast can be expected to speed up compiling a program, and performing 
heavy number crunching needed for fluid mechanics calculations in parallel would a safe bet.

Adjusting the granularity of parallel execution, however, is beyond the 
Scaly language specification which only states what computations can *potentially* 
be done in parallel, or to be exact, makes no statement about the order in which 
independent computations are done.

