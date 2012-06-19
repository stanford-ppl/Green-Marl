#ifndef GL_AST_H
#define GL_AST_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "gm_frontend_api.h"

enum
{
    AST_ID,       // 
    AST_FIELD,    // A.B
    AST_IDLIST,   // A, B, C, 
    AST_TYPEDECL, // INT
    AST_ARGDECL,  // a,b : B
    AST_PROCDEF,  // proc A() {}
    AST_EXPR,     // c + 3
    AST_EXPR_RDC,       // c + 3
    AST_EXPR_BUILTIN,   // c + 3
    AST_EXPR_FOREIGN,  // Foreign Expression
    AST_SENT,     // 
    AST_SENTBLOCK, // { ... }
    AST_ASSIGN,   // C =D
    AST_VARDECL,  // INT x;
                  // NODE_PROPERTY<INT>(G) x;
    AST_FOREACH,  // Foreach (t: G.Nodes) {...}
    AST_IF,       // IF (x) THEN s; ELSE z ;
    AST_WHILE,    // While (x) {...} or Do {...} While (x)
    AST_RETURN,   // Return y;
    AST_BFS,      // InBFS(t: G.Nodes) {....} InReverse {....}
    AST_CALL,     // Call to (built-in) function
    AST_FOREIGN,  // Foreign syntax
    AST_NOP,      // NOP (for backend-only)

    AST_END,
};

class gm_symtab_entry;
class gm_symtab;
class gm_scope;
// typechecker context;
class gm_apply;
// defined in gm_traverse.h
char* gm_strdup(const char* org); // defined in gm_misc.cc
class ast_expr;
class ast_node;

class expr_list
{
public:
    // temporary class used only during parsing
    std::list<ast_expr*> LIST;
};

class lhs_list
{
public:
    // temporary class used only during parsing
    std::list<ast_node*> LIST;
};

// any information can be added to nodes
class ast_extra_info
{
public:
    int ival;
    bool bval;
    float fval;
    void* ptr1;
    void* ptr2;

    ast_extra_info();
    ast_extra_info(bool b);
    ast_extra_info(int i);
    ast_extra_info(float f);
    ast_extra_info(void* p1, void* p2 = NULL);

    virtual ~ast_extra_info() {
    }
    virtual ast_extra_info* copy();
    virtual void base_copy(ast_extra_info* from);
};

class ast_extra_info_string : public ast_extra_info
{
public:
    char* str;
    ast_extra_info_string();
    virtual ~ast_extra_info_string();

    ast_extra_info_string(const char* org);
    const char* get_string();
    virtual ast_extra_info* copy();
};

class ast_extra_info_set : public ast_extra_info
{
public:
    std::set<void*> set;
    ast_extra_info_set() {
    }
    ~ast_extra_info_set() {
    }
    virtual ast_extra_info* copy() {
        assert(false);
        return NULL;
    }
    std::set<void*>& get_set() {
        return set;
    }
};

class ast_extra_info_list : public ast_extra_info
{
public:
    std::list<void*> list;
    ast_extra_info_list() {
    }
    ~ast_extra_info_list() {
    }
    virtual ast_extra_info* copy() {
        assert(false);
        return NULL;
    }
    std::list<void*>& get_list() {
        return list;
    }
};

class ast_extra_info_map : public ast_extra_info
{
public:
    std::map<void*, void*> map;
    ast_extra_info_map() {
    }
    ~ast_extra_info_map() {
    }
    virtual ast_extra_info* copy() {
        assert(false);
        return NULL;
    }
    std::map<void*, void*>& get_map() {
        return map;
    }
};

class ast_node
{
    friend class gm_apply;
protected:
    ast_node(int nt) :
            nodetype(nt), parent(NULL) {
    }
    int nodetype;

protected:
    ast_node() :
            nodetype(AST_END), parent(NULL) {
    }
    ast_node* parent;

public:
    void set_nodetype(int nt) {
        nodetype = nt;
    }

    virtual ~ast_node() {
        std::map<std::string, ast_extra_info*>::iterator i;
        for (i = extra.begin(); i != extra.end(); i++) {
            delete i->second;
        }
        extra.clear();
    }

    int get_nodetype() {
        return nodetype;
    }
    ast_node* get_parent() {
        return parent;
    }
    void set_parent(ast_node* p) {
        parent = p;
    }

    virtual bool is_sentence() {
        return false;
    }
    virtual bool is_expr() {
        return false;
    }
    bool has_symtab() {
        return (nodetype == AST_SENTBLOCK) || (nodetype == AST_FOREACH) || (nodetype == AST_PROCDEF) || (nodetype == AST_EXPR_RDC) || (nodetype == AST_BFS)
                || false;
    }

    // for parser debug
    virtual void reproduce(int id_level) = 0; // defined in reproduce.cc
    virtual void dump_tree(int id_level) = 0; // defined in dump_tree.cc

    // defined in traverse.cc
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre) {
        assert(false);
    }
    void traverse_pre(gm_apply*a) {
        traverse(a, false, true);
    }
    void traverse_post(gm_apply*a) {
        traverse(a, true, false);
    }
    void traverse_both(gm_apply*a) {
        traverse(a, true, true);
    }
    virtual void apply_symtabs(gm_apply*a, bool is_post_apply = false);

    // scoped elements
    virtual bool has_scope() {
        return false;
    }
    virtual gm_symtab* get_symtab_var() {
        assert(has_scope());
        return sym_vars;
    }
    virtual gm_symtab* get_symtab_field() {
        assert(has_scope());
        return sym_fields;
    }
    virtual gm_symtab* get_symtab_proc() {
        assert(has_scope());
        return sym_procs;
    }
    virtual void get_this_scope(gm_scope* s);
    virtual void set_symtab_var(gm_symtab* v) {
        assert(has_scope());
        sym_vars = v;
    }
    virtual void set_symtab_field(gm_symtab* f) {
        assert(has_scope());
        sym_fields = f;
    }
    virtual void set_symtab_proc(gm_symtab* p) {
        assert(has_scope());
        sym_procs = p;
    }

protected:
    gm_symtab* sym_vars;
    gm_symtab* sym_fields;
    gm_symtab* sym_procs;
    virtual void delete_symtabs();
    virtual void create_symtabs();

public:
    int get_line() {
        return line;
    }
    int get_col() {
        return col;
    }
    void set_line(int l) {
        line = l;
    }
    void set_col(int c) {
        col = c;
    }
    void copy_line_info(ast_node* n) {
        this->col = n->col;
        this->line = n->line;
    }

    //--------------------------------------
    // extra infomation attached to this node
    //--------------------------------------
    bool has_info(const char* id);
    ast_extra_info* find_info(const char*id); // returns NULL if not
    bool find_info_bool(const char* id);
    const char* find_info_string(const char* id);
    float find_info_float(const char* id);
    int find_info_int(const char* id);
    void* find_info_ptr(const char* id);
    void* find_info_ptr2(const char* id);
    void add_info(const char* id, ast_extra_info* e);
    void add_info_int(const char* id, int i);
    void add_info_bool(const char* id, bool b);
    void add_info_ptr(const char* id, void* ptr1, void*ptr2 = NULL);
    void add_info_float(const char* id, float f);
    void add_info_string(const char* id, const char* str);
    void remove_info(const char* id);
    void remove_all_info();

    bool has_info_set(const char* id);
    void add_info_set_element(const char* id, void* element);
    std::set<void*>& get_info_set(const char* id);

    bool has_info_list(const char* id);
    void add_info_list_element(const char* id, void* element);
    std::list<void*>& get_info_list(const char* id);

    bool has_info_map(const char* id);
    void add_info_map_key_value(const char* id, void* key, void* value);
    void* find_info_map_value(const char* id, void* key);
    std::map<void*, void*>& get_info_map(const char* id);

    void copy_info_from(ast_node* n); // [xxx] what happens to the entry with same key?

