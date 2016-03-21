Scaly
=====

Welcome to https://github.com/rschleitzer/Scaly[_Scaly_], the _self-scaling programming language_.

Overview
--------

Scaly uses implicit https://en.wikipedia.org/wiki/Region-based_memory_management[region-based memory management] 
where objects conceptually live on the stack. 
No heap, no garbage collection, no reference counting required. 
Objects are accessed via references within well-defined constraints.

C++ is used to implement https://github.com/rschleitzer/Scaly/tree/master/scaly[runtime support and standard library code] as well as for https://en.wikipedia.org/wiki/Bootstrapping_%28compilers%29[bootstrapping] the evolving https://github.com/rschleitzer/Scaly/tree/master/scalyc[compiler] till the compiler can https://en.wikipedia.org/wiki/Self-hosting[host itself].

Building
--------

You can build Scaly taking the following steps:

1. Install http://codelite.org/[CodeLite] and run the Setup Wizard.

2. Clone the github repository:

  git clone https://github.com/rschleitzer/Scaly.git
    
3. Open `scaly.workspace` and build.

Currently, only Ubuntu 15.10 is known to build.

License
-------

Scaly and the whole contents of the http://scaly.io[scaly.io] web site is subject to the https://github.com/rschleitzer/Scaly/blob/master/LICENSE[MIT License].
