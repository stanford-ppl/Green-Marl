

#include <assert.h>
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_misc.h"

struct gm_sym_change_info {
  bool change_lhs;
  bool change_rhs;
  bool src_scalar;
  bool tgt_scalar;
  gm_symtab_entry* src;
  gm_symtab_entry* s_drv;
  gm_symtab_entry* tgt;
  gm_symtab_entry* t_drv;
};

class gm_replace_symbol_access_t : public gm_apply
{
public:
    gm_replace_symbol_access_t(gm_sym_change_info& I) : INFO(I)
    {
        set_for_sent(I.change_lhs);
        set_for_expr(I.change_rhs);
        changed = false;
    }

private:
    ast_id* make_replace_id(ast_id* src) 
    {
        ast_id* new_id = INFO.tgt->getId()->copy(true);
        new_id ->set_line (src->get_line());
        new_id ->set_col  (src->get_col());
        return new_id;
    }
    ast_id* make_replace_id(ast_field* src) 
    {
        ast_id* new_id = INFO.tgt->getId()->copy(true);
        new_id ->set_line (src->get_line());
        new_id ->set_col  (src->get_col());
        return new_id;
    }
    ast_field* make_replace_field(ast_id* src) 
    {
        ast_id* new_id1 = INFO.t_drv->getId()->copy(true);
        ast_id* new_id2 = INFO.tgt->getId()->copy(true);
        ast_field* f = ast_field::new_field(new_id1, new_id2);
        new_id1 ->set_line (src->get_line());
        new_id1 ->set_col  (src->get_col());
        new_id2 ->set_line (src->get_line());
        new_id2 ->set_col  (src->get_col());
        f->set_line(new_id1->get_line());
        f->set_col(new_id1->get_col());

        return f;
    }
    ast_field* make_replace_field(ast_field* src) 
    {
        ast_id* new_id1 = INFO.t_drv->getId()->copy(true);
        ast_id* new_id2 = INFO.tgt->getId()->copy(true);
        ast_field* f = ast_field::new_field(new_id1, new_id2);
        new_id1 ->set_line (src->get_first()->get_line());
        new_id1 ->set_col  (src->get_first()->get_col());
        new_id2 ->set_line (src->get_second()->get_line());
        new_id2 ->set_col  (src->get_second()->get_col());
        f->set_line(src->get_line());
        f->set_col(src->get_col());

        return f;
    }
    ast_node* make_replace(ast_id* src) {
        if (INFO.tgt_scalar) return make_replace_field(src);
        else return make_replace_id(src);
    }
    ast_node* make_replace(ast_field* src) {
        if (INFO.tgt_scalar) return make_replace_field(src);
        else return make_replace_id(src);
    }

public:
    bool apply(ast_expr* e)
    {
        ast_id* new_id;
        if (e->is_id() && (INFO.src_scalar)) {
            ast_id* i = e->get_id();
            if (i->getSymInfo() == INFO.src) {
                if (INFO.tgt_scalar) {
                    e->set_id(make_replace_id(i));
                } else {
                    e->set_id(NULL);
                    e->set_field(make_replace_field(i));
                }
                changed = true;
                delete i;
            }
        }
        else if (e->is_field() && (!INFO.src_scalar)) {
            ast_field* f = e->get_field();
            if ((f->get_first()->getSymInfo() == INFO.s_drv) &&
                (f->get_second()->getSymInfo() == INFO.src))
            {
                if (INFO.tgt_scalar) {
                    e->set_field(NULL);
                    e->set_id(make_replace_id(f));
                } else {
                    e->set_field(make_replace_field(f));
                }
                changed = true;
                delete f;
            }
        }
        return true;
    }


