#ifndef GM_BACKEND_CPP
#define GM_BACKEND_CPP

#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_compile_step.h"
#include "gm_backend_cpp_opt_steps.h"
#include "gm_backend_cpp_gen_steps.h"
#include "../backend_cpp/gm_cpplib_words.h"

#include <list>

//-----------------------------------------------------------------
// interface for graph library Layer
//  ==> will be deprecated
//-----------------------------------------------------------------
class gm_cpp_gen;
class gm_cpplib: public gm_graph_library
{
public:
    gm_cpplib() {
        main = NULL;
    }
    gm_cpplib(gm_cpp_gen* gen) {
        main = gen;
    }
    void set_main(gm_cpp_gen* gen) {
        main = gen;
    }

    virtual const char* get_header_info() {
        return "gm.h";
    }
    virtual const char* get_type_string(ast_typedecl* t);
    virtual const char* get_type_string(int prim_type);

    virtual const char* max_node_index(ast_id* graph);
    virtual const char* max_edge_index(ast_id* graph);
    virtual const char* node_index(ast_id* iter);
    virtual const char* edge_index(ast_id* iter);

    virtual bool do_local_optimize();

    virtual void generate_sent_nop(ast_nop* n);
    virtual void generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body);
    virtual void generate_expr_nil(ast_expr* e, gm_code_writer& Body);
    virtual bool add_collection_def(ast_id* set);
    virtual void add_map_def(ast_maptypedecl* map, ast_id* mapId);
    virtual void build_up_language_voca(gm_vocabulary& V);

    virtual bool need_up_initializer(ast_foreach* fe);
    virtual bool need_down_initializer(ast_foreach* fe);
    virtual void generate_up_initializer(ast_foreach* fe, gm_code_writer& Body);
    virtual void generate_down_initializer(ast_foreach* fe, gm_code_writer& Body);
    virtual void generate_foreach_header(ast_foreach* fe, gm_code_writer& Body);

    const char* get_reduction_function_name(GM_REDUCE_T type);

protected:
    gm_cpp_gen* get_main() {return main;}

