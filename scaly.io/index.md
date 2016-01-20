Scaly
=====

Welcome to _Scaly_, the _self-scaling programming language_.

Scaly uses implicit [region-based memory management](https://en.wikipedia.org/wiki/Region-based_memory_management) combined with [value semantics](https://en.wikipedia.org/wiki/Value_semantics) where objects conceptually live on the stack. No heap, no garbage collection, no reference counting, not even pointers - although references to objects can be made and used within well-defined constraints.

C++ is used to implement [runtime support and standard library code](https://github.com/rschleitzer/scaly) as well as for [bootstrapping](https://en.wikipedia.org/wiki/Bootstrapping_%28compilers%29) the evolving [compiler](https://github.com/rschleitzer/scalyc) till the compiler can [host itself](https://en.wikipedia.org/wiki/Self-hosting).
