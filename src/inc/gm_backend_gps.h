#ifndef GM_BACKEND_GPS
#define GM_BACKEND_GPS

#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_gps_basicblock.h"

#include <list>
#include <map>

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_gps_gen;
class gm_gpslib : public gm_graph_library {
    public:
    gm_gpslib() {main = NULL;}
    gm_gpslib(gm_gps_gen* gen) {main = gen;}
    void set_main(gm_gps_gen* gen) {main = gen;}

    virtual const char* get_header_info() {return "";}
    virtual const char* get_type_string(ast_typedecl*, int usage) {return "";}

    virtual bool generate(ast_nop* n) {return true;}
    virtual bool generate_builtin(ast_expr_builtin* e) {return true;}

    virtual bool do_local_optimize() {return true;}

    protected:

    private:
        char str_buf[1024*8];
        gm_gps_gen* main;
};



//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
// state number,
// begin sentence
// is pararell
class gm_gps_gen : public gm_backend 
{
    public:
        gm_gps_gen() : dname(NULL), fname(NULL), f_body(NULL), proc(NULL) {
            glib = new gm_gpslib(this);
        }
        virtual ~gm_gps_gen() { close_output_files(); delete [] dname; delete [] fname; }
        virtual void setTargetDir(const char* dname) ;
        virtual void setFileName(const char* fname) ;

        virtual bool do_local_optimize_lib() {return true;}
        virtual bool do_local_optimize() ;
        virtual bool do_generate(); 


    protected:
        gm_gpslib* get_lib() {return glib;}
        void do_generate_main();
        bool do_merge_msg_information();
        void set_current_proc(ast_procdef* p) {proc = p;}
        ast_procdef* get_current_proc() {return proc;}



        void write_headers();
        void begin_class();
        void end_class();
        bool open_output_files();
        void close_output_files();

        //----------------------------------
        // stages in backend opt
        //----------------------------------
    protected:
        void do_create_stages();
        bool do_check_synthesizable();
        void merge_basic_blocks(gm_gps_basic_block* entry);

    private:
        char* dname;
        char* fname;
        gm_code_writer Body;
        FILE* f_body;
        gm_gpslib* glib; // graph library
        ast_procdef* proc;
};



#endif