private:
    //map sizes
    static const int SMALL = 0;
    static const int MEDIUM = 1;
    static const int LARGE = 2;

    char str_buf[1024 * 8];
    gm_cpp_gen* main;

    virtual void generate_expr_builtin_field(ast_expr_builtin_field* builtinExpr, gm_code_writer& body);
    const char* get_function_name_graph(int methodId);
    const char* get_function_name_nset(int methodId, bool in_parallel = false);
    const char* get_function_name_nseq(int methodId);
    const char* get_function_name_norder(int methodId);
    const char* get_function_name_map(int methodId, bool in_parallel = false);
    const char* get_function_name_map_seq(int methodId);
    const char* get_function_name_map_par(int methodId);
    void add_arguments_and_thread(gm_code_writer& body, ast_expr_builtin* builtinExpr, bool addThreadId);
    const char* getMapDefaultValueForType(int type);
    const char* getMapTypeString(int mapType);
    const char* getAdditionalMapParameters(int mapType);

    static const char* get_primitive_type_string(int type_id) {
        switch (type_id) {
            case GMTYPE_BYTE:
                return "int8_t";
            case GMTYPE_SHORT:
                return "int16_t";
            case GMTYPE_INT:
                return "int32_t";
            case GMTYPE_LONG:
                return "int64_t";
            case GMTYPE_FLOAT:
                return "float";
            case GMTYPE_DOUBLE:
                return "double";
            case GMTYPE_BOOL:
                return "bool";
            default:
                assert(false);
                return "??";
        }
    }

    static const char* getTypeString(int type) {
        if (gm_is_prim_type(type)) {
            return get_primitive_type_string(type);
        } else if (gm_is_node_type(type)) {
            return NODE_T;
        } else if (gm_is_edge_type(type)) {
            return EDGE_T;
        } else {
            assert(false);
        }
        return NULL;
    }


};

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_cpp_gen: public gm_backend, public gm_code_generator
{
    friend class nop_reduce_scalar;
public:
    gm_cpp_gen() :
            gm_code_generator(Body), fname(NULL), dname(NULL), f_header(NULL), f_body(NULL), _target_omp(false), _pblock(false) {
        f_shell = NULL;
        glib = new gm_cpplib(this);
        init();
    }
    gm_cpp_gen(gm_cpplib* l) :
            gm_code_generator(Body), fname(NULL), dname(NULL), f_header(NULL), f_body(NULL), _target_omp(false), _pblock(false) {
        assert(l != NULL);
        glib = l;
        glib->set_main(this);
        init();
    }
protected:
    void init() {
        init_opt_steps();
        init_gen_steps();
        build_up_language_voca();
    }

public:
    virtual ~gm_cpp_gen() {
        close_output_files();
    }
    virtual void setTargetDir(const char* dname);
    virtual void setFileName(const char* fname);

    virtual bool do_local_optimize_lib();
    virtual bool do_local_optimize();
    virtual bool do_generate();
    virtual void do_generate_begin();
    virtual void do_generate_end();

    /*
     */

protected:
    std::list<gm_compile_step*> opt_steps;
    std::list<gm_compile_step*> gen_steps;

    virtual void build_up_language_voca();
    virtual void init_opt_steps();
    virtual void init_gen_steps();

    virtual void prepare_parallel_for(bool b);
    int _ptr, _indent;

public:
    gm_cpplib* get_lib() {
        return glib;
    }

    //std::list<const char*> local_names;

public:
    virtual void set_target_omp(bool b) {
        _target_omp = b;
    }
    virtual bool is_target_omp() {
        return _target_omp;
    }

protected:
    // data structure for generation
    char *fname;        // current source file (without extension)
    char *dname;        // output directory

    gm_code_writer Header;
    gm_code_writer Body;
    FILE *f_header;
    FILE *f_body;
    FILE *f_shell;
    bool open_output_files();
    void close_output_files(bool remove_files=false);
    void remove_output_files();
    void do_generate_compile_shell(std::map<std::string,std::string>& setup);
    void do_generate_user_main();

    bool _target_omp;
    gm_cpplib* glib; // graph library

    // some common sentence
    virtual void add_include(const char* str1, gm_code_writer& Out, bool is_clib = true, const char* str2 = "");
    virtual void add_ifdef_protection(const char* str);

    //------------------------------------------------------------------------------
    // Generate Method from gm_code_generator
    //------------------------------------------------------------------------------
public:
    virtual void generate_rhs_id(ast_id* i);
    virtual void generate_rhs_field(ast_field* i);
    virtual void generate_expr_builtin(ast_expr* e);
    virtual void generate_expr_minmax(ast_expr* e);
    virtual void generate_expr_abs(ast_expr* e);
    virtual void generate_expr_inf(ast_expr* e);
    virtual void generate_expr_nil(ast_expr* e);

    virtual const char* get_type_string(ast_typedecl* t);
    virtual const char* get_type_string(int prim_type);
    virtual void generate_lhs_id(ast_id* i);
    virtual void generate_lhs_field(ast_field* i);
    virtual void generate_sent_nop(ast_nop* n);
    virtual void generate_sent_reduce_assign(ast_assign *a);
    virtual void generate_sent_defer_assign(ast_assign *a) {
        assert(false);
    } // should not be here
    virtual void generate_sent_vardecl(ast_vardecl *a);
    virtual void generate_sent_foreach(ast_foreach *a);
    virtual void generate_sent_bfs(ast_bfs* b);
    virtual void generate_sent_block(ast_sentblock *b);
    virtual void generate_sent_block(ast_sentblock* b, bool need_br);
    virtual void generate_sent_return(ast_return *r);
    virtual void generate_sent_call(ast_call* c);
    virtual void generate_sent_assign(ast_assign* a);
    virtual const char* get_function_name_map_reduce_assign(int reduceType);

    virtual void generate_sent_block_enter(ast_sentblock *b);
    virtual void generate_sent_block_exit(ast_sentblock* b);

    virtual void generate_idlist(ast_idlist *i);
    virtual void generate_proc(ast_procdef* proc);
    void generate_proc_decl(ast_procdef* proc, bool is_body_file);

protected:
    bool is_under_parallel_sentblock() {
        return _pblock;
    }
    void set_under_parallel_sentblock(bool b) {
        _pblock = b;
    }

    virtual void declare_prop_def(ast_typedecl* t, ast_id* i);
    virtual void generate_sent_reduce_argmin_assign(ast_assign *a);

    bool _pblock;

protected:
    void generate_bfs_def(ast_bfs* bfs);
    void generate_bfs_body_fw(ast_bfs* bfs);
    void generate_bfs_body_bw(ast_bfs* bfs);
    void generate_bfs_navigator(ast_bfs* bfs);

    const char* i_temp;  // temporary variable name
    char temp[2048];

private:
    const char* get_function_name(int methodId, bool& addThreadId);
    void generate_idlist_primitive(ast_idlist* idList);
    void generate_lhs_default(int type);
    void generate_sent_map_assign(ast_assign_mapentry* a);
};

