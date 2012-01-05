1 Introduction
======================================

This document provides a walk-through to write down your own Green-Marl
program and to execute it.  Before proceed, please read $(top)/Readme.md and
follow the steps in the document, which guides you to build up the
compiler properly.


2 First program: Hello World
======================================

Now let us write the traditional "Hello World" application with Green-Marl.
Note that we will re-use the _makefile_ scripts that are already provided with the sample
application. However, this is not mandatory but you are free to create your own
makefile scripts, once you're accustomed to Green-Marl environment.

Also we recommend to use vim as a text editor, because we provide vim
Syntax-Highlighting support for Green-Marl. See README.md for details.


2-1 Writing hello_world.gm
--------------------------------------

We will add our new program in $(top)/apps/src
    
    cd $(top)/apps/src
    vi hello_world.gm    %% Start write your code with vi

Now write your first Green-Marl program as follows:

    Procedure hello_world() 
    {
        [ printf("Hello world\n") ];
    }
    
Unfortunately, hello_world program does not exercise much of interesting features of Green-Marl.

**Procedure** is a keyword that indicates a Green-Marl procedure, followed by procedure name and
argument lists (inside parenthesis) and the procedure body (inside braces).

There is only one statement in the body. Interestingly the statement is enclosed within a bracket \[ \]. 
This denotes _foreign statement_ in Green-Marl, which means that any string inside a bracket
is not Green-Marl statements but the statements of the target language, which in our case is C++. The compiler
will simply reproduce this string in the proper place, when generating the target code.


2-2 Compiling hello_world.gm
--------------------------------------

Now we will compile hello_world.gm into target (i.e. C++) code. 
First, let's add hello_world progam to our Makefile script.

    cd $(top)/apps
    vi Programs.mk
    %%%% Programs.mk has the following line:
    %%%%    PROGS=pagerank conduct bc kosaraju 
    %%%% Add hello_world at the end of it:
    %%%%    PROGS=pagerank conduct bc kosaraju hello_world
    
Now we can compile the hello_world program through the following command:

    cd $(top)/apps
    make gen         %% this command generates .cc file out of .gm file
    ls $(top)/apps/output_cpp/generated/hello_world.cc   % check the code has been well created.
 
In fact, the actual shell command invoked by the previous make is as following:

    cd $(top)/apps/src
    ../../bin/gm_comp -o=../output_cpp/generated -t=cpp_omp hello_world.gm
    
The command is invoking gm_comp compiler with 
  -o option (where to put generated code) and
  -t option (what is the target environment; cpp_omp means C++ with OpenMP)
in addition to the source filename, hello_world.gm

Before we move on, let us take a look at what is the generated code look like.

    cd $(top)/apps/output_cpp/generated/
    cat hello_world.cc % The generated code shall look like as follows: 
    
    #include "hello_world.h"
    
    void hello_world()
    {
       // Initializations
       gm_rt_initialize();
       
       printf("hello_world\n");
    }
    
Noticeablly, the compiler inserted _runtime_ initialization code at the function entry. 
(If the runtime has been already initialized, the call to `gm_rt_initialization()` returns
immediatedly.) Other than that, the compiler simply reporduced foreign syntax in place, as expected.


2-3 Creating an executable and Running it
--------------------------------------





3 Second Applications
======================================

Now let us write the traditional "Hello World" application with Green-Marl.
Note that we will re-use the _makefile_ scripts that are already provided with the sample
application. However, this is not mandatory but you are free to create your own
makefile scripts, once you're accustomed to Green-Marl environment.


4 Further information
======================================

Please refer to the following documents for further information

* Green-Marl language specification : about the language (syntax and semantic)
* compiler.txt : about gm_comp compiler
* gm_graph.txt : about c++ runtime and graph library.