protected:
    int line;
    int col;
    std::map<std::string, ast_extra_info*> extra;
};

// access of identifier
class ast_typedecl;
class ast_id : public ast_node
{
    friend class gm_symtab_entry;
public:
    virtual ~ast_id() {
        delete[] name;
        delete[] gen_name; // if name is not usable in generator
    }

    // make a copy of id reference
    // [NOTE] pointer to symbol table entry is *not* copied if cp_syminfo is false
    ast_id* copy(bool cp_syminfo = false) {
        ast_id* cp;
        cp = new ast_id(get_orgname(), line, col); // name can be null here. [xxx] WHY?
        if (cp_syminfo) {
            cp->info = this->info;
        }
        return cp;
    }

    //-------------------------------------------------
    // Type information related to this id
    // set up by type-checker during local_typecheck
    //-------------------------------------------------
    gm_symtab_entry* getSymInfo() {
        return info;
    }
    void setSymInfo(gm_symtab_entry* e, bool is_symtab_entry = false) {
        info = e;
        if (!is_symtab_entry) use_names_from_symbol();
    }
    ast_typedecl* getTypeInfo(); // returns Typedecl for this id
    int getTypeSummary();        // returns Typedecl for this
                                 // return TypeDecl->getTypeSummary. returns one of GMTYPE_*

                                 // only called for property types
    ast_typedecl* getTargetTypeInfo();
    int getTargetTypeSummary();

private:
    ast_id() :
            ast_node(AST_ID), name(NULL), info(NULL), gen_name(NULL) {
    }
    ast_id(const char* org, int l, int c) :
            ast_node(AST_ID), info(NULL), gen_name(NULL) {
        if (org != NULL) {
            name = new char[strlen(org) + 1];
            strcpy(name, org);
        } else {
            name = NULL;
        }
        set_line(l);
        set_col(c);
    }

public:
    static ast_id* new_id(const char* org, int line, int col) {
        return new ast_id(org, line, col);
    }
    char* get_orgname();  // returns name in GM
    void set_orgname(const char* c) {  // copy is saved. old name is deleted
        if (name != NULL) delete[] name;
        name = new char[strlen(c) + 1];
        strcpy(name, c);
    }
    char* get_genname();

    void set_genname(const char* c) {  // copy is saved. old name is deleted
        if (gen_name != NULL) delete[] gen_name;
        gen_name = new char[strlen(c) + 1];
        strcpy(gen_name, c);
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);

public:
    char* name;

private:
    gm_symtab_entry* info;
    char* gen_name;

    char* get_orgname_from_symbol();  // gm_typecheck.cc
    char* get_genname_from_symbol();  // gm_typecheck.cc
public:
    void use_names_from_symbol(); // gm_typecheck.cc

};

class ast_idlist : public ast_node
{
public:
    ast_idlist() :
            ast_node(AST_IDLIST) {
    }

    virtual ~ast_idlist() {
        for (int i = 0; i < (int) lst.size(); i++)
            delete lst[i];
        lst.clear();
    }
    ast_idlist* copy(bool cp_sym = false) {
        ast_idlist* cpy = new ast_idlist;
        for (int i = 0; i < (int) lst.size(); i++)
            cpy->add_id(lst[i]->copy(cp_sym));
        return cpy;
    }

    void add_id(ast_id* id) {
        lst.push_back(id);
    }
    int get_length() {
        return lst.size();
    }
    ast_id* get_item(int i) {
        return lst[i];
    }
    virtual void apply_id(gm_apply*a, bool is_post_apply = false); // defined in traverse.cc
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    bool contains(char* id) {
        for (int i = 0; i < (int) lst.size(); i++)
            if (!strcmp(id, lst[i]->get_orgname())) return true;
        return false;
    }

    int get_line() {
        assert(get_length()>0);
        return lst[0]->get_line();
    }
    int get_col() {
        assert(get_length()>0);
        return lst[0]->get_col();
    }

private:
    std::vector<ast_id*> lst;
};

class ast_field : public ast_node
{ // access of node/edge property
public:
    virtual ~ast_field() {
        delete first;
        delete second;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);

private:
    ast_field() :
            ast_node(AST_FIELD), first(NULL), second(NULL), rarrow(false) {
    }
    ast_field(ast_id* l, ast_id* f) :
            ast_node(AST_FIELD), first(l), second(f), rarrow(false) {
        first->set_parent(this);
        second->set_parent(this);
        this->line = first->get_line();
        this->col = first->get_col();
    }
public:
    static ast_field* new_field(ast_id* l, ast_id* f, bool is_r_arrow = false) {
        ast_field* af = new ast_field(l, f);
        l->set_parent(af);
        f->set_parent(af);
        af->set_rarrow(is_r_arrow);
        return af;
    }
    // FIRST.SECOND
    ast_id* get_first() {
        return first;
    }           // Identifier
    ast_id* get_second() {
        return second;
    }         // Field
    bool is_rarrow() {
        return rarrow;
    }         // Is it Edge(x).y?
    void set_rarrow(bool b) {
        rarrow = b;
    }

    // type information about source (node/edge/graph)
    int getSourceTypeSummary() {
        return first->getTypeSummary();
    }
    ast_typedecl* getSourceTypeInfo() {
        return first->getTypeInfo();
    }

    // type information about field (nodeprop/edgeprop)
    int getTypeSummary() {
        return second->getTypeSummary();
    }
    ast_typedecl* getTypeInfo() {
        return second->getTypeInfo();
    }

    // type information about target (primitive)
    int getTargetTypeSummary() {
        return second->getTargetTypeSummary();
    }
    ast_typedecl* getTargetTypeInfo() {
        return second->getTargetTypeInfo();
    }

    inline void set_first(ast_id* f) {
        first = f;
        f->set_parent(this);
    }
    inline void set_second(ast_id* s) {
        second = s;
        s->set_parent(this);
    }

    ast_field* copy(bool cp_sym = false) {
        ast_id* f = first->copy(cp_sym);
        ast_id* s = second->copy(cp_sym);
        ast_field* fld = new ast_field(f, s);
        return fld;
    }

private:
    ast_id* first;
    ast_id* second;
    bool rarrow;
};

//==========================================================================

class ast_typedecl : public ast_node
{  // property or type
private:
    ast_typedecl() :
            ast_node(AST_TYPEDECL), target_type(NULL), target_graph(NULL), target_collection(NULL), target_nbr(NULL), target_nbr2(NULL), _well_defined(false) {
    }

public:
    // give a deep copy
    ast_typedecl* copy() {
        ast_typedecl *p = new ast_typedecl();
        p->type_id = this->type_id;
        p->target_type = (this->target_type == NULL) ? NULL : this->target_type->copy();
        p->target_graph = (this->target_graph == NULL) ? NULL : this->target_graph->copy(true);
        p->target_collection = (this->target_collection == NULL) ? NULL : this->target_collection->copy(true);
        p->target_nbr = (this->target_nbr == NULL) ? NULL : this->target_nbr->copy(true);
        p->target_nbr2 = (this->target_nbr2 == NULL) ? NULL : this->target_nbr2->copy(true);
        p->line = this->line;
        p->col = this->col;
        p->_well_defined = this->_well_defined;

        return p;
    }

