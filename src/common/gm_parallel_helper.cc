
#include "gm_transform_helper.h"
#include "gm_traverse.h"

class check_par_exe_t : public gm_apply {
public:
    check_par_exe_t(bool entry_is_seq) {
        _current_is_par = !entry_is_seq;
        _current_context = NULL;
    }

    //----------------------------------------
    // called sequence for node n
    //   begin_context(n) : AST_PROCDEF, AST_FOREACH, AST_SENTBLOCK, AST_BFS, AST_REDUCE_EXPR
    //   apply(n)
    //     (traverse)
    //   end_context(n)
    //----------------------------------------

    virtual void begin_context(ast_node* n) 
    {
        if (!n->is_sentence()) return;

        // process my self
        ((ast_sent*)n)->set_under_parallel_execution(_current_is_par);

        // save context
        _context.push_back(_current_is_par);

        // begin new context
        if (n->get_nodetype() == AST_BFS) {
            _current_is_par = _current_is_par || ((ast_bfs*)n)->is_parallel();
        } else if (n->get_nodetype() == AST_FOREACH) {
            _current_is_par = _current_is_par || ((ast_foreach*)n)->is_parallel();
        }
        _current_context = n;
    }

    virtual void end_context(ast_node* n) 
    {
        if (!n->is_sentence()) return;
        _current_is_par = _context.back();
        _context.pop_back();
    }

    virtual bool apply(ast_sent* s) 
    {
        if (_current_context == s) return true;
        s->set_under_parallel_execution(_current_is_par);
        return true;
    }
private:
    bool _current_is_par;
    std::list<bool> _context;
    ast_node* _current_context;
    
};

void gm_mark_sents_under_parallel_execution(ast_sent* S, bool entry_is_seq)
{
    check_par_exe_t  T(entry_is_seq);
    gm_traverse_sents(S, &T);
}
