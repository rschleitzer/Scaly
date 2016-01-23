Scaly
=====

Welcome to [_Scaly_](https://github.com/rschleitzer/Scaly), the _self-scaling programming language_.

Overview
--------

Scaly uses implicit [region-based memory management](https://en.wikipedia.org/wiki/Region-based_memory_management) 
combined with [value semantics](https://en.wikipedia.org/wiki/Value_semantics) where objects conceptually live on the stack. 
No heap, no garbage collection, no reference counting, not even pointers - although references to objects can be made 
and used within well-defined constraints.

C++ is used to implement [runtime support and standard library code](https://github.com/rschleitzer/Scaly/tree/master/scaly) 
as well as for [bootstrapping](https://en.wikipedia.org/wiki/Bootstrapping_%28compilers%29) 
the evolving [compiler](https://github.com/rschleitzer/Scaly/tree/master/scalyc) 
till the compiler can [host itself](https://en.wikipedia.org/wiki/Self-hosting).

Building
--------

You can build Scaly taking the following steps:

1. Install [CodeLite](http://codelite.org/) and run the Setup Wizard.

2. Clone the github repository:

        git clone https://github.com/rschleitzer/Scaly.git
    
3. Open `scaly.workspace` and build.

Currently, only Ubuntu 15.10 is known to build.

License
-------

Scaly and the whole contents of the [scaly.io](http://scaly.io) web site is subject to the [MIT License](https://github.com/rschleitzer/Scaly/blob/master/LICENSE).