    virtual ~ast_typedecl() {
        delete target_type;
        delete target_graph; //gets deleted twice (sometimes) why??? o.O
        delete target_collection;
        delete target_nbr;
    }
    static ast_typedecl* new_primtype(int ptype_id) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = ptype_id;
        return t;
    }
    static ast_typedecl* new_graphtype(int gtype_id) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = gtype_id;
        return t;
    }
    static ast_typedecl* new_nodetype(ast_id* tg) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = GMTYPE_NODE;
        if (tg == NULL) //no graph defined for this node - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_edgetype(ast_id* tg) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = GMTYPE_EDGE;
        if (tg == NULL)  //no graph defined for this edge - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }

    static ast_typedecl* new_nodeedge_iterator(ast_id* tg, int iter_type) {
        assert(gm_is_all_graph_iter_type(iter_type));
        ast_typedecl* t = new ast_typedecl();
        t->type_id = iter_type;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_nbr_iterator(ast_id* tg, int iter_type) {
        assert(gm_is_any_nbr_iter_type(iter_type));
        ast_typedecl* t = new ast_typedecl();
        t->type_id = iter_type;
        t->target_nbr = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_common_nbr_iterator(ast_id* tg, ast_id* tg2, int iter_type) {
        assert(gm_is_any_nbr_iter_type(iter_type));
        ast_typedecl* t = new ast_typedecl();
        t->type_id = iter_type;
        t->target_nbr = tg;
        t->target_nbr2 = tg2;
        tg->set_parent(t);
        tg2->set_parent(t);
        return t;
    }
    static ast_typedecl* new_set(ast_id* tg, int set_type) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = set_type;
        if (tg == NULL) //no graph defined for this set - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_set_iterator(ast_id* set, int iter_type) {
        // deprecated
        ast_typedecl* t = new ast_typedecl();
        t->type_id = iter_type;
        t->target_collection = set;
        set->set_parent(t);
        return t;
    }
    static ast_typedecl* new_collection_iterator(ast_id* set, int iter_type) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = iter_type;
        t->target_collection = set;
        set->set_parent(t);
        return t;
    }

    static ast_typedecl* new_nodeprop(ast_typedecl* type, ast_id* tg) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = GMTYPE_NODEPROP;
        t->target_type = type;
        type->set_parent(t);
        if (tg == NULL) //no graph defined for this property - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_edgeprop(ast_typedecl* type, ast_id* tg) {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = GMTYPE_EDGEPROP;
        t->target_type = type;
        type->set_parent(t);
        if (tg == NULL) //no graph defined for this property - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }
    static ast_typedecl* new_void() {
        ast_typedecl* t = new ast_typedecl();
        t->type_id = GMTYPE_VOID;
        return t;
    }

    int get_typeid() {
        return type_id;
    }
    void set_typeid(int s) {
        type_id = s;
    }

    // seed gm_frontend_api.h
    bool is_primitive() {
        return gm_is_prim_type(type_id);
    }
    bool is_graph() {
        return gm_is_graph_type(type_id);
    }
    bool is_node_property() {
        return gm_is_node_property_type(type_id);
    }
    bool is_edge_property() {
        return gm_is_edge_property_type(type_id);
    }
    bool is_property() {
        return gm_is_property_type(type_id);
    }
    bool is_node() {
        return gm_is_node_type(type_id);
    }
    bool is_edge() {
        return gm_is_edge_type(type_id);
    }
    bool is_nodeedge() {
        return gm_is_nodeedge_type(type_id);
    }
    bool is_collection() {
        return gm_is_collection_type(type_id);
    }
    bool is_node_collection() {
        return gm_is_node_collection_type(type_id);
    }
    bool is_edge_collection() {
        return gm_is_edge_collection_type(type_id);
    }
    bool is_collection_iterator() {
        return gm_is_collection_iter_type(type_id);
    }
    bool is_unknown_collection_iterator() {
        return gm_is_unknown_collection_iter_type(type_id);
    }
    bool is_node_iterator() {
        return gm_is_node_iter_type(type_id);
    }
    bool is_edge_iterator() {
        return gm_is_edge_iter_type(type_id);
    }
    bool is_node_edge_iterator() {
        return is_node_iterator() || is_edge_iterator();
    }
    bool is_numeric() {
        return gm_is_numeric_type(type_id);
    }
    bool is_node_compatible() {
        return gm_is_node_compatible_type(type_id);
    }
    bool is_edge_compatible() {
        return gm_is_edge_compatible_type(type_id);
    }
    bool is_node_edge_compatible() {
        return gm_is_node_edge_compatible_type(type_id);
    }
    bool is_boolean() {
        return gm_is_boolean_type(type_id);
    }
    bool is_reverse_iterator() {
        return gm_is_iteration_use_reverse(type_id);
    }
    bool has_target_graph() {
        return gm_has_target_graph_type(type_id);
    }
    bool is_void() {
        return gm_is_void_type(type_id);
    }
    bool is_all_graph_iterator() {
        return gm_is_all_graph_iter_type(type_id);
    }
    bool is_any_nbr_iterator() {
        return gm_is_any_nbr_iter_type(type_id);
    }
    bool is_common_nbr_iterator() {
        return gm_is_common_nbr_iter_type(type_id);
    }
    bool is_sequence_collection() {
        return gm_is_sequence_collection_type(type_id);
    }
    bool is_order_collection() {
        return gm_is_order_collection_type(type_id);
    }
    bool is_set_collection() {
        return gm_is_set_collection_type(type_id);
    }
    bool is_sequential_collection() {
        return gm_is_sequential_collection_type(type_id);
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);

    // there is no copying of type

    gm_symtab_entry* get_target_graph_sym() {
        if (is_collection_iterator()) {
            assert(target_collection!=NULL);
            assert(target_collection->getTypeInfo() != NULL);
            assert(target_collection->getTypeInfo()->get_target_graph_sym() != NULL);
            return target_collection->getTypeInfo()->get_target_graph_sym();
        } else if (is_collection() || is_property() || is_nodeedge() || is_node_iterator() || is_edge_iterator()) {
            assert(target_graph!=NULL);
            assert(target_graph->getSymInfo() != NULL);
            return target_graph->getSymInfo();
        } else {
            assert(false);
            return NULL;
        }
    }
    ast_id* get_target_graph_id() {
        return target_graph;
    }
    ast_id* get_target_collection_id() {
        return target_collection;
    }
    ast_id* get_target_nbr_id() {
        return target_nbr;
    }
    ast_id* get_target_nbr2_id() {
        return target_nbr2;
    }
    ast_typedecl* get_target_type() {
        return target_type;
    }
    int getTypeSummary() {  // same as get type id
        return type_id;
    }
    void setTypeSummary(int s) {
        // type id might be overriden during type-checking
        set_typeid(s);
    }
    int getTargetTypeSummary() {
        assert(is_property());
        assert(target_type!=NULL);
        return target_type->getTypeSummary();
    }

    void set_target_graph_id(ast_id* i) {

        assert(target_graph == NULL);
        assert(i->getTypeInfo() != NULL);
        target_graph = i;
        i->set_parent(this);
    }

    bool is_well_defined() {
        return _well_defined;
    }
    void set_well_defined(bool b) {
        _well_defined = b;
    }

    // for the compiler generated symbols
    // (when scope is not available)
    void enforce_well_defined();

private:
    // defined in gm_frontend_api.h
    int type_id;
    ast_typedecl* target_type;  // for property
    ast_id* target_graph;       // for property, node, edge, set
    ast_id* target_collection;  // for set-iterator set
    ast_id* target_nbr;         // for nbr-iterator
    ast_id* target_nbr2;        // for common neighbor iterator
    bool _well_defined;
};

//==========================================================================
class ast_sent : public ast_node
{
protected:
    ast_sent(int y) :
            ast_node(y), eline(0), _par(false) {
    }

    // save original empty lines before this sentence
public:
    int get_empty_lines_before() {
        return eline;
    }
    void set_empty_lines_before(int t) {
        eline = t;
    }
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre) {
        assert(false);
    }
    ;
    virtual bool is_sentence() {
        return true;
    }

    // when to set this variable? (-> should be just before code gen.)
    virtual bool is_under_parallel_execution() {
        return _par;
    }
    virtual void set_under_parallel_execution(bool b) {
        _par = b;
    }

