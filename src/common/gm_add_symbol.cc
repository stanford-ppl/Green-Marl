
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"

ast_sentblock*   gm_find_upscope(ast_sent* s)
{
    if (s==NULL) return NULL;

    ast_node* up = s->get_parent();
    if (up == NULL) return NULL;
    if (up->get_nodetype() == AST_SENTBLOCK)
        return (ast_sentblock*) up;
    if (up->is_sentence() )
        return gm_find_upscope((ast_sent*)up);
    return NULL;
}

// temporary: should be improved
extern bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V); // should be called separatedly for property type.
extern bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl *type, bool is_readable, bool is_writeable);

//-------------------------------------------------------
// add a new symbol of primitive type into given sentence block
// assumption: newname does not have any name-conflicts
//--------------------------------------------------------
gm_symtab_entry* gm_add_new_symbol_primtype(ast_sentblock* sb, int primtype, char* newname)
{
    assert(sb!=NULL);

    gm_symtab* target_syms;
    target_syms = sb->get_symtab_var(); assert(target_syms!=NULL);

    // create type object and check
    ast_typedecl* type = ast_typedecl::new_primtype(primtype);
    bool success = gm_check_type_is_well_defined(type, target_syms);
    assert(success);

    // create id object and declare
    ast_id* new_id = ast_id::new_id(newname, 0, 0); 
    success = gm_declare_symbol(target_syms, new_id, type, true, true);
    assert(success);

    // return symbol
    gm_symtab_entry *e = NULL;
    e = new_id->getSymInfo(); 
    assert(e!=NULL);

    // these are temporary
    delete type;
    delete new_id;

    return e;
}

gm_symtab_entry* gm_add_new_symbol_nodeedge_type(ast_sentblock* sb, int nodeedge_type, gm_symtab_entry* graph_sym, char* newname)
{
    assert(sb!=NULL);

    gm_symtab* target_syms;
    target_syms = sb->get_symtab_var(); assert(target_syms!=NULL);

    // create type object and check
    ast_typedecl* type;
    if (nodeedge_type == GMTYPE_NODE)
        type = ast_typedecl::new_nodetype(graph_sym->getId()->copy(true));
    else if (nodeedge_type == GMTYPE_EDGE)
        type = ast_typedecl::new_edgetype(graph_sym->getId()->copy(true));
    else {
        assert(false);
    }
    bool success = gm_check_type_is_well_defined(type, target_syms);
    assert(success);

    // create id object and declare
    ast_id* new_id = ast_id::new_id(newname, 0, 0); 
    success = gm_declare_symbol(target_syms, new_id, type, true, true);
    assert(success);

    // return symbol
    gm_symtab_entry *e = NULL;
    e = new_id->getSymInfo(); 
    assert(e!=NULL);

    // these are temporary
    delete type;
    delete new_id;

    return e;
}

//-------------------------------------------------------
// add a new symbol of node(edge) property type into given sentence block
// assumption: newname does not have any name-conflicts
//--------------------------------------------------------
gm_symtab_entry* gm_add_new_symbol_property(
        ast_sentblock* sb, int primtype, 
        bool is_nodeprop, gm_symtab_entry* target_graph, 
        char* newname) // assumtpion: no name-conflict.
{
    ast_id* target_graph_id = target_graph->getId()->copy();
    ast_typedecl* target_type = ast_typedecl::new_primtype(primtype);

    // create type object and check
    ast_typedecl* type;
    if (is_nodeprop) 
        type = ast_typedecl::new_nodeprop(target_type, target_graph_id);
    else
        type = ast_typedecl::new_edgeprop(target_type, target_graph_id);

    bool success = gm_check_type_is_well_defined(type,sb->get_symtab_var());
    assert(success);

    // create property id object and declare
    ast_id* new_id = ast_id::new_id(newname, 0, 0);
    gm_symtab* target_syms;
    target_syms = sb->get_symtab_field(); assert(target_syms!=NULL);
    success = gm_declare_symbol(target_syms, new_id, type, true, true);
    assert(success);

    // return symbol
    gm_symtab_entry *e = NULL;
    e = new_id->getSymInfo();
    assert(e!=NULL);

    // these are temporary
    delete type;
    delete new_id;

    return e;
}