    // LHS changing
    bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_ASSIGN)
        {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar() && INFO.src_scalar)
            {
               if (a->get_lhs_scala()->getSymInfo() == INFO.src)
               {
                    ast_id* old_id = a->get_lhs_scala();
                    if (INFO.tgt_scalar) 
                    {
                        a->set_lhs_scala(make_replace_id(old_id));
                    }
                    else 
                    {
                        a->set_lhs_scala(NULL);
                        a->set_lhs_field(make_replace_field(old_id));
                    }

                    changed = true;
                    delete old_id;
                }
            }
            else if (!a->is_target_scalar() && !INFO.src_scalar)
            {
                ast_field* old_f = a->get_lhs_field();
                if ((old_f->get_first()->getSymInfo()  == INFO.s_drv) && 
                    (old_f->get_second()->getSymInfo() == INFO.src)) 
                {
                    if (INFO.tgt_scalar)
                    {
                        a->set_lhs_field(NULL);
                        a->set_lhs_scala(make_replace_id(old_f));
                    }
                    else
                    {
                        a->set_lhs_field(make_replace_field(old_f));
                    }
                    changed = true;
                    delete old_f;
                }
            }

            // lhs list
            if (a->has_lhs_list()) 
            {
                std::list<ast_node*>& lhs_list = a->get_lhs_list();
                std::list<ast_node*>::iterator I;
                std::list<std::list<ast_node*>::iterator> to_be_removed ;
                for(I=lhs_list.begin(); I!=lhs_list.end(); I++) 
                {
                    ast_node* n = *I;
                    if (n->get_nodetype() == AST_ID) 
                    {
                        ast_id* id = (ast_id*) n;
                        if (id->getSymInfo() == INFO.src) {

                            // insert new lhs in front of this one
                            ast_node* new_target = make_replace(id); 
                            lhs_list.insert(I, new_target);

                            to_be_removed.push_back(I);
                        }
                    }
                    else if (n->get_nodetype() == AST_FIELD) 
                    {
                        ast_field* f = (ast_field*) n;
                        if ((f->get_first()->getSymInfo() == INFO.s_drv) &&
                            (f->get_second()->getSymInfo() == INFO.src)) {
                            // insert new lhs in front of this one
                            ast_node* new_target = make_replace(f); 
                            lhs_list.insert(I, new_target);

                            to_be_removed.push_back(I);
                        }
                    }
                    else {assert(false);}
                }

                std::list<std::list<ast_node*>::iterator>::iterator J;
                for(J=to_be_removed.begin(); J!=to_be_removed.end(); J++)
                {
                    std::list<ast_node*>::iterator I = *J;
                    ast_node* n = *I;
                    lhs_list.erase(I);
                    delete n;
                }
            }

        }
        return true;

        // TODO: procedure call
    }
    

    bool is_changed() {return changed;}
private:
    gm_sym_change_info& INFO;
    bool changed;

};



static bool replace_symbol_access(ast_node* top, gm_sym_change_info& I)
{
    // traverse the nodes
    //   replace every instance of symbol access
    gm_replace_symbol_access_t T(I);
    top->traverse_pre(&T);
    
    return T.is_changed();
}


bool gm_replace_symbol_access_scalar_scalar(ast_node* top, gm_symtab_entry* src, gm_symtab_entry* target, bool change_rhs, bool change_lhs) {
    gm_sym_change_info S;
    S.change_lhs = change_lhs;
    S.change_rhs = change_rhs;
    S.src_scalar = true;
    S.tgt_scalar = true;
    S.src = src;
    S.tgt = target;
    return replace_symbol_access(top, S);
}

bool gm_replace_symbol_access_scalar_field(ast_node* top, gm_symtab_entry* src, gm_symtab_entry* t_drv, gm_symtab_entry* target, bool change_rhs, bool change_lhs) 
{
    gm_sym_change_info S;
    S.change_lhs = change_lhs;
    S.change_rhs = change_rhs;
    S.src_scalar = true;
    S.tgt_scalar = false;
    S.src = src;
    S.tgt = target;
    S.t_drv = t_drv;
    return replace_symbol_access(top, S);
}
bool gm_replace_symbol_access_field_scalar(ast_node* top, gm_symtab_entry* src_drv, gm_symtab_entry* src, gm_symtab_entry* target, bool change_rhs, bool change_lhs) 
{
    gm_sym_change_info S;
    S.change_lhs = change_lhs;
    S.change_rhs = change_rhs;
    S.src_scalar = false;
    S.tgt_scalar = true;
    S.src = src;
    S.s_drv = src_drv;
    S.tgt = target;
    return replace_symbol_access(top, S);
}
bool gm_replace_symbol_access_field_field(ast_node* top, gm_symtab_entry* src_drv, gm_symtab_entry* src, gm_symtab_entry* t_drv, gm_symtab_entry* target, bool change_rhs, bool change_lhs) 
{
    gm_sym_change_info S;
    S.change_lhs = change_lhs;
    S.change_rhs = change_rhs;
    S.src_scalar = true;
    S.tgt_scalar = true;
    S.src = src;
    S.s_drv = src_drv;
    S.tgt = target;
    S.t_drv = t_drv;

    return replace_symbol_access(top, S);
}

