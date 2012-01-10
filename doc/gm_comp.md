1 Introduction
======================================

This document provides a brief explanation about gm_comp, a 
Green-Marl compiler. gm_comp reads a Green-Marl program and generates
equivalent code for the target environment. Currently gm_comp is able
to generate C++ code for cache coherent shared-memory environment, and
Java code for distributed execution based on a Pregel-Like environment. 

Please refer to [Readme document](../README.md) which explains
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
 * -t=\<string\> option selects the target environment
     + cpp_omp : generate multi-threaded C++ implementation using OpenMP
     + cpp_seq : generate single-threaded C++ implementation 
                 (TODO: still some library initialization routine uses OpenMp internally.)
     + gps     : generate java implementation for GPS environment
 * -o=\<string\> option specifies where the generated file will be located. Default is ./ 

The next four options are used to track-down what compiler is doing with given code. 

 * -V=\<int\> option specifies compiler verbosity level. Default is 0.
     + -V=1: shows the name of each compilation step
 * -DS=\<string\> option lets the user to stop the compiler after certain stage.
 * -Dr=1 option lets the compiler to reproduce Green-Marl code when it is  stopped by -DS option.
 * -Dw=1 option lets the compiler to print rw analysis information when it is stopped.



2.2 Tracking Compiler Optimization
----------------------------------------

gm_comp allows the user to stop the compiler at any step, and to reproduce the Green-Marl code again, 
but with all the optimizations up to that point being applied. This is very helpful to debug your program 
(or debug the compiler and blame the authors of the compiler).

Let us try this with an example. Make a short Green-Marl program named foo.gm

    Proc foo (G:Graph, A,B: N_P<Int>(G)) : Int // Proc is a short-hand for Procedure, N_P for Node_Property
    {
        Int X = Sum(s:G.Nodes) {s.A};
        Int Y = Sum(t:G.Nodes) {t.B};
        Return X*Y;
    }

Now let us compile this with verbosity option; the compiler will print out 
a message at each stage it is perfoming.
(The printed result may differ slightly depending on the compiler version).

    ./gm_comp -V=1 foo.gm
    ......
    ...Stage 2.1: Frontend.[Check syntax rules]
    ...Stage 2.2: Frontend.[Regularize syntax (without typeinfo)]
    ......
    ...Stage 3.2: Indep-Opt.[Regularize syntax]
    ......
    ...Stage 3.6: Indep-Opt.[Merge loops]
    ......
    ...Stage 6.8: Code Generation.[Creating source for each procedure]

The compiler goes through 6 major steps.

  * 1: Parsing
  * 2: Type and Data-race Checking
  * 3: Backend Independant Optimization
  * 4: Backend Specific Optimization
  * 5: Library Specific Optimization
  * 6: Code Generation

For example, you can stop the compiler after Stage 2 by the following command. 
When stopped, the compiler will reproduce the program as -Dr=1 option is set by default.

    ./gm_comp -V=1 -DS=2 foo.gm
    ......
    ...Stage 2.11: Frontend.[Check RW conflict errors]
    ...Stage 2.12: Frontend.[Remove variable declarations (Use Symtab)]
    ...Stopping compiler after Stage 2:Frontend
    ======================================================
    Procedure foo(
        G : Graph,
        A : N_P <Int>(G),
        B : N_P <Int>(G)) : Int
    {
        Int X;
        Int Y;
        X =  Sum(s: G.Nodes){s.A} ;
        Y =  Sum(t: G.Nodes){t.B} ;
        Return X * Y;
    }
    ======================================================