private:
    ast_sent() :
            ast_node(AST_SENT) {
    }
    int eline;
    bool _par;

};

extern const char* gm_get_nodetype_string(int t);

class ast_sentblock : public ast_sent
{
public:
    virtual ~ast_sentblock() {
        std::list<ast_sent*>::iterator it;
        for (it = sents.begin(); it != sents.end(); it++) {
            delete *it;
        }
        delete_symtabs();
    }
public:
    static ast_sentblock* new_sentblock() {
        return new ast_sentblock();
    }
    void add_sent(ast_sent* s) {
        sents.push_back(s);
        s->set_parent(this);
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
    virtual bool has_scope() {
        return true;
    }

    std::list<ast_sent*>& get_sents() {
        return sents;
    }

private:
    ast_sentblock() :
            ast_sent(AST_SENTBLOCK) {
        create_symtabs();
    }

private:
    std::list<ast_sent*> sents;

};

class ast_argdecl : public ast_node
{
public:
    virtual ~ast_argdecl() {
        delete idlist;
        delete type;
    }
private:
    ast_argdecl() :
            ast_node(AST_ARGDECL), tc_finished(false) {
    }
public:
    static ast_argdecl* new_argdecl(ast_idlist* id, ast_typedecl* type) {
        ast_argdecl* d = new ast_argdecl();
        d->idlist = id;
        d->type = type;
        id->set_parent(d);
        type->set_parent(d);
        return d;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);

    ast_typedecl* get_type() {
        if (!tc_finished) {
            return type;           // obtain type from syntax
        } else {
            return idlist->get_item(0)->getTypeInfo(); // obtain type from symbol table
        }
    }
    ast_idlist* get_idlist() {
        return idlist;
    }

private:
    ast_idlist* idlist;
    ast_typedecl* type;
    bool tc_finished; // is typecheck finished?
};

//-------------------------------------------------------
// Procedure declaration
//-------------------------------------------------------
class ast_procdef : public ast_node
{
public:
    virtual ~ast_procdef() {
        std::list<ast_argdecl*>::iterator it;
        for (it = in_args.begin(); it != in_args.end(); it++)
            delete *it;
        for (it = out_args.begin(); it != out_args.end(); it++)
            delete *it;
        delete_symtabs();

        delete id;
        delete sents;
        delete ret_type;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual void apply_id(gm_apply*a, bool is_post_apply = false); // defined in traverse.cc
    virtual bool has_scope() {
        return true;
    }

private:
    ast_procdef() :
            ast_node(AST_PROCDEF), id(NULL), sents(NULL), ret_type(NULL), local(false) {
        create_symtabs();
    }

public:
    static ast_procdef* begin_new_procdef(ast_id* id) {
        ast_procdef* d = new ast_procdef();
        d->id = id;
        id->set_parent(d);
        return d;
    }

    void add_argdecl(ast_argdecl* d) {
        in_args.push_back(d);
        d->set_parent(this);
    }
    void add_out_argdecl(ast_argdecl* d) {
        out_args.push_back(d);
        d->set_parent(this);
    }
    void set_sentblock(ast_sentblock* s) {
        sents = s;
        s->set_parent(this);
    }
    void set_return_type(ast_typedecl* t) {
        ret_type = t;
        t->set_parent(this);
    }

    std::list<ast_argdecl*>& get_in_args() {
        return in_args;
    }
    std::list<ast_argdecl*>& get_out_args() {
        return out_args;
    }
    ast_sentblock* get_body() {
        return sents;
    }
    ast_typedecl* get_return_type() {
        return ret_type;
    }
    ast_id* get_procname() {
        return id;
    }

    void set_local(bool b) {
        local = b;
    }
    bool is_local() {
        return local;
    }

private:
    ast_id* id;       // function name
    std::list<ast_argdecl*> in_args;
    std::list<ast_argdecl*> out_args;
    ast_sentblock* sents;
    ast_typedecl* ret_type;
    bool local;
};

//-------------------------------------------------------
// Class of Expressions
//-------------------------------------------------------
static enum
{
    GMEXPR_IVAL,    // integer literal
    GMEXPR_FVAL,    // floating literal
    GMEXPR_BVAL,    // boolean literal
    GMEXPR_INF,     // infinite literal
    GMEXPR_NIL,      // NIL literal
    GMEXPR_ID,      // identifier
    GMEXPR_FIELD,   // field access
    GMEXPR_UOP,     // unary op (neg)
    GMEXPR_LUOP,     // logical not
    GMEXPR_BIOP,     // numeric binary op
    GMEXPR_LBIOP,    // logical binary op
    GMEXPR_COMP,     // comparision ops (==, !=, <, >, <=, >=)
    GMEXPR_REDUCE,   // reduction ops (Sum, Product, Min, Max)
    GMEXPR_BUILTIN,  // builtin ops (NumNodes, NumNbrs, ...)
    GMEXPR_TER,      // ternary operation
    GMEXPR_FOREIGN,
// foreign expression
} GMEXPR_T;

// Numeric or boolean expression
class gm_builtin_def;
// defined in gm_builtin.h

class ast_expr : public ast_node
{
public:
    virtual ~ast_expr() {
        delete id1;
        delete field;
        delete left;    // object is new-ed
        delete right;
        delete cond;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual bool is_expr() {
        return true;
    }

    virtual ast_expr* copy(bool cp_syminfo = false);

    // factory methods
public:
    static ast_expr* new_id_expr(ast_id* id) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_ID;
        E->id1 = id;
        id->set_parent(E);
        return E;
    }

    static ast_expr* new_field_expr(ast_field* f) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_FIELD;
        E->field = f;
        f->set_parent(E);
        return E;
    }

    static ast_expr* new_ival_expr(long ival) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_IVAL;
        E->type_of_expression = GMTYPE_INT; // LONG?
        E->ival = ival;
        return E;
    }
    static ast_expr* new_fval_expr(double fval) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_FVAL;
        E->type_of_expression = GMTYPE_FLOAT; // DOUBLE?
        E->fval = fval;
        return E;
    }
    static ast_expr* new_bval_expr(bool bval) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_BVAL;
        E->type_of_expression = GMTYPE_BOOL;
        E->bval = bval;
        return E;
    }
    static ast_expr* new_nil_expr() {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_NIL;
        E->type_of_expression = GMTYPE_NIL_UNKNOWN;
        return E;
    }
    static ast_expr* new_inf_expr(bool is_p) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_INF;
        E->type_of_expression = GMTYPE_INF;
        E->plus_inf = is_p;
        return E;
    }
    static ast_expr* new_typeconv_expr(int target_type, ast_expr* l) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_UOP;
        E->op_type = GMOP_TYPEC;
        E->type_of_expression = target_type; // GMTYPE_xxx
        E->left = l;
        l->up = E;
        l->parent = E;
        return E;
    }
    static ast_expr* new_uop_expr(int op_type, ast_expr* l) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_UOP;
        E->op_type = op_type;
        E->type_of_expression = GMTYPE_UNKNOWN_NUMERIC;
        E->left = l;
        l->up = E;
        l->parent = E;
        return E;
    }
    static ast_expr* new_biop_expr(int op_type, ast_expr* l, ast_expr* r) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_BIOP;
        E->op_type = op_type;
        E->left = l;
        E->right = r;
        l->up = E;
        r->up = E;
        r->is_right = true;
        E->type_of_expression = GMTYPE_UNKNOWN_NUMERIC;
        l->parent = r->parent = E;
        return E;
    }
    static ast_expr* new_luop_expr(int op_type, ast_expr* l) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_LUOP;
        E->op_type = op_type;
        E->type_of_expression = GMTYPE_BOOL;
        E->left = l;
        l->up = E;
        l->parent = E;
        return E;
    }
    static ast_expr* new_lbiop_expr(int op_type, ast_expr* l, ast_expr* r) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_LBIOP;
        E->op_type = op_type;
        E->left = l;
        E->right = r;
        l->up = E;
        r->up = E;
        r->is_right = true;
        E->type_of_expression = GMTYPE_BOOL;
        l->parent = r->parent = E;
        return E;
    }
    static ast_expr* new_comp_expr(int op_type, ast_expr* l, ast_expr* r) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_COMP;
        E->op_type = op_type;
        E->left = l;
        E->right = r;
        l->up = E;
        r->up = E;
        r->is_right = true;
        E->type_of_expression = GMTYPE_BOOL;
        l->parent = r->parent = E;
        return E;
    }

    static ast_expr* new_ternary_expr(ast_expr* cond, ast_expr* left, ast_expr* right) {
        ast_expr* E = new ast_expr();
        E->expr_class = GMEXPR_TER;
        E->op_type = GMOP_TER;
        E->left = left;
        E->right = right;
        E->cond = cond;
        cond->up = left->up = right->up = E;
        cond->parent = left->parent = right->parent = E;
        right->is_right = cond->is_cond = true;
        return E;
    }

