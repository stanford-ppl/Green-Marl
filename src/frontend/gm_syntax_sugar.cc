#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_misc.h"

//---------------------------------------------------
// syntax sugar elimination, prior to type resolution
//  1. filter of foreach --> if inside foreach
//  2. define and initialization --> define; initialization
//---------------------------------------------------
class gm_ss1_filter : public gm_apply {
    private:
    inline ast_sent* make_if_then(ast_sent* old_body, ast_expr * filter)
    {
        ast_if* new_body = ast_if::new_if(filter, old_body, NULL);
        return new_body;
    }

    public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {

            // if it has a filter, change it into if-then
            ast_foreach* fe = (ast_foreach*) s;
            ast_expr *e = fe->get_filter();
            if (e == NULL) return true;

            ast_sent *old_body = fe->get_body();
            ast_sent* new_body = make_if_then(old_body, e);

            fe->set_filter(NULL);
            fe->set_body(new_body);

            // Note that no symtab is available yet
        }
        else if (s->get_nodetype() == AST_BFS) { // or DFS
            ast_bfs* bfs = (ast_bfs*) s;
            ast_expr* e = bfs->get_filter();
            if (e == NULL) return true;

            // [todo] create backward filter
            ast_sent *old_body = bfs->get_fbody();
            ast_sent* new_body = make_if_then(old_body, e);
            ast_sentblock* sb = ast_sentblock::new_sentblock();
            sb->add_sent(new_body);

            bfs->set_filter(NULL);
            bfs->set_fbody(sb);

            // Note that no symtab is available yet
        }
        return true;
    }

};

class gm_ss1_initial_expr : public gm_apply {
public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_VARDECL)
            return true;
        
        // if it has an initializer, create new sentence
        ast_vardecl *v = (ast_vardecl*) s;
        ast_expr *e = v->get_init();
        if (e == NULL)
            return true;
        v->set_init(NULL);

        // should be single variable definition
        // should be non-field type.
        ast_idlist* idl = v->get_idlist();
        assert(idl->get_length() == 1);
        ast_id* id = idl->get_item(0)->copy();

        // new assign statement
        ast_assign* a = ast_assign::new_assign_scala(id, e, GMASSIGN_NORMAL, NULL, GMREDUCE_NULL);

        // add this sententence next to current statement
        gm_add_sent_after(v, a, false /* no fix symtab */);

        return true;
    }
};

bool gm_frontend::do_syntax_sugar_1(ast_procdef* p)
{
    gm_ss1_filter s1;
    gm_traverse_sents(p, &s1);

    gm_ss1_initial_expr s2;
    gm_traverse_sents(p, &s2);

    return true;
}


