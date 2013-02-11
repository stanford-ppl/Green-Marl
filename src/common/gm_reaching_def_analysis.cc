#include "gm_ast.h"
#include "gm_transform_helper.h"

//---------------------------------------------------------------------------------------
// Create a map between
//     <sentence, symbol>  ==> set <reaching definitions>
//
// sym is scalar variables only
//
//     reaching def is either
//         ast_procdef (input argument)
//         ast_assign  (reduction target)
//         ast_expr    (any other assignment)
//         ast_bfs/foreach (iterator)
//
//---------------------------------------------------------------------------------------

static void append_all_reaching_defs_to_map(gm_rda_map& R, ast_sent *s, gm_symtab_entry* sym, gm_rda_set& all_reaching_defs)
{
    if (all_reaching_defs.size() == 0) return ;

    gm_rda_key key(sym, s);
    if (R.find(key) == R.end())
    {
        // insert empty set first
        R[key] = all_reaching_defs; // copy
    }
    else {
        // add elements one by one
        gm_rda_set &S = R[key];
        gm_rda_set::iterator I;
        for(I=all_reaching_defs.begin(); I!=all_reaching_defs.end(); I++)
        {
            S.insert(*I);
        }
    }
}


// reaching_context is all the reaching defs to all the symbols at some program point
typedef std::map<gm_symtab_entry*, gm_rda_set> reaching_context_t;


// add a def expression the context (kill previous def)n
// def_node is either (1) ast_procdef (for input arguments)
// (2) ast_expr (for scalar)
// (3) ast_foreach/ast_bfs (for iterators)
static void add_def(reaching_context_t& C, gm_symtab_entry *e, ast_node* def_node)
{
    if (C.find(e) == C.end())
    {
        gm_rda_set EMPTY;
        C[e] = EMPTY;   // copy
    }

    C.find(e)->second.clear();      // kill
    C.find(e)->second.insert(def_node);
}

static void add_all_defs_into_context(reaching_context_t& C, gm_defined_symbol_map& DI, ast_node *n)
{
    // find all local defs in this sentence
    if (DI.find(n) != DI.end()) {
        std::vector<gm_symbol_def_t> &V = DI[n];
        std::vector<gm_symbol_def_t>::iterator v;
        // add them into context
        for (v=V.begin(); v!=V.end(); v++)
        {
            gm_symbol_def_t def = *v;
            gm_symtab_entry* sym = def.first;
            ast_node* def_node = def.second;
            add_def(C, sym, def_node);
        }
    }
}

static void remove_symbol_from_context(reaching_context_t& C, gm_symtab_entry *e)
{
    if (C.find(e) != C.end())
    {
        C.erase(e);
    }
}


static void register_all_reaching_defs_to_map(reaching_context_t& C, 
        gm_used_symbol_map& UI, gm_rda_map& R, ast_sent*s)
{

    // check if there is any used symbol
    if (UI.find(s) != UI.end())
    {
        // for all the used symbols
        std::set<gm_symtab_entry*>& S = UI[s];
        std::set<gm_symtab_entry*>::iterator I;
        for (I=S.begin(); I!=S.end(); I++)
        {
            gm_symtab_entry* symbol = *I;

            // has any reaching defs?
            if (C.find(symbol) != C.end())
            {
                gm_rda_set& all_reaching_defs = C[symbol];

                append_all_reaching_defs_to_map(R, s, symbol, all_reaching_defs);
            }
        }
    }
}


extern void gm_flush_reproduce(); 
static void dump_context(reaching_context_t& C)
{
// for debug
    printf("==============================\n");
    std::map<gm_symtab_entry*, gm_rda_set>::iterator I;
    for(I=C.begin(); I!=C.end(); I++)
    {
        gm_symtab_entry* s = I->first;
        gm_rda_set &S = I->second;
        gm_rda_set::iterator J;
        printf("%s => ", s->getId()->get_genname());
        for(J=S.begin(); J!=S.end(); J++)
        {
            ast_node* n = *J;
            printf("[");
            n->reproduce(-1);
            gm_flush_reproduce();
            printf("]");
        } 
        printf("\n");
    }
    printf("==============================\n");
}


// add C2 into C1
static void merge_contexts(reaching_context_t& C1, reaching_context_t& C2)
{

    reaching_context_t::iterator I;
    for(I=C2.begin(); I!=C2.end(); I++)
    {
        gm_symtab_entry* sym = I->first;
        gm_rda_set& S = I->second;

        if (C1.find(sym) == C1.end())
        {
            // sym is defined in C2 only
            C1[sym] = S;  // copy
        }
        else 
        {
            // insert all defs of C2 set, into C1 set
            gm_rda_set& S2 = C1.find(sym)->second;
            gm_rda_set::iterator J;
            for(J=S.begin(); J!=S.end(); J++)
            {
                S2.insert(*J);
            }
        }
    }
}
static void overwrite_contexts(reaching_context_t& C1, reaching_context_t& C2)
{

    reaching_context_t::iterator I;
    for(I=C2.begin(); I!=C2.end(); I++)
    {
        gm_symtab_entry* sym = I->first;
        gm_rda_set& S = I->second;

        C1[sym] = S;  // copy and overwrite
    }
}