protected:
    ast_expr() :
            ast_node(AST_EXPR), expr_class(GMEXPR_ID), left(NULL), right(NULL), cond(NULL), up(NULL), id1(NULL), field(NULL), ival(0), fval(0), bval(false), plus_inf(
                    false), op_type(GMOP_END), is_right(false), is_cond(false), type_of_expression(GMTYPE_UNKNOWN), alternative_type_of_expression(
                    GMTYPE_UNKNOWN), bound_graph_sym(NULL) {
    }

protected:
    int expr_class;  // GMEXPR_...
    ast_expr* left;
    ast_expr* right;
    ast_expr* cond;
    ast_expr* up;
    ast_id* id1;
    ast_field* field;
    long ival;
    double fval;
    bool bval;
    bool plus_inf;
    int op_type;
    bool is_right; // am I a right-operand?
    bool is_cond; // am I a conditional-operand?

    int type_of_expression;  // set after local typecheck
    int alternative_type_of_expression; // used for group-assignment only. (during type checking)

public:
    bool is_biop() {
        return (expr_class == GMEXPR_BIOP) || (expr_class == GMEXPR_LBIOP);
    }
    bool is_uop() {
        return (expr_class == GMEXPR_UOP) || (expr_class == GMEXPR_LUOP);
    }
    bool is_comp() {
        return (expr_class == GMEXPR_COMP);
    }
    bool is_id() {
        return expr_class == GMEXPR_ID;
    }
    bool is_nil() {
        return expr_class == GMEXPR_NIL;
    }
    bool is_field() {
        return expr_class == GMEXPR_FIELD;
    }
    bool is_int_literal() {
        return expr_class == GMEXPR_IVAL;
    }
    bool is_float_literal() {
        return expr_class == GMEXPR_FVAL;
    }
    bool is_boolean_literal() {
        return expr_class == GMEXPR_BVAL;
    }
    bool is_inf() {
        return expr_class == GMEXPR_INF;
    }
    bool is_literal() {
        return is_int_literal() || is_float_literal() || is_boolean_literal() || is_inf();
    }
    bool is_reduction() {
        return expr_class == GMEXPR_REDUCE;
    }
    bool is_builtin() {
        return expr_class == GMEXPR_BUILTIN;
    }
    bool is_terop() {
        return expr_class == GMEXPR_TER;
    }
    bool is_foreign() {
        return expr_class == GMEXPR_FOREIGN;
    }

    //-----------------------------------------------
    // type is set after type-checker execution
    //-----------------------------------------------
    int get_type_summary() {
        return type_of_expression;
    }
    void set_type_summary(int t) {
        type_of_expression = t;
    } // set by type checker

    gm_symtab_entry* get_bound_graph() {
        return bound_graph_sym;
    }
    void set_bound_graph(gm_symtab_entry*e) {
        bound_graph_sym = e;
    }

    long get_ival() {
        return ival;
    }
    double get_fval() {
        return fval;
    }
    bool get_bval() {
        return bval;
    }
    bool is_plus_inf() {
        return is_inf() && plus_inf;
    } // true o
    ast_id* get_id() {
        return id1;
    }
    ast_field* get_field() {
        return field;
    }

    int get_opclass() {
        return expr_class;
    }
    int get_optype() {
        return op_type;
    }
    bool is_right_op() {
        return is_right;
    }
    bool is_cond_op() {
        return is_cond;
    }
    void set_id(ast_id* i) {
        id1 = i;
        if (i != NULL) {
            i->set_parent(this);
            expr_class = GMEXPR_ID;
        }
    }
    void set_field(ast_field* f) {
        field = f;
        if (f != NULL) {
            f->set_parent(this);
            expr_class = GMEXPR_FIELD;
        }
    }
    bool is_type_conv() {
        return op_type == GMOP_TYPEC;
    }

    ast_expr* get_left_op() {
        return left;
    }
    ast_expr* get_right_op() {
        return right;
    }
    ast_expr* get_up_op() {
        return up;
    } // same to parent. but expr
    ast_expr* get_cond_op() {
        return cond;
    }
    void set_left_op(ast_expr* l) {
        left = l;
        if (l != NULL) {
            l->set_parent(this);
            l->set_up_op(this);
        }
    }
    void set_right_op(ast_expr* r) {
        right = r;
        if (r != NULL) {
            r->set_parent(this);
            r->set_up_op(this);
        }
    }
    void set_up_op(ast_expr* e) {
        up = e;
    }
    void set_cond_op(ast_expr* e) {
        cond = e;
        if (e != NULL) {
            e->set_parent(this);
            e->set_up_op(this);
        }
    }

    void set_alternative_type(int i) {
        alternative_type_of_expression = i;
    }
    int get_alternative_type() {
        return alternative_type_of_expression;
    }

    void set_expr_class(int i) {
        expr_class = i;
    }
protected:
    gm_symtab_entry* bound_graph_sym; // used only during typecheck
};

class ast_expr_foreign : public ast_expr
{
public:
    ~ast_expr_foreign() {
        std::list<ast_node*>::iterator I;
        for (I = parsed_gm.begin(); I != parsed_gm.end(); I++) {
            delete *I;
        }
        delete[] orig_text;
    }

    static ast_expr_foreign* new_expr_foreign(char* text) {
        ast_expr_foreign* aef = new ast_expr_foreign();
        aef->expr_class = GMEXPR_FOREIGN;
        aef->orig_text = gm_strdup(text);
        aef->type_of_expression = GMTYPE_FOREIGN_EXPR;
        return aef;
    }
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual void reproduce(int id_level);

    std::list<ast_node*>& get_parsed_nodes() {
        return parsed_gm;
    }
    std::list<std::string>& get_parsed_text() {
        return parsed_foreign;
    }

    void parse_foreign_syntax();
private:
    ast_expr_foreign() :
            orig_text(NULL) {
        set_nodetype(AST_EXPR_FOREIGN);
    }
    char* orig_text;

    // parsed foreign syntax
    std::list<ast_node*> parsed_gm;
    std::list<std::string> parsed_foreign;
    void apply_id(gm_apply*a, bool apply2);
    void apply_rhs(gm_apply*a, bool apply2);
};

