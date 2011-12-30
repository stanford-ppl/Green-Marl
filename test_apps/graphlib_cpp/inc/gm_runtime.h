#ifndef GM_RUNTIME_H
#define GM_RUNTIME_H

//----------------------------------------------------
// A thin layer over openmp 
//----------------------------------------------------
// Thin layer over open-mp.
//----------------------------------------------------

class gm_runtime {
public:
    gm_runtime();
    virtual ~gm_runtime();

    void initialize();
    bool is_initialized();
    int  get_num_threads();
    void set_num_threads(int n);
    int  get_thread_id();

private:
    bool is_init;
    int  num_threads;
};

extern gm_runtime _GM_RT;

static inline void gm_rt_initialize()            {_GM_RT.initialize();}
static inline bool gm_rt_is_initialized()        {return _GM_RT.is_initialized();}

static inline int  gm_rt_get_num_threads()       {return _GM_RT.get_num_threads();}
static inline void gm_rt_set_num_threads(int n)  {_GM_RT.set_num_threads(n);}
static inline int  gm_rt_thread_id(int n)        {return _GM_RT.get_thread_id();}



#endif
