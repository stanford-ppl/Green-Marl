#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"

//---------------------------------------------------------
// Merge two sentence blocks (P, Q) into one. (P)
// [Assumption: the two sentblocks are siblings]
//   - All the sentences in Q is moved to P.
//   - Name conflicts are resolved ==> should not happen (see handle_vardecl)
//   - Symbol table is kept valid.
//   - (RW analysis is re-done here?)
//---------------------------------------------------------
/*
*/

void gm_merge_sentblock(ast_sentblock* P, ast_sentblock *Q, bool delete_Q_after)
{
    //(assumption) type-checking is already done
    //(assumption) var-decl has been hoisted up.

    assert(P!=NULL);
    assert(Q!=NULL);

    // 1. resolve name conflict in each direction
    //resolve_name_conflict(P,Q);
    //resolve_name_conflict(Q,P);

    // 2. merge symbol tables
    gm_symtab* V = P->get_symtab_var();
    gm_symtab* F = P->get_symtab_field();
    gm_symtab* R = P->get_symtab_proc();
    V->merge(Q->get_symtab_var());
    F->merge(Q->get_symtab_field());
    R->merge(Q->get_symtab_proc());

    // 3. move sentence one-by-one
    // (3.1) keep decls at the highest position
    // (3.2) other sentneces at the bottem
    ast_sent* anchor = NULL;
    std::list<ast_sent*>& Ps = P->get_sents();
    std::list<ast_sent*>::iterator it;
    for(it= Ps.begin(); it!=Ps.end();it++) {
        ast_sent*s = *it;
        if (s->get_nodetype() != AST_VARDECL) break;  // stop at the first non-decl sentence
        anchor = s;
    }

    std::list<ast_sent*> Qs = Q->get_sents(); // work on a copy
    for(it = Qs.begin(); it!= Qs.end(); it++) {
        ast_sent*s = *it;
        gm_ripoff_sent(s);
        if (s->get_nodetype() == AST_VARDECL) {
            if (anchor == NULL)
                gm_insert_sent_begin_of_sb(P, s);
            else
                gm_add_sent_after(anchor, s);

            anchor = s;
        } else {
            gm_insert_sent_end_of_sb(P, s);
        }
    }

    if (delete_Q_after) 
        delete Q;
}