class ast_expr_builtin : public ast_expr
{
public:
    ~ast_expr_builtin() {
        delete[] orgname;
        delete driver;
        std::list<ast_expr*>::iterator I;
        for (I = args.begin(); I != args.end(); I++)
            delete *I;
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual ast_expr* copy(bool cp_syminfo = false);

    static ast_expr_builtin* new_builtin_expr(ast_id* id, const char* orgname, expr_list* t) {
        ast_expr_builtin* E = new ast_expr_builtin();
        E->expr_class = GMEXPR_BUILTIN;
        E->driver = id;
        if (id != NULL) id->set_parent(E); // type unknown yet.
        E->orgname = gm_strdup(orgname);
        if (t != NULL) {
            E->args = t->LIST;  // shallow copy LIST
            // but not set 'up' pointer. 
            std::list<ast_expr*>::iterator I;
            for (I = E->args.begin(); I != E->args.end(); I++)
                (*I)->set_parent(E);
            delete t; // t is only temporary, delete it.
        }
        return E;
    }

    // defined in gm_builtin.cc
    static ast_expr_builtin* new_builtin_expr(ast_id* id, gm_builtin_def* d, expr_list* t);

    char* get_orgname() {
        return orgname;
    }
    char* get_callname() {
        return orgname;
    }
    ast_id* get_driver() {
        return driver;
    }
    void set_driver(ast_id* i) {
        driver = i;
        i->set_parent(this);
    }
    std::list<ast_expr*>& get_args() {
        return args;
    }
    gm_builtin_def* get_builtin_def() {
        return def;
    }
    void set_builtin_def(gm_builtin_def* d) {
        def = d;
    }

protected:
    ast_expr_builtin() :
            ast_expr(), driver(NULL), orgname(NULL), def(NULL) {
        set_nodetype(AST_EXPR_BUILTIN);
    }

protected:
    ast_id* driver;  // canbe null
    char* orgname;
    std::list<ast_expr*> args;
    gm_builtin_def* def;
};

// Reduction expression
class ast_expr_reduce : public ast_expr
{
public:
    ~ast_expr_reduce() {
        delete iter;
        delete body;
        delete filter;
        delete src;
        delete src2;
        delete_symtabs();
    }
    static ast_expr_reduce*
    new_reduce_expr(int optype, ast_id* iter, ast_id* src, int iter_op, ast_expr* body, ast_expr* filter = NULL) {
        ast_expr_reduce *e = new ast_expr_reduce();
        e->iter = iter;
        e->body = body;
        e->filter = filter;
        e->src = src;
        e->expr_class = GMEXPR_REDUCE;
        e->reduce_type = optype;
        e->iter_type = iter_op;

        iter->set_parent(e);
        src->set_parent(e);
        body->set_parent(e);
        body->set_up_op(e);
        if (filter != NULL) {
            filter->set_parent(e);
            filter->set_up_op(e);
        }

        return e;
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse(gm_apply*a, bool is_post, bool is_pre);
    virtual bool has_scope() {
        return true;
    }

    int get_iter_type() {
        return iter_type;
    }
    void set_iter_type(int i) {
        iter_type = i;
    }
    int get_reduce_type() {
        return reduce_type;
    }
    ast_id* get_source() {
        return src;
    }
    ast_id* get_iterator() {
        return iter;
    }
    ast_expr* get_filter() {
        return filter;
    }
    ast_expr* get_body() {
        return body;
    }
    ast_id* get_source2() {
        return src2;
    }

    void set_source2(ast_id* i) {
        src2 = i;
        if (i != NULL) i->set_parent(this);
    }
    void set_filter(ast_expr* e) {
        filter = e;
        if (e != NULL) e->set_parent(this);
    }
    void set_body(ast_expr* e) {
        body = e;
        if (e != NULL) e->set_parent(this);
    }
    virtual ast_expr* copy(bool cp_syminfo = false);

private:
    ast_expr_reduce() :
            ast_expr(), iter(NULL), src(NULL), src2(NULL), body(NULL), filter(NULL), reduce_type(GMREDUCE_NULL) {
        set_nodetype(AST_EXPR_RDC);
        create_symtabs();
    }
    ast_id* iter;
    ast_id* src;
    ast_id* src2;
    ast_expr* body;
    ast_expr* filter;
    int reduce_type;
    int iter_type;
};

//-------------------------------------------------------
// Assignments
//-------------------------------------------------------
enum
{
    GMASSIGN_NORMAL, GMASSIGN_REDUCE, GMASSIGN_DEFER, GMASSIGN_INVALID
};
enum
{
    GMASSIGN_LHS_SCALA, GMASSIGN_LHS_FIELD, GMASSIGN_LHS_END
};

class ast_assign : public ast_sent
{
public:
    virtual ~ast_assign() {
        delete lhs_scala;
        delete lhs_field;
        delete rhs;
        delete bound;
    }

public:
    static ast_assign* new_assign_scala(ast_id* id, ast_expr* r, int assign_type = GMASSIGN_NORMAL, ast_id* itor = NULL, int reduce_type = GMREDUCE_NULL) {
        // assign to scala
        ast_assign* A = new ast_assign();
        A->lhs_scala = id;
        A->rhs = r;
        id->set_parent(A);
        r->set_parent(A);
        A->lhs_type = GMASSIGN_LHS_SCALA;
        if (itor != NULL) {
            itor->set_parent(A);
        }
        A->bound = itor;
        A->assign_type = assign_type; // normal, reduced, or deferred
        A->reduce_type = reduce_type; // reduce or defer type
        return A;
    }
    static ast_assign* new_assign_field(ast_field* id, ast_expr* r, int assign_type = GMASSIGN_NORMAL, ast_id* itor = NULL, int reduce_type = GMREDUCE_NULL) {
        // assign to property
        ast_assign* A = new ast_assign();
        A->lhs_field = id;
        A->rhs = r;
        id->set_parent(A);
        r->set_parent(A);
        A->lhs_type = GMASSIGN_LHS_FIELD;
        if (itor != NULL) {
            itor->set_parent(A);
        }
        A->bound = itor;
        A->assign_type = assign_type;
        A->reduce_type = reduce_type;
        return A;
    }