extern gm_cpp_gen CPP_BE;

//---------------------------------------------------
// (NOPS) for CPP/CPP_LIB
//---------------------------------------------------
enum nop_enum_cpp
{
    NOP_REDUCE_SCALAR = 1000,
};

class nop_reduce_scalar: public ast_nop
{
public:
    nop_reduce_scalar() :
            ast_nop(NOP_REDUCE_SCALAR) {
    }
    void set_symbols(std::list<gm_symtab_entry*>& O, std::list<gm_symtab_entry*>& N, std::list<int>& R, std::list<std::list<gm_symtab_entry*> >& O_S,
            std::list<std::list<gm_symtab_entry*> >& N_S);

    virtual bool do_rw_analysis();
    void generate(gm_cpp_gen* gen);

public:
    std::list<gm_symtab_entry*> old_s;
    std::list<gm_symtab_entry*> new_s;
    std::list<int> reduce_op;
    std::list<std::list<gm_symtab_entry*> > old_supple; // supplimental lhs for argmin/argmax
    std::list<std::list<gm_symtab_entry*> > new_supple;
};

//-----------------------------------
// define labels, which is nothing but a string 
//-----------------------------------
DEF_STRING(LABEL_PAR_SCOPE);
DEF_STRING(CPPBE_INFO_HAS_BFS);
DEF_STRING(CPPBE_INFO_IS_PROC_ENTRY);
DEF_STRING(CPPBE_INFO_HAS_PROPDECL);
DEF_STRING(CPPBE_INFO_BFS_SYMBOLS);
DEF_STRING(CPPBE_INFO_BFS_NAME);
DEF_STRING(CPPBE_INFO_BFS_LIST);
DEF_STRING(CPPBE_INFO_COLLECTION_LIST);
DEF_STRING(CPPBE_INFO_COLLECTION_ITERATOR);
DEF_STRING(CPPBE_INFO_COMMON_NBR_ITERATOR);
DEF_STRING(CPPBE_INFO_NEIGHBOR_ITERATOR);
DEF_STRING(CPPBE_INFO_USE_REVERSE_EDGE);
DEF_STRING(CPPBE_INFO_USE_DOWN_NBR);
DEF_STRING(CPPBE_INFO_NEED_SEMI_SORT);
DEF_STRING(CPPBE_INFO_NEED_FROM_INFO);

//----------------------------------------
// For runtime
//----------------------------------------
static const char* MAX_THREADS = "gm_rt_get_num_threads";
static const char* THREAD_ID = "gm_rt_thread_id";
static const char* ALLOCATE_BOOL = "gm_rt_allocate_bool";
static const char* ALLOCATE_LONG = "gm_rt_allocate_long";
static const char* ALLOCATE_INT = "gm_rt_allocate_int";
static const char* ALLOCATE_DOUBLE = "gm_rt_allocate_double";
static const char* ALLOCATE_FLOAT = "gm_rt_allocate_float";
static const char* ALLOCATE_NODE = "gm_rt_allocate_node_t";
static const char* ALLOCATE_EDGE = "gm_rt_allocate_edge_t";
static const char* ALLOCATE_COLLECTION = "gm_rt_allocate_collection";
static const char* DEALLOCATE = "gm_rt_deallocate";
static const char* CLEANUP_PTR = "gm_rt_cleanup";
static const char* RT_INIT = "gm_rt_initialize";
static const char* BFS_TEMPLATE = "gm_bfs_template";
static const char* DFS_TEMPLATE = "gm_dfs_template";
static const char* DO_BFS_FORWARD = "do_bfs_forward";
static const char* DO_BFS_REVERSE = "do_bfs_reverse";
static const char* DO_DFS = "do_dfs";
static const char* RT_INCLUDE = "gm.h";
static const char* PREPARE = "prepare";
static const char* FREEZE = "freeze";
static const char* MAKE_REVERSE = "make_reverse_edges";
static const char* SEMI_SORT = "do_semi_sort";
static const char* IS_SEMI_SORTED = "is_semi_sorted";
static const char* PREPARE_FROM_INFO = "prepare_edge_source";

#endif
