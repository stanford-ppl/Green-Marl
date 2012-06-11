#ifndef GIRAPH_SYMINFO
#define GIRAPH_SYMINFO

#include "gm_typecheck.h"

// where the symbol is defined
enum {
    GIRAPH_SCOPE_GLOBAL,
    GIRAPH_SCOPE_OUTER,
    GIRAPH_SCOPE_INNER
};


// how the symbols is used (LHS/RHS/REDUCE) 
enum {
    GIRAPH_SYM_USED_AS_RHS,
    GIRAPH_SYM_USED_AS_LHS,
    GIRAPH_SYM_USED_AS_REDUCE,
};


// where the symbols is used 
enum {
    GIRAPH_CONTEXT_MASTER,
    GIRAPH_CONTEXT_VERTEX,
    GIRAPH_CONTEXT_RECEIVER
};

// symbol usage info
#define GIRAPH_TAG_BB_USAGE            "GIRAPH_TAG_BB"

class giraph_syminfo : public ast_extra_info
{
    public:
        giraph_syminfo(bool sc):
            _scalar(sc), 
            _is_arg(false) ,
            _used_in_multiple_BB (false),  last_BB(-1),  
            used_as_rhs(false), used_as_lhs(false), 
            used_as_reduce(false), 
            used_in_vertex(false), used_in_master(false), 
            used_in_receiver(false), 
            reduce_op_type(GMREDUCE_NULL), start_byte(0),
            scope(GIRAPH_SCOPE_GLOBAL)
            {}
        virtual ~giraph_syminfo() {}

        void set_is_argument(bool b) {_is_arg = b;}
        bool is_argument() {return _is_arg;}
        bool is_scalar() {return _scalar;}
        void add_usage_in_BB(int bb_no, int usage, 
                int context ,  // MASTER, VERTEX, SENDER, RECEIVER
                int reduce_type = GMREDUCE_NULL);

        bool is_used_in_multiple_BB() {return _used_in_multiple_BB;}

        bool is_used_as_rhs() {return used_as_rhs;}
        bool is_used_as_lhs() {return used_as_lhs;}
        bool is_used_as_reduce() {return used_as_reduce;}

        bool is_used_in_vertex() {return used_in_vertex;}
        bool is_used_in_master() {return used_in_master;}
        //bool is_used_in_sender()   {return used_in_sender;}
        bool is_used_in_receiver() {return used_in_receiver;}

        int  get_reduce_type() {return reduce_op_type;}

        // for message/state
        int get_start_byte() {return start_byte;}
        void set_start_byte(int b) {start_byte = b;}

        // where the symbol is defined?
        int get_scope()  {return scope;}
        void set_scope(int s) {scope = s;}

        bool is_scoped_global() {return scope == GIRAPH_SCOPE_GLOBAL;}
        bool is_scoped_outer() {return scope == GIRAPH_SCOPE_OUTER;}
        bool is_scoped_inner() {return scope == GIRAPH_SCOPE_INNER;}

    private:
        bool _used_in_multiple_BB;
        int last_BB;
        bool _scalar;
        bool _is_arg;
        std::vector<int> used_BB;
        std::vector<int> used_type;
        std::vector<int> used_context;
        int scope; // GIRAPH_SCOPE_XX

        bool used_as_rhs;
        bool used_as_lhs;
        bool used_as_reduce;

        bool used_in_vertex;
        bool used_in_master;
        //bool used_in_sender;
        bool used_in_receiver;

        int  reduce_op_type;
        int  start_byte;
};

inline giraph_syminfo* giraph_get_global_syminfo(ast_id* i) {
    return (giraph_syminfo*) i->getSymInfo()->find_info(GIRAPH_TAG_BB_USAGE);
}


#endif