    virtual void reproduce(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
    virtual void dump_tree(int id_level);

    int get_assign_type() {
        return assign_type;
    }
    int get_lhs_type() {
        return lhs_type;
    }
    int get_reduce_type() {
        return reduce_type;
    }
    void set_assign_type(int a) {
        assign_type = a;
    }
    void set_reduce_type(int a) {
        reduce_type = a;
    }

    ast_id* get_lhs_scala() {
        return lhs_scala;
    }
    ast_field* get_lhs_field() {
        return lhs_field;
    }
    ast_expr* get_rhs() {
        return rhs;
    }
    ast_id* get_bound() {
        return bound;
    }
    void set_bound(ast_id* i) {
        bound = i;
        if (bound != NULL) i->set_parent(this);
    }
    bool is_reduce_assign() {
        return assign_type == GMASSIGN_REDUCE;
    }
    bool is_defer_assign() {
        return assign_type == GMASSIGN_DEFER;
    }
    bool is_target_scalar() {
        return get_lhs_type() == GMASSIGN_LHS_SCALA;
    }
    void set_rhs(ast_expr* r) {
        rhs = r;
        rhs->set_parent(this);
    }

    bool is_argminmax_assign() {
        return arg_minmax;
    }
    void set_argminmax_assign(bool b) {
        arg_minmax = b;
    }

    bool has_lhs_list() {
        return l_list.size() > 0;
    }
    std::list<ast_node*>& get_lhs_list() {
        return l_list;
    }
    std::list<ast_expr*>& get_rhs_list() {
        return r_list;
    }
    void set_lhs_list(std::list<ast_node*>& L) {
        l_list = L;
    }
    void set_rhs_list(std::list<ast_expr*>& R) {
        r_list = R;
    }

    void set_lhs_scala(ast_id* new_id) {
        lhs_scala = new_id;
        if (new_id != NULL) lhs_type = GMASSIGN_LHS_SCALA;
    }
    void set_lhs_field(ast_field* new_id) {
        lhs_field = new_id;
        if (new_id != NULL) lhs_type = GMASSIGN_LHS_FIELD;
    }

private:
    ast_assign() :
            ast_sent(AST_ASSIGN), lhs_scala(NULL), lhs_field(NULL), rhs(NULL), bound(NULL), arg_minmax(false) {
    }

    int assign_type; // normal, deferred, reduce
    int lhs_type; // scalar, field
    int reduce_type; // add, mult, min, max
    ast_id* lhs_scala;
    ast_field* lhs_field;
    ast_expr* rhs;
    ast_id* bound;  // bounding iterator

    bool arg_minmax;

    std::list<ast_node*> l_list;
    std::list<ast_expr*> r_list;

};

class ast_vardecl : public ast_sent
{
public:
    virtual ~ast_vardecl() {
        delete idlist;
        delete type;
        //assert(init_expr == NULL);
    }
private:
    ast_vardecl() :
            ast_sent(AST_VARDECL), idlist(NULL), type(NULL), init_expr(NULL), tc_finished(false) {
    }

public:
    void set_typechecked(bool b) {
        tc_finished = b;
    }
    static ast_vardecl* new_vardecl(ast_typedecl* type, ast_idlist* id) {
        ast_vardecl* d = new ast_vardecl();
        d->idlist = id;
        d->type = type;
        id->set_parent(d);
        type->set_parent(d);
        return d;
    }
    static ast_vardecl* new_vardecl(ast_typedecl* type, ast_id* id) {
        ast_vardecl* d = new ast_vardecl();
        ast_idlist* idl = new ast_idlist();
        idl->add_id(id);
        d->idlist = idl;
        d->type = type;
        idl->set_parent(d);
        type->set_parent(d);
        return d;
    }
    static ast_vardecl* new_vardecl_init(ast_typedecl* type, ast_id* id, ast_expr* init) {
        ast_vardecl* d = new ast_vardecl();
        ast_idlist* idl = new ast_idlist();
        idl->add_id(id);
        d->idlist = idl;
        d->type = type;
        d->init_expr = init;
        id->set_parent(d);
        type->set_parent(d);
        if (init != NULL) init->set_parent(d);
        return d;
    }
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    ast_idlist* get_idlist() {
        return idlist;
    }
    ast_typedecl* get_type() {
        if (!tc_finished)
            return type;           // obtain type from syntax
        else
            return idlist->get_item(0)->getTypeInfo(); // obtain type from symbol table
    }
    ast_expr* get_init() {
        return init_expr;
    }
    void set_init(ast_expr* v) {
        init_expr = v;
        if (v != NULL) v->set_parent(this);
    }

    bool is_tc_finished() {
        return tc_finished;
    }
    void set_tc_finished(bool b) {
        tc_finished = b;
    } // who calls it?

private:
    ast_idlist* idlist;
    ast_typedecl* type;
    ast_expr* init_expr; // for syntax sugar.
    bool tc_finished;
};

class ast_return : public ast_sent
{
protected:
    ast_return() :
            ast_sent(AST_RETURN), expr(NULL) {
    }
    ast_expr* expr;

public:
    ~ast_return() {
        delete expr;
    }
    static ast_return* new_return(ast_expr* e) {
        ast_return* R = new ast_return();
        R->expr = e;
        if (e != NULL) e->set_parent(R);
        return R;
    }
    ast_expr* get_expr() {
        return expr;
    }
    void set_expr(ast_expr*e) {
        expr = e;
        if (e != NULL) e->set_parent(this);
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
};

class ast_foreach : public ast_sent
{
public:
    virtual ~ast_foreach() {
        delete body;
        delete iterator;
        delete source;
        delete source2;
        delete cond;
        //delete symbol info
        delete_symtabs();
    }
private:
    ast_foreach() :
            ast_sent(AST_FOREACH), body(NULL), iterator(NULL), source(NULL), source2(NULL), cond(NULL), seq_exe(false), use_reverse(false) {
        create_symtabs();
    }

public:
    // iterate on a graph
    static ast_foreach* new_foreach(ast_id* it, ast_id* src, ast_sent* b, int iter_type, ast_expr* cond = NULL) {
        ast_foreach* d = new ast_foreach();
        d->iterator = it;
        d->source = src;
        d->body = b;
        d->iter_type = iter_type;
        d->cond = cond;
        src->set_parent(d);
        it->set_parent(d);
        b->set_parent(d);
        if (cond != NULL) cond->set_parent(d);
        return d;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    ast_id* get_source() {
        return source;
    }
    ast_id* get_iterator() {
        return iterator;
    }
    ast_sent* get_body() {
        return body;
    }
    ast_expr* get_filter() {
        return cond;
    }
    int get_iter_type() {
        return iter_type;
    } // GM_ITERATORS
    void set_iter_type(int i) {
        iter_type = i;
    } // GM_ITERATORS
      // should be same to get_iterator()->get_type_summary()
    ast_id* get_source2() {
        return source2;
    }
    void set_source2(ast_id* i) {
        source2 = i;
        if (i != NULL) i->set_parent(this);
    }
    void set_filter(ast_expr* expr = NULL) {
        cond = expr;
        if (cond != NULL) cond->set_parent(this);
    }
    void set_body(ast_sent* s) {
        body = s;
        assert(body!=NULL);
        body->set_parent(this);
    }
    virtual bool has_scope() {
        return true;
    }

    // For is sequential while FOREACH is parallel.
    // Optimization may overrride parallel exeuction with sequential.
    bool is_sequential() {
        return seq_exe;
    } // sequential execution
    void set_sequential(bool b) {
        seq_exe = b;
    }
    bool is_parallel() {
        return !is_sequential();
    } // sequential execution

    // for set iterator
    bool is_reverse_iteration() {
        return use_reverse;
    }
    void set_reverse_iteration(bool b) {
        use_reverse = b;
    }

private:
    ast_sent* body;
    ast_id* iterator;
    ast_id* source; // graph
    ast_id* source2; // common nbr
    int iter_type; // GM_ITERATORS
    ast_expr* cond;
    bool seq_exe;
    bool use_reverse;
};

// BFS or DFS
class ast_bfs : public ast_sent
{
public:
    ~ast_bfs() {
        delete f_body;
        delete b_body;
        delete f_filter;
        delete b_filter;
        delete navigator;
        delete iter;
        delete src;
        delete root;
        delete iter2;
        delete_symtabs();
    }

    static ast_bfs* new_bfs(ast_id* it, ast_id* src, ast_id* root, ast_expr* navigator, ast_expr* f_filter, ast_expr* b_filter, ast_sentblock* fb,
            ast_sentblock* bb, bool use_tp, bool is_bfs = true) {
        ast_bfs* d = new ast_bfs();
        d->iter = it;
        d->src = src;
        d->root = root;
        d->f_body = fb;
        d->b_body = bb;
        d->use_transpose = use_tp;
        d->b_filter = b_filter;
        d->f_filter = f_filter;
        d->navigator = navigator;
        d->_bfs = is_bfs;

        src->set_parent(d);
        it->set_parent(d);
        root->set_parent(d);
        if (fb != NULL) fb->set_parent(d);
        if (bb != NULL) bb->set_parent(d);
        if (navigator != NULL) navigator->set_parent(d);
        if (f_filter != NULL) f_filter->set_parent(d);
        if (b_filter != NULL) b_filter->set_parent(d);
        return d;
    }

