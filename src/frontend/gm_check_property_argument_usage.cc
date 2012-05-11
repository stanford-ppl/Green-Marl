
#include "gm_ast.h"
#include "gm_defs.h"
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

//---------------------------------------------------------------------
// Examination about how each property is used inside a procedure
// : inout vs output
//    If every element is initialized first, and then read it is being used as output
//    Otherwise, it is inout.
// (Do we need this at all?)
//---------------------------------------------------------------------
class gm_check_property_usage_t : public gm_apply {
public:
    gm_check_property_usage_t() {
       topmost_iterator = false;
       set_separate_post_apply(true);
       set_for_sent(true);
       set_for_expr(true);
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (is_under_condition() || is_under_random_iterator() || (topmost_iterator!=NULL)) {
                random_iter_stack.push_back(s);
            }
            else if ((fe->get_iter_type()==GMTYPE_NODEITER_ALL) ||
                     (fe->get_iter_type()==GMTYPE_EDGEITER_ALL)) {
                topmost_iterator = fe->get_iterator()->getSymInfo();
            } else {
                topmost_iterator = fe->get_iterator()->getSymInfo();
            }
        }
        else if (s->get_nodetype() == AST_BFS)
        {
            random_iter_stack.push_back(s);
        }
        else if ((s->get_nodetype() == AST_WHILE) ||
                 (s->get_nodetype() == AST_IF))
        {
            condition_stack.push_back(s); 
        }
        else if (s->get_nodetype() == AST_ASSIGN) 
        {
           ast_assign* a  = (ast_assign*) s;
           if (!a->is_target_scalar()) {
                ast_field* f = a->get_lhs_field();
                if (a->is_reduce_assign()) {  // this is read & write
                    property_is_read(f->get_second()->getSymInfo(), 
                            f->get_first()->getSymInfo(), true);
                    std::list<ast_node*>& L = a->get_lhs_list(); 
                    std::list<ast_node*>::iterator I;
                    for(I=L.begin(); I != L.end(); I++) {
                        ast_node* n = *I;
                        assert(n->get_nodetype() == AST_FIELD);
                        ast_field* f2 = (ast_field*)n;
                        property_is_read(f2->get_second()->getSymInfo(), f2->get_first()->getSymInfo(), true);
                    }
                }
                else { // normal assignment
                    property_is_written(f->get_second()->getSymInfo(), f->get_first()->getSymInfo());
                }
            }
        }
        else if (s->get_nodetype() == AST_CALL)
        {
            // [todo]
        }

        return true;
    }

    virtual bool apply2(ast_sent*s) {
        if (!condition_stack.empty() && s == condition_stack.back()) {
            condition_stack.pop_back();
        }
        else if (!random_iter_stack.empty() && s == random_iter_stack.back()) {
            random_iter_stack.pop_back();
        }
        else {
            if (s->get_nodetype() == AST_FOREACH) {
                ast_foreach* fe = (ast_foreach*) s;
                if (fe->get_iterator()->getSymInfo() == topmost_iterator)
                {
                    topmost_iterator = NULL;
                    // finish written
                    std::set<gm_symtab_entry*>::iterator I;
                    for(I= under_current_linear_update.begin(); I!= under_current_linear_update.end(); I++)
                    {
                        gm_symtab_entry* prop = *I;
                        if (prop->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INVALID)
                            continue;

                        prop->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_OUT);
                    }

                    under_current_linear_update.clear();
                }
            }
        }

        return true;
    }

    virtual bool apply(ast_expr* e) {
        if (e->is_field()) {
            gm_symtab_entry *prop   = e->get_field()->get_first()->getSymInfo();
            gm_symtab_entry *driver = e->get_field()->get_first()->getSymInfo();
            property_is_read(prop, driver, false);
        }
    
        return true;
    }

    void property_is_written(gm_symtab_entry* prop, gm_symtab_entry* driver)
    {
        if (prop->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INVALID)
            return ;

        if (!is_under_condition() && !is_under_random_iterator() && (driver == topmost_iterator))
        {
            under_current_linear_update.insert(prop);
        }
    }

    void property_is_read(gm_symtab_entry* prop, gm_symtab_entry* driver, bool is_reduce)
    {
        if (prop->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INVALID)
            return ;

        if (is_reduce) {
            prop->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_INOUT);
        }

        if (driver == topmost_iterator) {
            if (is_under_random_iterator()) {
                prop->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_INOUT);
            }
        }
    }

  bool is_under_condition() {
      return ((int) condition_stack.size() > 0);
  }

  bool is_under_random_iterator() {
      return ((int) random_iter_stack.size() > 0);
  }

private:
    std::set<gm_symtab_entry*> under_current_linear_update;
    gm_symtab_entry* topmost_iterator;
    std::list<ast_sent*> condition_stack;
    std::list<ast_sent*> random_iter_stack;

};

//----------------------------------------------------------
// Check how property argument is used only
//    - input ; read-only
//    - output ; write only or (write whole -> read)
//    - inout ; read and write
//----------------------------------------------------------
void gm_fe_check_property_argument_usage::process(ast_procdef* proc)
{
   gm_symtab* props = proc->get_symtab_field();
   std::set<gm_symtab_entry*>& SET = props->get_entries(); 
   std::set<gm_symtab_entry*>::iterator I;
   std::set<gm_symtab_entry*> write_or_read_write;
   gm_rwinfo_map &R = get_rwinfo_sets(proc->get_body())->read_set;
   gm_rwinfo_map &W = get_rwinfo_sets(proc->get_body())->write_set;
   for(I=SET.begin(); I!=SET.end(); I++)
   {
       gm_symtab_entry* e = *I;
       if ((R.find(e) == R.end()) && (W.find(e) == W.end()))
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_UNUSED);
       else if ((R.find(e) == R.end()) && (W.find(e) != W.end()))
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_OUT);
       else if ((W.find(e) == W.end()) && (R.find(e) != R.end()))
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_IN);
       else {
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_INVALID); // temporary marking
       }
   }

   // now traverse the source and see if write after read
   gm_check_property_usage_t T;
   proc->get_body()->traverse_both(&T);
/*
   for(I=SET.begin(); I!=SET.end(); I++)
   {
       gm_symtab_entry* e = *I;
       assert( e->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INVALID); // temporary marking
       printf("%s used as : %s\n", e->getId()->get_orgname(), 
               (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_UNUSED) ? "Unused" :
               (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_OUT)    ? "Output" :
               (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN)     ? "Input" :
               (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT) ? "Inout" : "Invalid");
   }
   */
}