//-------------------------------------------------------------------
// - move a symbol entry up into another symbol table
// [assumption] new_tab belongs to a sentence block
// - name conflict does not happen 
// [return]
//   the sentence block which is the new scope
//-------------------------------------------------------------------
void gm_move_symbol_into(gm_symtab_entry *e, gm_symtab* old_tab, gm_symtab* new_tab, bool is_scalar)
{
    assert(new_tab->get_ast()->get_nodetype() == AST_SENTBLOCK);
    assert(old_tab->is_entry_in_the_tab(e));

    ast_sentblock* sb = (ast_sentblock*) new_tab->get_ast();

    // delete from the old-table
    old_tab->remove_entry_in_the_tab(e);

    // resolve name conflict in the up-scope
    //gm_resolve_name_conflict(sb, e, is_scalar);

    // add in the new-table
    new_tab->add_symbol(e);
}

//-------------------------------------------------------------------
// - move a symbol entry up into a sentence block
// - name conflict is resolved 
// [return]
//   the sentence block which is the new scope
//-------------------------------------------------------------------
ast_sentblock* gm_move_symbol_up(gm_symtab_entry *e, gm_symtab* old_tab, bool is_scalar)
{
    assert(old_tab->is_entry_in_the_tab(e));

    // find up_scope table
    gm_symtab* up;
    bool found = false;
    while(true) {
        up = old_tab->get_parent();
        if (up == NULL) break;
        if (up->get_ast()->get_nodetype() == AST_SENTBLOCK) {
            found = true;
            break;
        }
    }
    if (!found) return NULL;
    ast_sentblock* sb = (ast_sentblock*) up->get_ast();
    gm_symtab* new_tab = is_scalar? sb->get_symtab_var() : sb->get_symtab_field();

    gm_move_symbol_into(e, old_tab, new_tab, is_scalar);

    return sb;
}


//---------------------------------------------------------------------------
// remove set of symbols
//---------------------------------------------------------------------------
class gm_remove_symbols_t : public gm_apply {
public:
    gm_remove_symbols_t(std::set<gm_symtab_entry*>& S) : TARGETS(S) 
    { set_for_symtab(true); }

    virtual bool apply(gm_symtab* e, int symtab_sype) 
    {
        std::set<gm_symtab_entry*>::iterator T;
        for(T=TARGETS.begin(); T!=TARGETS.end(); T++)
        {
            gm_symtab_entry * t = *T;
            if (e->is_entry_in_the_tab(t)) {
                e->remove_entry_in_the_tab(t);
            }
        }

        return true;
    }

private:
    std::set<gm_symtab_entry*> TARGETS;
};

void gm_remove_symbols(ast_node* top, std::set<gm_symtab_entry*> & S)
{
    gm_remove_symbols_t T(S);
    top->traverse_pre(&T);
}
void gm_remove_symbol(ast_node* top, gm_symtab_entry* e)
{
    std::set<gm_symtab_entry*> S;
    S.insert(e);

    gm_remove_symbols_t T(S);
    top->traverse_pre(&T);
}


ast_sentblock*  gm_find_defining_sentblock_up(ast_node* node, gm_symtab_entry* e, bool is_property)
{
    while (node != NULL) {
        if (node->has_symtab()) 
        {
            if (is_property) {
                if (node->get_symtab_field()->is_entry_in_the_tab(e)) {
                    assert(node->get_nodetype() == AST_SENTBLOCK);
                    return (ast_sentblock*) node;
                }
            } else {
                if (node->get_symtab_var()->is_entry_in_the_tab(e)) {
                    if (node->get_nodetype()!=AST_SENTBLOCK) {
                        printf("%s not defined in a sentblock\n", e->getId()->get_genname());
                    }
                    assert(node->get_nodetype() == AST_SENTBLOCK);
                    return (ast_sentblock*) node;
                }
            }

        }
        node = node->get_parent();
    }

    return NULL;
}