    ast_sentblock* get_fbody() {
        return f_body;
    }
    ast_sentblock* get_bbody() {
        return b_body;
    }
    ast_expr* get_navigator() {
        return navigator;
    }
    ast_expr* get_f_filter() {
        return f_filter;
    }
    ast_expr* get_b_filter() {
        return b_filter;
    }
    ast_id* get_iterator() {
        return iter;
    }
    ast_id* get_iterator2() {
        return iter2;
    }
    ast_id* get_source() {
        return src;
    }
    ast_id* get_root() {
        return root;
    }
    bool is_transpose() {
        return use_transpose;
    }
    bool is_bfs() {
        return _bfs;
    }

    void set_iterator2(ast_id* id) {
        assert(iter2 == NULL);
        iter2 = id;
    }
    void set_navigator(ast_expr* e) {
        if (e != NULL) e->set_parent(this);
        navigator = e;
    }
    void set_f_filter(ast_expr* e) {
        if (e != NULL) e->set_parent(this);
        f_filter = e;
    }
    void set_b_filter(ast_expr* e) {
        if (e != NULL) e->set_parent(this);
        b_filter = e;
    }
    void set_fbody(ast_sentblock* b) {
        f_body = b;
    }
    void set_bbody(ast_sentblock* b) {
        b_body = b;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
    int get_iter_type() {
        return GMTYPE_NODEITER_BFS;
    }
    int get_iter_type2() {
        return is_transpose() ? GMTYPE_NODEITER_IN_NBRS : GMTYPE_NODEITER_NBRS;
    }

    virtual bool has_scope() {
        return true;
    }

    // currently BFS is always parallel. (codegen assumes there is only one BFS. also flip-edge opt does)
    bool is_sequential() {
        return !is_bfs();
    } // sequential execution
    bool is_parallel() {
        return is_bfs();
    } // sequential execution

protected:
    ast_bfs() :
            ast_sent(AST_BFS), f_body(NULL), b_body(NULL), f_filter(NULL), b_filter(NULL), navigator(NULL), iter(NULL), src(NULL), root(NULL), iter2(NULL), use_transpose(
                    false), _bfs(true) {
        create_symtabs();
    }

private:
    ast_sentblock* f_body;
    ast_sentblock* b_body;

    ast_expr* f_filter;
    ast_expr* b_filter;
    ast_expr* navigator;

    ast_id* iter;
    ast_id* src;
    ast_id* root;
    ast_id* iter2; // iterator used for frontier expansion [xxx] what?
    bool use_transpose;
    bool _bfs;
};

class ast_call : public ast_sent
{

public:
    virtual ~ast_call() {
        delete b_in;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    static ast_call* new_builtin_call(ast_expr_builtin* b) {
        ast_call* C = new ast_call();
        b->set_parent(C);
        C->b_in = b;
        C->is_blt_in = true;
        return C;
    }

private:
    ast_call() :
            ast_sent(AST_CALL), b_in(NULL), is_blt_in(false) {
    }

public:
    ast_expr_builtin* get_builtin() {
        return b_in;
    }
    void set_builtin(ast_expr_builtin* b) {
        b_in = b;
        assert(b_in!=NULL);
        b_in->set_parent(this);
    }
    bool is_builtin_call() {
        return is_blt_in;
    }

private:
    ast_expr_builtin* b_in;
    bool is_blt_in;
};

class ast_if : public ast_sent
{
public:
    virtual ~ast_if() {
        delete cond;
        delete then_part;
        delete else_part;
    }
private:
    ast_if() :
            ast_sent(AST_IF), then_part(NULL), else_part(NULL), cond(NULL) {
    }

public:
    static ast_if* new_if(ast_expr* c, ast_sent* t, ast_sent* e) {
        ast_if* ifs = new ast_if();
        ifs->then_part = t;
        ifs->else_part = e;
        ifs->cond = c;
        c->set_parent(ifs);
        t->set_parent(ifs);
        if (e != NULL) e->set_parent(ifs);
        return ifs;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    ast_sent* get_then() {
        return then_part;
    }
    ast_sent* get_else() {
        return else_part;
    }
    ast_expr* get_cond() {
        return cond;
    }
    void set_then(ast_sent* s) {
        then_part = s;
    }
    void set_else(ast_sent* s) {
        else_part = s;
    }
    void set_cond(ast_expr* c) {
        cond = c;
        if (c != NULL) c->set_parent(this);
    }

private:
    ast_sent* then_part;
    ast_sent* else_part;
    ast_expr* cond;
};

class ast_foreign : public ast_sent
{
public:
    virtual ~ast_foreign() {
        delete expr;
        std::list<ast_node*>::iterator I;
        for (I = modified.begin(); I != modified.end(); I++)
            delete *I;
    }

    static ast_foreign* new_foreign(ast_expr_foreign* f) {
        ast_foreign* S = new ast_foreign();
        S->set_expr(f);

        f->set_parent(S);
        return (S);
    }

    static ast_foreign* new_foreign_mutate(ast_expr_foreign* f, lhs_list* l) {
        ast_foreign* S = new ast_foreign();
        S->set_expr(f);
        f->set_parent(S);
        std::list<ast_node*>&L = l->LIST;
        std::list<ast_node*>::iterator I;
        for (I = L.begin(); I != L.end(); I++) {
            ast_node* n = *I;
            n->set_parent(S);
            S->modified.push_back(n);
        }
        delete l;
        return (S);
    }

    virtual void reproduce(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);
    virtual void dump_tree(int id_level) {
    }

    std::list<ast_node*>& get_modified() {
        return modified;
    }
    ast_expr_foreign* get_expr() {
        return expr;
    }
    void set_expr(ast_expr_foreign* f) {
        expr = f;
    }

private:
    ast_foreign() :
            ast_sent(AST_FOREIGN), expr(NULL) {
    }
    ast_expr_foreign* expr;
    std::list<ast_node*> modified;
};

class ast_while : public ast_sent
{
public:
    virtual ~ast_while() {
        delete body;
        delete cond;
    }
private:
    ast_while() :
            ast_sent(AST_WHILE), body(NULL), cond(NULL), do_while(false) {
    }

public:
    static ast_while* new_while(ast_expr* c, ast_sentblock* s) {
        ast_while* w = new ast_while();
        w->cond = c;
        w->body = s;
        w->do_while = false;
        c->set_parent(w);
        s->set_parent(w);
        return w;
    }
    static ast_while* new_do_while(ast_expr* c, ast_sentblock* s) {
        ast_while* w = new ast_while();
        w->cond = c;
        w->body = s;
        w->do_while = true;
        c->set_parent(w);
        s->set_parent(w);
        return w;
    }

    // rotuines that should be implemented
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    ast_sent* get_body() {
        return body;
    }
    ast_expr* get_cond() {
        return cond;
    }
    bool is_do_while() {
        return do_while;
    }
    void set_body(ast_sentblock* s) {
        body = s;
    }
    void set_cond(ast_expr* e) {
        cond = e;
        if (e != NULL) {
            e->set_parent(this);
        }
    }

private:
    ast_sentblock* body;
    ast_expr* cond;
    bool do_while; // if true do_while, else while
};

// a dummy nop IR.
// May be used in back-end processing
class gm_symtab_entry;
//class gm_rwinfo;
//typedef std::list<gm_rwinfo*> gm_rwinfo_list;
//typedef std::map<gm_symtab_entry*, gm_rwinfo_list*> gm_rwinfo_map;
class ast_nop : public ast_sent
{
protected:
    ast_nop() :
            ast_sent(AST_NOP) {
    }
    ast_nop(int t) :
            ast_sent(AST_NOP) {
        set_subtype(t);
    }
public:
    virtual ~ast_nop() {
    }
    int get_subtype() {
        return subtype;
    }
    void set_subtype(int s) {
        subtype = s;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre) {
    }
    virtual bool do_rw_analysis() {
        return true;
    }
private:
    int subtype;
};

#endif