//-----------------------------------------------------------------------------------
// Main traversal for reaching definition analysis
// [Pre apply] (sentence)
//     (1) get current reaching-def context 
//     (2) Add all reaching-defs to the final rda_map
//     (3) Modify current context
//     (4) (IF/BFS/DFS/WHILE/FOREACH) fork current context 
//     (5) continue recurse
//
//  [Post apply] (sentence)
//     (1) (IF/BFS/DFS/WHILE/FOREACH) get forked context 
//     (2) (DFS/BFS/WHILE/FOREACH) do re-flow
//     (3)  merge to current context
//-----------------------------------------------------------------------------------

class gm_reaching_def_traverse : public gm_apply
{
    public:
        gm_reaching_def_traverse(gm_used_symbol_map& _UI, gm_defined_symbol_map& _DI, gm_rda_map& _R, bool _is_reflow = false) : 
            UI(_UI), DI(_DI), R(_R), is_reflow(_is_reflow)
        {
            set_for_proc(true);
            set_for_sent(true);
            set_separate_post_apply(true);
            curr_context = NULL;
        }

        virtual ~gm_reaching_def_traverse()
        {

            std::set<ast_sent*>::iterator v;
            for (v=saved_reaching_context_map_is_a_copy.begin(); 
                    v!= saved_reaching_context_map_is_a_copy.end(); v++)
            {
                ast_sent* s = *v;
                reaching_context_t* t = saved_reaching_context_map[s];
                assert (t!=NULL);
                delete t;
            }
        }


        bool apply(ast_procdef* p) 
        {
            reaching_context_t INIT;

            // create initial context (arguments)
            add_all_defs_into_context(INIT, DI, p);

            // fork new context with copy
            set_context_for(p->get_body(),&INIT, true); // create a copy
        }

        // pre visit
        bool apply(ast_sent* s)
        {
            reaching_context_t* C = get_context_for(s);
            set_curr_context(C);
            assert(C!=NULL);

            if ((s->get_nodetype() == AST_FOREACH) || 
                    (s->get_nodetype() == AST_BFS)) 
            {
                // insert iterator definition into the context first
                add_all_defs_into_context(*C, DI, s);
            }

            // register all the reaching defs (for all the used symbols)
            // to the result map
            register_all_reaching_defs_to_map(*C, UI, R, s);

            // now prepare for forking context
            if (s->get_nodetype() == AST_IF)
            {
                ast_if* iff = (ast_if*) s;
                previsit_if(iff);
            }
            else if (s->get_nodetype() == AST_WHILE)
            {
                ast_while* w = (ast_while*) s;
                previsit_while(w);
            }
            else if (s->get_nodetype() == AST_FOREACH)
            {
                ast_foreach* f = (ast_foreach*) s;
                previsit_foreach(f);
            }
            else if (s->get_nodetype() == AST_BFS)
            {
                ast_bfs* b = (ast_bfs*) s;
                if (b->is_bfs())
                {
                    previsit_bfs(b);
                }
                else
                {
                    previsit_dfs(b);
                }
            }

            return true;
        }


        // post visit
        bool apply2(ast_sent* s)
        {
            reaching_context_t* C = get_context_for(s);
            set_curr_context(C);

            if ((s->get_nodetype() != AST_FOREACH) &&
                    (s->get_nodetype() != AST_BFS)) 
            {
                // insert modification into context
                // (kill some defs)
                add_all_defs_into_context(*C, DI, s);
            }

            // Now merge forked context
            if (s->get_nodetype() == AST_IF)
            {
                ast_if* iff = (ast_if*) s;
                postvisit_if(iff);
            }
            else if (s->get_nodetype() == AST_WHILE)
            {
                ast_while* w = (ast_while*) s;
                postvisit_while(w);
            }
            else if (s->get_nodetype() == AST_FOREACH)
            {
                ast_foreach* f = (ast_foreach*) s;
                postvisit_foreach(f);
            }
            else if (s->get_nodetype() == AST_BFS)
            {
                ast_bfs* b = (ast_bfs*) s;
                if (b->is_bfs())
                {
                    postvisit_bfs(b);
                }
                else
                {
                    postvisit_dfs(b);
                }
            }

            return true;
        }

