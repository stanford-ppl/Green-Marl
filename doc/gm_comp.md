1 Introduction
======================================

This document provides a brief explanation about gm_comp, a 
Green-Marl compiler.

gm_comp is a compiler for Green-Marl. 


Please refer to [Readme documentary](../README.md) which explains
how to set up and build the compiler; once you finish following that document,
gm_comp should be created at $(top)/bin.


2 Compiler Options
========================================





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
