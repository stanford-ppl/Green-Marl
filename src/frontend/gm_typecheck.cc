#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------------------------------------------------------
// Utility functions (Type summary)
//----------------------------------------------------------------------------------------------------------------
// check the byte size of two numeric type
static inline int gm_compare_numeric_type_size(int t1, int t2)
{
    // GMTYPE_... is defined as small to larger
    return t1 - t2; // +:t1 > t2 , 0:t2==t2, -:t1 < t2
}
// check the size (in Bytes) of two numeric types 
static inline int gm_get_larger_type(int t1, int t2)
{
    if (gm_compare_numeric_type_size(t1,t2) > 0) return t1;
    else return t2;
}

// determine resulting type of numeric operation A (+,-,*,/) B 
int gm_determine_result_type(int t1, int t2)
{
    // assumption. t1/t2 is compatible
    if (t1 == t2) return t1;
    else if (gm_is_inf_type(t1)) return t1;
    else if (gm_is_inf_type(t2)) return t2;
    else if (gm_is_numeric_type(t1))
    {
        if (gm_is_float_type(t1) == gm_is_float_type(t2))
            return gm_get_larger_type(t1, t2);
        else if (gm_is_float_type(t1)) return t1;
        else return t2;
    }
    else if (gm_is_iter_type(t1))
        return t2;
    else if (gm_is_iter_type(t1)) 
        return t1;
    else {assert(false);}
}

//-----------------------------------------------------------
// t1:lhs type summary, t2:rhs type summary (GMTYPE_ ...), t1!=t2
// return true, if assignment is possible
// assumption: target graph check is separately done.
// assumption: write-protection check is sperately done. (i.e. preventing write to node iterator)
//-----------------------------------------------------------


static enum {
    FOR_ASSIGN,
    FOR_EQ,
    FOR_LESS,
    FOR_BOP
} gm_type_compatible_t;

inline bool gm_check_compatible_types(int t1, int t2, int for_what)
{
    if (t1 == t2) return true;  

    //----------------------------------------------------------
    // ASSUMTION
    // let t1 be the 'smaller' type (ordering by GM_XXX_TYPE enumeration)
    // GRAPH -> PROP -> NODE-EDGE/ITER -> NUMERIC -> BOOL -> INF  (see gm_frontend_api.h)
    //----------------------------------------------------------
    if (t2 < t1) {
        int t3;
        t3 = t1;
        t1 = t2;
        t2 = t3;
    }

    if (gm_is_node_compatible_type(t1)) {
        if (for_what == FOR_BOP) return false;
        else return gm_is_node_compatible_type(t2);
    }

    if (gm_is_edge_compatible_type(t1)) {
        if (for_what == FOR_BOP) return false;
        else return gm_is_edge_compatible_type(t2);
    }

    if (gm_is_numeric_type(t1)) {
        if (for_what == FOR_BOP) return gm_is_numeric_type(t2);
        else return gm_is_numeric_type(t2) || gm_is_inf_type(t2); // it is possible to assign INF to numeric
    }

    if (gm_is_boolean_type(t1)) return gm_is_boolean_type(t2);
    if (gm_is_inf_type(t1)) return gm_is_inf_type(t2);

    //printf("unexpected type = %s\n", gm_get_type_string(t1));
    //assert(false);
    return false;
}

bool gm_is_compatible_type_for_assign(int lhs, int rhs) {return gm_check_compatible_types(lhs, rhs, FOR_ASSIGN);}
bool gm_is_compatible_type_for_eq(int t1, int t2)       {return gm_check_compatible_types(t1, t2, FOR_EQ);}
bool gm_is_compatible_type_for_less(int t1, int t2)     {return gm_check_compatible_types(t1, t2, FOR_LESS);}
bool gm_is_compatible_type_for_biop(int t1, int t2)     {return gm_check_compatible_types(t1, t2, FOR_BOP);}

/*
bool gm_frontend::do_type_resolve(ast_procdef* p)
{
    return true;
}
*/


//=========================================================================
// defines for gm_ast.h
//=========================================================================
void ast_node::create_symtabs()
{
    sym_vars = new gm_symtab(GM_SYMTAB_VAR, this);
    sym_fields = new gm_symtab(GM_SYMTAB_FIELD, this);
    sym_procs = new gm_symtab(GM_SYMTAB_PROC, this);
}
void ast_node::delete_symtabs()
{
    delete sym_vars;
    delete sym_fields;
    delete sym_procs;
}
void ast_node::get_this_scope(gm_scope *s) 
{
    s->push_symtabs(sym_vars, sym_fields, sym_procs);
}

ast_typedecl* ast_id::getTypeInfo() 
{
    assert(info != NULL);
    return info->getType();
}
int ast_id::getTypeSummary()
{
    assert(info!=NULL);
    return info->getType()->getTypeSummary();
}
ast_typedecl* ast_id::getTargetTypeInfo() 
{
    assert(info != NULL);
    return info->getType()->get_target_type();
}
int ast_id::getTargetTypeSummary()
{
    assert(info!=NULL);
    return info->getType()->getTargetTypeSummary();
}

char* ast_id::get_orgname_from_symbol() 
{
    assert(info!=NULL);
    assert(info->getId() != this);
    return info->getId()->get_orgname();
}
char* ast_id::get_genname_from_symbol() 
{
    assert(info!=NULL);
    assert(info->getId() != this);
    return info->getId()->get_genname();
}
char* ast_id::get_orgname() 
{
    if ((info==NULL)  || (info->getId() == this)) {
        if (name != NULL) return name;
        else {
            printf("line:%d, col:%d, name:%p\n", line, col, &name); fflush(stdout);
            assert(false);
        }
    } else {
        assert(info!=NULL);
        return get_orgname_from_symbol();
    }
}
void ast_id::use_names_from_symbol() 
{
    assert(info!=NULL); 
    if (name!=NULL) delete [] name;
    name = NULL;
    if (gen_name!=NULL) delete [] gen_name;
    gen_name = NULL;
}

char* ast_id::get_genname() 
{
    if ((info==NULL)  || (info->getId() == this)) {
        if (gen_name != NULL) return gen_name;
        else if (name != NULL) return name;
        else assert(false);
    } else {
        assert(info!=NULL);
        return get_genname_from_symbol();
    }
}

void ast_typedecl::enforce_well_defined()
{
   
  if (is_collection() || is_nodeedge() || is_all_graph_iterator() || is_property()) 
  {
      if (is_property())
        assert(target_type != NULL);
      assert(target_graph != NULL); 
      assert(target_graph->getSymInfo() != NULL);
  }
  else if (is_any_nbr_iterator())
  {
      assert(target_nbr!= NULL);
      assert(target_nbr->getSymInfo() != NULL);
      if (target_graph == NULL) {
          target_graph = target_nbr->getTypeInfo()->get_target_graph_id()->copy(true);
      }
  }
  else if (is_collection_iterator())
  {
      assert(target_collection != NULL);
      assert(target_collection->getSymInfo() != NULL);
      if (target_graph == NULL) {
          target_graph = target_collection->getTypeInfo()->get_target_graph_id()->copy(true);
      }
  }

  set_well_defined(true);
}
