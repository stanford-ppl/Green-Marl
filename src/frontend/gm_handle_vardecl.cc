#include "gm_frontend.h"
#include "gm_rw_analysis.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"


class remove_vardecl_t : public gm_apply
{
public:
    // POST Apply
    virtual bool apply(ast_sent* b)
    {
        if (b->get_nodetype() != AST_SENTBLOCK) return true;
        ast_sentblock* sb = (ast_sentblock*) b;
        std::list<ast_sent*> sents = sb->get_sents(); // need a copy
        std::list<ast_sent*> stack;
        std::list<ast_sent*>::iterator i,j;

        //--------------------------------------------
        // 1. find all var-decls
        // 3. delete var-decl
        //--------------------------------------------
        for(i = sents.begin(); i!= sents.end(); i++) 
        {
            ast_sent* z = *i;
            if (z->get_nodetype() != AST_VARDECL)
                continue;
            ast_vardecl* v = (ast_vardecl*) z;

            /*
            */

            stack.push_back(v);
        }

        // 3. delete var-decl
        for(i = stack.begin(); i!= stack.end(); i++) 
        {
            // now delete
            ast_sent* z = *i;
            gm_ripoff_sent(z,false);
            delete z;
        }
        return true;
    }

public:
    void do_removal(ast_procdef* p) 
    {
        set_all(false); set_for_sent(true);
        gm_traverse_sents(p, this, GM_POST_APPLY);
    }
};


//---------------------------------------------------
// rename all potential name conflicts
//---------------------------------------------------
class rename_all_t : public gm_apply
{
public:
    rename_all_t() {
        set_for_symtab(true);
    }

    virtual bool apply(gm_symtab_entry* e, int symtab_type){
        ast_id* id = e->getId();

        // if name is already in -> generate-new-name
        const char* name = id->get_orgname();

        //printf("checking :%s\n",name);
        if (FE.voca_isin((char*)name)) {
            // should use a new name
            const char* new_name = FE.voca_temp_name(name);
            id->set_orgname(new_name); // new name is copied & old name is deleted inside
            delete [] new_name; 
        }

        //printf("adding :%s\n",id->get_orgname());
        // add to vocabulary
        FE.voca_add(id->get_orgname());
        //assert(FE.voca_isin(id->get_orgname()));
        return true;
    }

    //-------------------------------------------------
    // rename all potential name conflicts
    //-------------------------------------------------
    void do_rename_all_potential(ast_procdef * p)
    {
        assert(p == FE.get_current_proc());

        FE.voca_clear();  // rebuild vocaburary
        p->traverse_pre(this);
    }
};


void gm_fe_remove_vardecl::process(ast_procdef* p)
{
    remove_vardecl_t T;
    T.do_removal(p);

    rename_all_t T2;
    T2.do_rename_all_potential(p);

    FE.set_vardecl_removed(true);
}




class restore_vardecl_t : public gm_apply
{
public:
    virtual bool apply(ast_sent* b)
    {
        if (b->get_nodetype() != AST_SENTBLOCK) return true;

        ast_sentblock* sb = (ast_sentblock*) b;
        gm_symtab* V = sb->get_symtab_var();
        gm_symtab* F = sb->get_symtab_field();
        std::set<gm_symtab_entry*> Vs = V->get_entries();
        std::set<gm_symtab_entry*> Fs = F->get_entries();

        ast_sent* top = NULL;
        //-------------------------------------
        // Add vardecls after all 'NOP's
        //-------------------------------------
        std::list<ast_sent*> &sents = sb->get_sents();
        std::list<ast_sent*>::iterator ii;
        for(ii=sents.begin(); ii !=sents.end(); ii++) 
        {
            if ((*ii)->get_nodetype() != AST_NOP)
                break;
            top = *ii;
        }


        //----------------------------------------
        // Iterate over symtab. 
        // Add vardecl for each symbol
        //----------------------------------------
        std::set<gm_symtab_entry*>::iterator i;  
        for(i=Vs.begin(); i!=Vs.end();i++) { // scalar
            gm_symtab_entry* e = *i;
            ast_typedecl* type = e->getType()->copy();
            ast_id* id = e->getId()->copy(true);

            ast_vardecl* v = ast_vardecl::new_vardecl(type, id); 
            if (top == NULL) 
                gm_insert_sent_begin_of_sb(sb, v, GM_NOFIX_SYMTAB);
            else {
                gm_add_sent_after(top, v, GM_NOFIX_SYMTAB);
            }
            top = v;
        }

        for(i=Fs.begin(); i!=Fs.end();i++) { // field
            gm_symtab_entry* e = *i;
            ast_typedecl* type = e->getType()->copy();
            ast_id* id = e->getId()->copy(true);

            ast_vardecl* v = ast_vardecl::new_vardecl(type, id); 
            assert(v->get_idlist()->get_item(0)->getSymInfo()!=NULL);
            if (top == NULL) 
                gm_insert_sent_begin_of_sb(sb, v, GM_NOFIX_SYMTAB);
            else {
                gm_add_sent_after(top, v, GM_NOFIX_SYMTAB);
            }
            top = v;
        }
        return true;
    }

public:
    void do_restore(ast_procdef* p) {
        set_all(false); set_for_sent(true);
        gm_traverse_sents(p, this, GM_POST_APPLY);
    }
};

void gm_fe_restore_vardecl::process(ast_procdef* p)
{
    FE.set_vardecl_removed(false);
    restore_vardecl_t T;
    T.do_restore(p);
}

void gm_frontend::restore_vardecl_all()
{
    std::list<gm_compile_step*> L;
    gm_apply_all_proc(gm_fe_restore_vardecl::get_factory());

}