Now we can trace the compiler optimization. 
Let's stop the compiler after Stage 3.2 (Regularize Syntax), where the in-place reductions are
expanded into explicit loops.

    ./gm_comp -V=1 -DS=3.2 foo.gm
    ......
    ...Stage 3.2: Indep-Opt.[Regularize syntax]
    ...Stopping compiler after Stage 3.2:Indep-Opt.[Regularize syntax]
    ======================================================
    Procedure foo(
        G : Graph,
        A : N_P <Int>(G),
        B : N_P <Int>(G)) : Int
    {
        Int X;
        Int Y;
        Int _S0;
        Int _S1;
        _S0 = 0;
        Foreach (s : G.Nodes)
            _S0 += s.A @ s ;

        X = _S0;
        _S1 = 0;
        Foreach (t : G.Nodes)
            _S1 += t.B @ t ;

        Y = _S1;
        Return X * Y;
    }
    ======================================================
    
Now, let the compiler proceed a little bit more so that it can re-arrange the statements and merge the loops.

    ./gm_comp -V=1 -DS=3.6 foo.gm
    ......
    ...Stopping compiler after Stage 3.6:Indep-Opt.[Merge loops]
    ======================================================
    Procedure foo(
        G : Graph,
        A : N_P <Int>(G),
        B : N_P <Int>(G)) : Int
    {
        Int X;
        Int Y;
        Int _S0;
        Int _S1;
        _S0 = 0;
        _S1 = 0;
        Foreach (s : G.Nodes)
        {
            _S0 += s.A @ s ;
            _S1 += s.B @ s ;
        }

        X = _S0;
        Y = _S1;
        Return X * Y;
    }
    ======================================================



2.3 Using vi + make + gm_comp
----------------------------------------

If gm_comp is invoked by make, which is again invoked inside vi (by :make), 
vi will capture any the error message generated by gm_comp during make,
and jump to the error location automatically.



3 C++ Backend
========================================

gm_comp can translate Green-Marl programs into c++ programs,
targetting cache-coherent shared-memory multi-processor systems, 
such as muti-core x86. 
gm_comp can produce, however, either single-threaded version or
multi-threaded version, depending on -t command-line option. 

Codes that are generated by gm_comp assumes a runtime/graph library
named gm_graph. A more detailed information 
about gm_graph can be found in a separate document. 


3.1 Type Mapping
-----------------------------------------
There is a one-to-one mapping Green-Marl types
and C++ types.

| GM Type   | C++ Type  |
|-----------|-----------|
| Int       | int32_t   |
| Long      | int64_t   |
| Float     | float     |
| Double    | double    |
| Graph     | gm_graph&  |
| N_P\<T\>  | pointer to the array of \<T\> |
| E_P\<T\>  | pointer to the array of \<T\> |
| Node      | node_t |
| Edge      | edge_t |
| Node_Order | gm_node_order& |
| Node_Seq   | gm_node_seq& |
| Node_Set   | gm_node_set& |

* gm_graph, gm_node_order, gm_node_set, gm_node_seq is defined in gm_graph library.
* Each node_t and edge_t is type-defined as either int32_t or int64_t.
* Node_Propery and Edge_Property is mapped into arrays having O(N) and O(E) size, respectively.


3.2 Graph/Runtime Library
-----------------------------------------
C++ codes generated by gm_comp
assumes gm_graph library. 






4 GPS Backend
========================================

This feature is still experimental (more than other features). 
Please refer to [Readme document](../README.md) to see what is GPS and how gm_comp can generate
GPS output.



4 Known Issues (To be resolved: before _real_ release)
========================================

* Currently, it is not possible for a Green-Marl procedure to make a call to other GM procedures 
  However, this can be circumvented a little bit, since the main function can make
  function calls to those procedures.

* Only the directed graphs are supported.

* Implementation of parallel addition/removal to the collection has not finished. 
     + Compiler should check the mutation gender inside parallel region.
       (should not allow concurrent removal and additioin at the same time.)
     + Compiler should apply appropriate interface to the collection.
        (e.g. add_seq vs add_par, depending on the context)

* Ad-hoc implementation used for paper submission is now being re-implemented.
     + Argmax implementation
     + Flipping edge optimization
     + Saving BFS children optimization
     + (node iterator).edge()