    private:
        void previsit_if(ast_if* iff)
        {
            reaching_context_t* C = get_curr_context();
            ast_sent* then_part = iff->get_then();
            ast_sent* else_part = iff->get_else();

            // for then part, fork a new context
            set_context_for(iff, C);      // save my context
            set_context_for(then_part, C, true); // create a copy

            // for else part, simply use current context (without copying)
            if (else_part != NULL) {
                set_context_for(else_part, C);
            }
        }
        void postvisit_if(ast_if* iff)
        {
            // merge current context (==else context) with then-part context
            reaching_context_t* C = get_context_for(iff);
            ast_sent* then_part = iff->get_then();

            reaching_context_t* C1 = get_context_for(then_part);
            assert(C!=C1);

            // merge C1 into C
            merge_contexts(*C, *C1);
        }

        void previsit_while(ast_while* w)
        {
            reaching_context_t* C = get_curr_context();
            ast_sent* body = w->get_body();
            set_context_for(w, C); // my context
            set_context_for(body, C, true); // body's context
        }
        void postvisit_while(ast_while* w)
        {
            reaching_context_t* C = get_context_for(w);
            ast_sent* body = w->get_body();
            reaching_context_t* C1 = get_context_for(body);

            // reflow C1 context to body (unless currenly reflowing)
            if (!is_reflow)
            {
                augment(w, C1, true);
                // C1 should be updated now
            }

            // now merge two context
            merge_contexts(*C, *C1);
        }

        void previsit_foreach(ast_foreach* fe)
        {
            // Do same as while.
            // Note: in fact, parallel foreach can have more reaching defs then while. 
            // However, since the specification does not gaurantee anything about such non-deterministic write
            // this analysis simply assumes sequential iteration
            reaching_context_t* C = get_curr_context();
            ast_sent* body = fe->get_body();
            set_context_for(fe, C); // my context
            set_context_for(body, C, true); // body's context
        }
        void postvisit_foreach(ast_foreach* fe)
        {
            reaching_context_t* C = get_context_for(fe);
            ast_sent* body = fe->get_body();
            reaching_context_t* C1 = get_context_for(body);

            // reflow C1 context to body (unless currenly reflowing)
            if (!is_reflow)
            {
                augment(fe, C1, true);
                // C1 should be updated now
            }

            // now merge two context
            merge_contexts(*C, *C1);
        }


        void previsit_bfs(ast_bfs* b)
        {
            reaching_context_t* C = get_context_for(b);
            ast_sent* fbody = b-> get_fbody(); 
            ast_sent* bbody = b-> get_bbody();
            assert (fbody!=NULL);

            set_context_for(b, C); // my context
            set_context_for(fbody, C, true); // copy

            if (bbody != NULL) {
                reaching_context_t T;           // dummy context
                set_context_for(bbody, &T, true); 
            }
        }
        void  postvisit_bfs(ast_bfs* b)
        {
            //--------------------------------
            // y = 0;
            // InBFS(...) { // fw-visit
            //    x = y ;  // possible values? [0, 1]
            //    y = 1;
            // } 
            // InReverse { // reverse-visit
            //    z = y;  // possible values? [1, 2]
            //    y = 2;
            // }
            // w = y;   // possible values? [2]
            //--------------------------------
            ast_sent* f_body = b-> get_fbody(); 
            ast_sent* b_body = b-> get_bbody();

            reaching_context_t* C = get_context_for(b);
            reaching_context_t* C_f = get_context_for(f_body);

            if (!is_reflow) {
                augment(f_body, C_f);  // F -> F
            }
            overwrite_contexts(*C, *C_f);

            if (b_body != NULL) 
            {
                // create a new context
                set_context_for(b_body, C, true); 
                reaching_context_t* C_r = get_context_for(b_body);

                augment(b_body, C_r, true);  // 1st flow  (keep context)

                augment(b_body, C_r);  // 2nd flow

                overwrite_contexts(*C, *C_r); // merge to original context
            }
        }



        void previsit_dfs(ast_bfs* b)
        {
            reaching_context_t* C = get_context_for(b);
            ast_sent* fbody = b-> get_fbody(); 
            ast_sent* bbody = b-> get_bbody();
            assert (fbody!=NULL);
            assert (bbody!=NULL);

            set_context_for(b, C); // my context
            set_context_for(fbody, C, true); // copy

            reaching_context_t T;           // dummy context
            set_context_for(bbody, &T, true); 
        }
        void postvisit_dfs(ast_bfs* b)
        {
            //--------------------------------
            // y = 0;
            // InDFS(...) { // pre-visit
            //    x = y ;  // possible values? [0, 1, 2]
            //    y = 1;
            // } 
            // InPost { // post-visit
            //    z = y;  // possible values? [1, 2]
            //    y = 2;
            // }
            // w = y;   // possible values? [1, 2]
            //--------------------------------
            ast_sent* f_body = b-> get_fbody(); 
            ast_sent* b_body = b-> get_bbody();

            reaching_context_t* C = get_context_for(b);
            reaching_context_t* C_f = get_context_for(f_body);
            reaching_context_t* C_r = get_context_for(b_body); // currently dummy

            // compute C_fr
            set_context_for(b_body, C_f, true);
            gm_reaching_def_traverse T(UI, DI, R); // do a no-reflow version
            T.set_context_for(b_body, C_f, true);
            b_body->traverse_both(&T);
            reaching_context_t* C_fr = T.get_context_for(b_body); 

            // final result is C_fr 
            overwrite_contexts(*C, *C_fr); 

            //if (b->get_navigator() == NULL) {
            //    overwrite_contexts(*C, *C_fr); 
            //}
            //else {
            //  merge_contexts(*C, *C_fr);
            //}

            // augment 
            if (!is_reflow) {
                //dump_context(*C_f);
                //dump_context(*C_fr);
                augment(f_body, C_f);  // F -> F
                augment(f_body, C_fr); // F -> R -> F 
                augment(b_body, C_fr); // F -> R -> R 
            }
        }

