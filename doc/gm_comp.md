1 Introduction
======================================

This document provides a brief explanation about gm_comp, a 
Green-Marl compiler. gm_comp reads a Green-Marl program and generates
equivalent code for the target environment. Currently gm_comp is able
to generate C++ code for cache coherent shared-memory environment, and
Java code for distributed execution based on a Pregel-Like environment. 

Please refer to [Readme documentary](../README.md) which explains
how to set up and build the compiler; once you finish following that document,
gm_comp should be created at $(top)/bin.


2 Using the Compiler
========================================


2.1 Compiler Options
----------------------------------------

The following command will show the available compiler options:

    cd ($top)/bin
    ./gm_comp -h

 * Currently, the compiler can process only one input file at a time.
 * -h option shows the help message
 * -v option prints the version
 * -t option selects 

2.2 
----------------------------------------



2.3 Using vi + make + gm_comp
----------------------------------------



3 C++ Backend
========================================




4 GPS Backend
========================================



4 Known Issues (To be resolved: before _real_ release)
========================================

* Currently, it is not possible for a Green-Marl procedure to make a call to other GM procedures 
  However, this can be circumvented a little bit, since the main function can make
  function calls to those procedures.


* Implementation of parallel addition/removal to the collection has not finished. 
     + Compiler should check the mutation gender inside parallel region.
       (should not allow concurrent removal and additioin)
     + Compiler should apply appropriate interface to the collection.
        (e.g. add_seq vs add_par, depending on the context)

* Ad-hoc implementation used for paper submission is now being re-implemented.
     + Argmax implementation
     + Flipping edge optimization
     + Saving BFS children optimization
     + (node iterator).edge()
