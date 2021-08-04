# Fuzz-headers
Helper files for making fuzzer writing easier.

The header files are used to quickly extract higher level data types
based on a random array, i.e. fuzzer input. The goal is also to make
some maintenance easier related to these types. As such, in the C
implementation you will also find some super light garbage collection
(make sure you read the code before using it) that can be used to easily
keep track of the types extracted by these headers. 

The files in this project progresses quite dynamically. We created them 
because we often found ourselves met with repetitive code constructs 
from fuzzing many projects. The goal is to unify some of these concepts
to make creating fuzzers easier. 