        void augment(ast_sent* n, reaching_context_t* T, bool keep_context=false)
        {
            gm_reaching_def_traverse X(UI, DI, R, true); // reflow version
            X.set_context_for(n, T, !keep_context); // create copy, unless (keep_context) is on
            n->traverse_both(&X);

        }
public:
        // end of static scope
        void end_context(ast_node* n)
        {
            if (!n->is_sentence()) return;

            ast_sent* s = (ast_sent*) n;

            reaching_context_t* C = get_context_for(s);

            // remove all the scoped-out variables from the context
            std::set<gm_symtab_entry*>& E = s->get_symtab_var()->get_entries();
            std::set<gm_symtab_entry*>::iterator I;
            for(I=E.begin(); I!=E.end(); I++)
            {
                gm_symtab_entry *e = *I;
                remove_symbol_from_context(*C, e);
                
            }
        }


    private:
        gm_used_symbol_map& UI;
        gm_defined_symbol_map& DI;
        gm_rda_map& R;
        bool is_reflow;
        std::map<ast_sent*, reaching_context_t*> saved_reaching_context_map;
        std::set<ast_sent*> saved_reaching_context_map_is_a_copy;
        reaching_context_t* curr_context;

    public:
        void set_context_for(ast_sent* s, reaching_context_t* RC, bool need_copy=false)
        {

            // save context of reaching defs
            reaching_context_t* RC2;
            if (need_copy)
            {
                if (saved_reaching_context_map_is_a_copy.find(s) != saved_reaching_context_map_is_a_copy.end())
                {
                    delete saved_reaching_context_map[s]; 
                }
                RC2 = new reaching_context_t();
                *RC2 = *RC; // copy
                saved_reaching_context_map_is_a_copy.insert(s);
            }
            else {
                RC2 = RC;
            }
            saved_reaching_context_map[s] = RC2;
        }

        reaching_context_t* get_context_for(ast_sent* s) 
        {
            if (saved_reaching_context_map.find(s) == saved_reaching_context_map.end())
                return get_curr_context();
            else 
                return saved_reaching_context_map[s];
        }

        void set_curr_context(reaching_context_t* T) {curr_context = T;}
        reaching_context_t* get_curr_context() {return curr_context;} 

};

void gm_do_reaching_def_analysis(ast_procdef* p, gm_rda_map& R)
{
    //---------------------------------------------------
    // As a preparation step, do used_symbol /defined_symbol analysis
    //---------------------------------------------------
    gm_used_symbol_map UI;
    gm_compute_used_symbol_info(p, UI);

    gm_defined_symbol_map DI;
    gm_compute_defined_symbol_info(p, DI);

    //---------------------------------------------------
    // Traverse procedure and compute 
    //---------------------------------------------------
    R.clear();
    gm_reaching_def_traverse T(UI, DI, R);
    p->traverse_both(&T);

}







// (Called after analysis)
// find all reaching definitions (set of <expression,bool>, returns NULL if none)
bool gm_has_any_reaching_defs(gm_rda_map& R, ast_id* scalar)
{
    ast_sent* sent = gm_find_parent_sentence(scalar);
    assert (sent!=NULL);

    gm_symtab_entry *sym  = scalar->getSymInfo();
    assert(sym != NULL);

    gm_rda_key K(sym, sent);

    return (R.find(K) == R.end());


}

gm_rda_set& gm_get_all_reaching_defs(gm_rda_map& R, ast_id* scalar)
{
    ast_sent* sent = gm_find_parent_sentence(scalar);
    assert (sent!=NULL);

    gm_symtab_entry *sym  = scalar->getSymInfo();
    assert(sym != NULL);

    gm_rda_key K(sym, sent);

    bool is_empty = (R.find(K) == R.end());
    assert(!is_empty);

    return R.find(K)->second;
}
