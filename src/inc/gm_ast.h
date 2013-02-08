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

extern const char* gm_get_type_string(int i);

enum AST_NODE_TYPE
{
    AST_ID,       // 
    AST_FIELD,    // A.B
    AST_MAPACCESS,    // A[B]
    AST_IDLIST,   // A, B, C, 
    AST_TYPEDECL, // INT
    AST_ARGDECL,  // a,b : B
    AST_PROCDEF,  // proc A() {}
    AST_EXPR,     // c + 3
    AST_EXPR_RDC,       // c + 3
    AST_EXPR_BUILTIN,   // c + 3
    AST_EXPR_FOREIGN,  // Foreign Expression
    AST_EXPR_MAPACCESS,
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
class ast_id;
class ast_field;
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

class ast_extra_info_string: public ast_extra_info
{
public:
    char* str;
    ast_extra_info_string();
    virtual ~ast_extra_info_string();

    ast_extra_info_string(const char* org);
    const char* get_string();
    virtual ast_extra_info* copy();
};

class ast_extra_info_set: public ast_extra_info
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

class ast_extra_info_list: public ast_extra_info
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

class ast_extra_info_map: public ast_extra_info
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
    ast_node(AST_NODE_TYPE nt) :
            nodetype(nt), parent(NULL), line(0), col(0), sym_vars(NULL), sym_procs(NULL), sym_fields(NULL) {
    }

    ast_node() :
            nodetype(AST_END), parent(NULL), line(0), col(0), sym_vars(NULL), sym_procs(NULL), sym_fields(NULL) {
    }

    AST_NODE_TYPE nodetype;
    ast_node* parent;

public:
    void set_nodetype(AST_NODE_TYPE nt) {
        nodetype = nt;
    }

    virtual ~ast_node() {
        std::map<std::string, ast_extra_info*>::iterator i;
        for (i = extra.begin(); i != extra.end(); i++) {
            delete i->second;
        }
        extra.clear();
    }

    AST_NODE_TYPE get_nodetype() {
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
        return (nodetype == AST_SENTBLOCK) || (nodetype == AST_FOREACH) || (nodetype == AST_PROCDEF) || (nodetype == AST_EXPR_RDC) || (nodetype == AST_BFS);
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

    // interafce for iteration definining ast-nodes
    virtual void set_iter_type(int i) {assert(false);}
    virtual int  get_iter_type()      {assert(false);return 0;}
    virtual ast_id* get_source()      {assert(false);return NULL;}
    virtual ast_id* get_source2()     {assert(false);return NULL;}
    virtual bool is_source_field()    {return false;}
    virtual ast_field* get_source_field() {return NULL;}


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
class ast_id: public ast_node
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
        cp->set_instant_assigned(is_instantly_assigned());
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
            ast_node(AST_ID), name(NULL), info(NULL), gen_name(NULL), instant_assignment(false) {
    }

    ast_id(const char* org, int l, int c) :
            ast_node(AST_ID), info(NULL), gen_name(NULL), instant_assignment(false) {
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

    bool is_instantly_assigned() {
        return instant_assignment;
    }

    void set_instant_assigned(bool value) {
        instant_assignment = value;
    }

public:
    char* name;

private:
    gm_symtab_entry* info;
    char* gen_name;

    bool instant_assignment;

    char* get_orgname_from_symbol();  // gm_typecheck.cc
    char* get_genname_from_symbol();  // gm_typecheck.cc
public:
    void use_names_from_symbol(); // gm_typecheck.cc

};

class ast_idlist: public ast_node
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
        assert(get_length() > 0);
        return lst[0]->get_line();
    }
    int get_col() {
        assert(get_length() > 0);
        return lst[0]->get_col();
    }

private:
    std::vector<ast_id*> lst;
};

class ast_field: public ast_node
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
        af->set_rarrow(is_r_arrow);
        return af;
    }
    // FIRST.SECOND
    ast_id* get_first() { // Identifier
        return first;
    }

    ast_id* get_second() { // Field
        return second;
    }

    bool is_rarrow() { // Is it Edge(x).y?
        return rarrow;
    }

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

class ast_typedecl: public ast_node
{  // property or type
protected:
    ast_typedecl() :
            ast_node(AST_TYPEDECL), target_type(NULL), target_graph(NULL), _well_defined(false), def_node(NULL), type_id(
                    0) {
    }

public:
    // give a deep copy, except defining node
    virtual ast_typedecl* copy() {
        ast_typedecl *p = new ast_typedecl();
        p->type_id = this->type_id;
        p->target_type = (this->target_type == NULL) ? NULL : this->target_type->copy();
        p->target_graph = (this->target_graph == NULL) ? NULL : this->target_graph->copy(true);
        p->def_node = this->def_node;
        p->line = this->line;
        p->col = this->col;
        p->_well_defined = this->_well_defined;

        return p;
    }

    virtual ~ast_typedecl() {
        delete target_type;
        delete target_graph; //gets deleted twice (sometimes) why??? o.O
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

    // node iterator, edge iterator, colllection iterator
    static ast_typedecl* new_iterator(ast_id* tg, int type, ast_node* defining_node) { 
        assert(gm_is_iterator_type(type));
        ast_typedecl* t = new ast_typedecl();
        t->type_id = type;
        t->def_node = defining_node;
        t->target_graph = tg;
        return t;
    }

    static ast_typedecl* new_collection(ast_id* tg, int set_type) { 
        assert(gm_is_simple_collection_type(set_type)); // No collection of collections
        ast_typedecl* t = new ast_typedecl();
        t->type_id = set_type;
        if (tg == NULL) //no graph defined for this set - we will handle this later (typecheck step 1)
            return t;
        t->target_graph = tg;
        tg->set_parent(t);
        return t;
    }

    static ast_typedecl* new_queue(ast_id* targetGraph, ast_typedecl* collectionType) {  // collection of collection
        ast_typedecl* typeDecl = new ast_typedecl();
        typeDecl->type_id = GMTYPE_COLLECTION_OF_COLLECTION;
        typeDecl->target_type = collectionType;
        if (targetGraph == NULL) return typeDecl; //no graph defined for this queue - we will handle this later (typecheck step 1)
        typeDecl->target_graph = targetGraph;
        targetGraph->set_parent(typeDecl);
        return typeDecl;
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

    virtual int get_typeid() {
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

    bool is_collection_of_collection() {
        return gm_is_collection_of_collection_type(type_id);
    }

    bool is_node_collection() {
        return gm_is_node_collection_type(type_id);
    }

    bool is_edge_collection() {
        return gm_is_edge_collection_type(type_id);
    }

    bool is_iterator() {
        return gm_is_iterator_type(type_id);
    }

    bool is_collection_iterator() { 
        return gm_is_collection_iterator_type(type_id);
    }

    bool is_node_iterator() {
        return gm_is_node_iterator_type(type_id);
    }

    bool is_edge_iterator() {
        return gm_is_edge_iterator_type(type_id);
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

    bool requires_target_graph() {
        return gm_requires_target_graph_type(type_id);
    }

    bool is_void() {
        return gm_is_void_type(type_id);
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

    bool is_inherently_unique_collection() {
        return gm_is_inherently_unique_collection_type(type_id);
    }

    virtual bool is_map() {
        return false;
    }

    //---------------------------------------------------------------
    // (assumption this->is_iterator): check the definining node
    // defined in frontend/gm_typecheck.cc
    //---------------------------------------------------------------
    int           get_defined_iteration_from_iterator(); 
    ast_id*       get_defined_source_from_iterator();  // to be depricated
    void          get_iteration_source_from_iterator(ast_id*& src_id, ast_field*& src_field);  

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);

    // there is no copying of type
    gm_symtab_entry* get_target_graph_sym() {

        if (is_collection() || is_property() || is_nodeedge() || 
            is_iterator() || is_collection_of_collection())  {
            assert(target_graph != NULL);
            assert(target_graph->getSymInfo() != NULL);
            return target_graph->getSymInfo();
        } else {
            printf("type = %s does not have target graph symbol\n", gm_get_type_string(type_id));
            assert(false);
            return NULL;
        }
    }

    ast_id* get_target_graph_id() {
        return target_graph;
    }

    ast_typedecl* get_target_type() {
        return target_type;
    }

    virtual int getTypeSummary() {  // same as get type id
        return type_id;
    }

    void setTypeSummary(int s) {
        // type id might be overriden during type-checking
        set_typeid(s);
    }

    int getTargetTypeSummary() {
        assert(is_property() || is_collection_of_collection());
        assert(target_type != NULL);
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

    ast_node*   get_definining_node() {
        return def_node;
    }

    void set_defining_node(ast_node* n)   {
        def_node = n ;
    }

private:
    // defined in gm_frontend_api.h
    ast_typedecl* target_type;        // for property
    ast_id*       target_graph;       // for property, node, edge, set
    ast_node*     def_node;           // The foreach (bfs,reduce) statement that defines this iterator

protected:
    int type_id;
    bool _well_defined;
};

class ast_maptypedecl: public ast_typedecl
{

private:
    ast_typedecl* keyType;
    ast_typedecl* valueType;

    ast_maptypedecl() :
            ast_typedecl(), keyType(NULL), valueType(NULL) {
    }

public:

    ~ast_maptypedecl() {
        delete keyType;
        delete valueType;
    }

    virtual void reproduce(int id_level);

    static ast_maptypedecl* new_map(ast_typedecl* keyType, ast_typedecl* valueType) {
        ast_maptypedecl* newMap = new ast_maptypedecl();
        newMap->type_id = GMTYPE_MAP;
        newMap->keyType = keyType;
        newMap->valueType = valueType;
        keyType->set_parent(newMap);
        valueType->set_parent(newMap);
        return newMap;
    }

    ast_typedecl* copy() {
        ast_maptypedecl* clone = new ast_maptypedecl();
        clone->type_id = type_id;
        clone->keyType = (keyType == NULL) ? NULL : keyType->copy();
        clone->valueType = (valueType == NULL) ? NULL : valueType->copy();
        clone->line = line;
        clone->col = col;
        clone->_well_defined = this->_well_defined;
        return clone;
    }

    void set_key_type(ast_typedecl* newKeyType) {
        assert(gm_can_be_key_type((GMTYPE_T)newKeyType->getTypeSummary()));
        keyType = newKeyType;
    }

    ast_typedecl* get_key_type() {
        return keyType;
    }

    ast_typedecl* get_value_type() {
        return valueType;
    }

    void set_value_type(ast_typedecl* newValueType) {
        assert(gm_can_be_key_type((GMTYPE_T)newValueType->getTypeSummary()));
        valueType = newValueType;
    }

    bool is_map() {
        return true;
    }

    int get_typeid() {
        return (int) GMTYPE_MAP;
    }

    int getTypeSummary() {
        return get_typeid();
    }

    int getKeyTypeSummary() {
        assert(keyType != NULL);
        return keyType->getTypeSummary();
    }

    int getValueTypeSummary() {
        assert(valueType != NULL);
        return valueType->getTypeSummary();
    }

};

//==========================================================================
class ast_sent: public ast_node
{
protected:
    ast_sent(AST_NODE_TYPE y) :
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
            ast_node(AST_SENT), eline(0), _par(false) {
    }

    int eline;
    bool _par;

};

extern const char* gm_get_nodetype_string(int t);

class ast_sentblock: public ast_sent
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

class ast_argdecl: public ast_node
{
public:
    virtual ~ast_argdecl() {
        delete idlist;
        delete type;
    }
private:
    ast_argdecl() :
            ast_node(AST_ARGDECL), tc_finished(false), idlist(NULL), type(NULL) {
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
class ast_procdef: public ast_node
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
enum GMEXPR_CLASS
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
    GMEXPR_BUILTIN_FIELD, //builtin ops on property entries
    GMEXPR_TER,      // ternary operation
    GMEXPR_FOREIGN,
    GMEXPR_MAPACCESS,
// foreign expression
};

// Numeric or boolean expression
class gm_builtin_def;
// defined in gm_builtin.h

class ast_expr: public ast_node
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
    virtual ast_expr* copy(bool cp_syminfo = false);

    virtual bool is_expr() {
        return true;
    }

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

    GMEXPR_CLASS expr_class;  // GMEXPR_...
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
        return expr_class == GMEXPR_BUILTIN || expr_class == GMEXPR_BUILTIN_FIELD;
    }

    bool is_terop() {
        return expr_class == GMEXPR_TER;
    }

    bool is_foreign() {
        return expr_class == GMEXPR_FOREIGN;
    }

    virtual bool is_mapaccess() {
        return false;
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
        if (bound_graph_sym == NULL && is_id())
            return id1->getTypeInfo()->get_target_graph_sym();
        else
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

    virtual ast_id* get_id() {
        return id1;
    }

    virtual ast_field* get_field() {
        return field;
    }

    virtual GMEXPR_CLASS get_opclass() {
        return expr_class;
    }

    virtual int get_optype() {
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

    void set_expr_class(GMEXPR_CLASS ec) {
        expr_class = ec;
    }

protected:
    gm_symtab_entry* bound_graph_sym; // used only during typecheck
};

class ast_mapaccess: public ast_node
{
private:
    ast_id* mapId;
    ast_expr* keyExpr;

    gm_symtab_entry* keyGraph;
    gm_symtab_entry* valueGraph;

    ast_mapaccess() :
            ast_node(AST_MAPACCESS), mapId(NULL), keyExpr(NULL), keyGraph(NULL), valueGraph(NULL) {
    }

    ast_mapaccess(ast_id* map, ast_expr* key) :
            ast_node(AST_MAPACCESS), mapId(map), keyExpr(key), keyGraph(NULL), valueGraph(NULL) {
    }

public:
    ~ast_mapaccess() {
        delete mapId;
        delete keyExpr;
    }

    virtual void dump_tree(int i);
    virtual void reproduce(int i);

    ast_mapaccess* copy(bool cp_sym = false) {
        ast_mapaccess* clone = new ast_mapaccess();
        clone->mapId = mapId->copy(cp_sym);
        clone->keyExpr = keyExpr->copy(cp_sym);
        return clone;
    }

    ast_id* get_map_id() {
        assert(mapId != NULL);
        return mapId;
    }

    ast_expr* get_key_expr() {
        assert(keyExpr != NULL);
        return keyExpr;
    }

    gm_symtab_entry* get_bound_graph_for_key() {
        return keyGraph;
    }

    void set_bound_graph_for_key(gm_symtab_entry* graphEntry) {
        keyGraph = graphEntry;
    }

    gm_symtab_entry* get_bound_graph_for_value() {
        return valueGraph;
    }

    void set_bound_graph_for_value(gm_symtab_entry* graphEntry) {
        valueGraph = graphEntry;
    }

    static ast_mapaccess* new_mapaccess(ast_id* map, ast_expr* key) {
        ast_mapaccess* newMapAccess = new ast_mapaccess(map, key);
        assert(newMapAccess->keyExpr != NULL);
        return newMapAccess;
    }

};

class ast_expr_mapaccess: public ast_expr
{
private:
    ast_mapaccess* mapAccess;

    ast_expr_mapaccess() :
            ast_expr(), mapAccess(NULL) {
        set_nodetype(AST_EXPR_MAPACCESS);
    }

    ast_expr_mapaccess(ast_mapaccess* mapAccess, int line, int column) :
            ast_expr(), mapAccess(mapAccess) {
        set_nodetype(AST_EXPR_MAPACCESS);
        set_line(line);
        set_col(column);
    }

public:
    ~ast_expr_mapaccess() {
        delete mapAccess;
    }

    ast_expr_mapaccess* copy(bool cp_sym = false) {
        ast_expr_mapaccess* clone = new ast_expr_mapaccess();
        clone->mapAccess = mapAccess->copy(cp_sym);
        return clone;
    }

    virtual void reproduce(int ind_level);

    bool is_mapaccess() {
        return true;
    }

    int get_optype() {
        return (int) GMOP_MAPACCESS;
    }

    GMEXPR_CLASS get_opclass() {
        return GMEXPR_MAPACCESS;
    }

    ast_id* get_id() {
        return mapAccess->get_map_id();
    }

    ast_mapaccess* get_mapaccess() {
        assert(mapAccess != NULL);
        return mapAccess;
    }

    static ast_expr_mapaccess* new_expr_mapaccess(ast_mapaccess* mapAccess, int line, int column) {
        ast_expr_mapaccess* newMapAccess = new ast_expr_mapaccess(mapAccess, line, column);
        return newMapAccess;
    }

};

class ast_expr_foreign: public ast_expr
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

class ast_expr_builtin: public ast_expr
{
public:
    virtual ~ast_expr_builtin() {
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

    virtual bool driver_is_field() {
        return false;
    }

    virtual int get_source_type() {
        return (driver == NULL) ? GMTYPE_VOID : driver->getTypeSummary();
    }
    virtual int get_source_iteration() {
        return (driver == NULL) ?  GMITER_ANY :
               (driver->getTypeInfo()->is_iterator()) ?
                driver->getTypeInfo()->get_defined_iteration_from_iterator():
                GMITER_ANY;
    }

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

    virtual ast_id* get_driver() {
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

class ast_expr_builtin_field: public ast_expr_builtin
{

public:
    virtual void reproduce(int id_level);

    ~ast_expr_builtin_field() {
        delete field_driver;
    }

    ast_id* get_driver() {
        assert(false);
        return NULL;
    }

    bool driver_is_field() {
        return true;
    }

    ast_field* get_field_driver() {
        return field_driver;
    }

    ast_field* get_field() {
        return field_driver;
    }

    int get_source_type() {
        assert(field_driver != NULL);
        return field_driver->get_second()->getTargetTypeInfo()->getTypeSummary();
    }

    static ast_expr_builtin_field* new_builtin_field_expr(ast_field* field, const char* orgname, expr_list* exList) {

        ast_expr_builtin_field* newExpression = new ast_expr_builtin_field();
        newExpression->expr_class = GMEXPR_BUILTIN_FIELD;
        newExpression->field_driver = field;
        newExpression->orgname = gm_strdup(orgname);

        if (field != NULL) {
            field->set_parent(newExpression); // type unknown yet.
        }

        if (exList != NULL) {
            newExpression->args = exList->LIST;  // shallow copy LIST
            // but not set 'up' pointer.
            std::list<ast_expr*>::iterator iter;
            for (iter = newExpression->args.begin(); iter != newExpression->args.end(); iter++)
                (*iter)->set_parent(newExpression);
            delete exList; // t is only temporary, delete it.
        }
        return newExpression;
    }

private:
    ast_expr_builtin_field() :
            ast_expr_builtin(), field_driver(NULL) {
    }

    ast_field* field_driver;
};

// Reduction expression
class ast_expr_reduce: public ast_expr
{
public:
    ~ast_expr_reduce() {
        delete iter;
        delete body;
        delete filter;
        delete src;
        delete src2;
        delete src_field;
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
        e->is_src_field = false;

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
    static ast_expr_reduce*
    new_reduce_expr(int optype, ast_id* iter, ast_field* src_fld, int iter_op, ast_expr* body, ast_expr* filter = NULL) {
        ast_expr_reduce *e = new ast_expr_reduce();
        e->iter = iter;
        e->body = body;
        e->filter = filter;
        e->src_field = src_fld;
        e->expr_class = GMEXPR_REDUCE;
        e->reduce_type = optype;
        e->iter_type = iter_op;
        e->is_src_field = true;

        iter->set_parent(e);
        src_fld->set_parent(e);
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

    virtual int get_iter_type() {
        return iter_type;
    }

    virtual void set_iter_type(int i) {
        iter_type = i;
    }

    int get_reduce_type() {
        return reduce_type;
    }

    virtual ast_id* get_source() {
        assert(is_src_field == false);
        return src;
    }
    virtual ast_field* get_source_field() {
        assert(is_src_field == true);
        return src_field;
    }
    virtual bool is_source_field() {
        return is_src_field;
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

    virtual ast_id* get_source2() {
        return src2;
    }


    void set_source2(ast_id* i) {
        src2 = i;
        if (i != NULL) { i->set_parent(this);}
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
            ast_expr(), iter(NULL), src(NULL), src2(NULL), body(NULL), filter(NULL), reduce_type(GMREDUCE_NULL), iter_type(0), src_field(NULL), is_src_field(false) {
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
    bool is_src_field;
    ast_field* src_field;
};

//-------------------------------------------------------
// Assignments
//-------------------------------------------------------
enum gm_assignment_t
{
    GMASSIGN_NORMAL, GMASSIGN_REDUCE, GMASSIGN_DEFER, GMASSIGN_INVALID
};

enum gm_assignment_location_t
{
    GMASSIGN_LHS_SCALA, GMASSIGN_LHS_FIELD, GMASSIGN_LHS_MAP, GMASSIGN_LHS_END
};

class ast_assign_mapentry;

class ast_assign: public ast_sent
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

    virtual int get_lhs_type() {
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

    virtual bool is_target_scalar() {
        return get_lhs_type() == GMASSIGN_LHS_SCALA;
    }

    virtual bool is_target_field() {
        return get_lhs_type() == GMASSIGN_LHS_FIELD;
    }

    virtual bool is_target_map_entry() {
        return false;
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

    virtual bool is_map_entry_assign() {
        return false;
    }

    virtual ast_assign_mapentry* to_assign_mapentry() {
        assert(false);
        return NULL;
    }

    void set_is_reference(bool is_ref) {
        isReference = is_ref;
    }

    bool is_reference() {
        return isReference;
    }

protected:
    ast_assign() :
            ast_sent(AST_ASSIGN), lhs_scala(NULL), lhs_field(NULL), rhs(NULL), bound(NULL), arg_minmax(false), lhs_type(0), assign_type(0), reduce_type(0), isReference(false) {
    }

private:
    int assign_type; // normal, deferred, reduce
    int lhs_type; // scalar, field
    int reduce_type; // add, mult, min, max
    ast_id* lhs_scala;
    ast_field* lhs_field;

    ast_id* bound;  // bounding iterator

    bool arg_minmax;
    bool isReference;

    std::list<ast_node*> l_list;
    std::list<ast_expr*> r_list;

protected:
    ast_expr* rhs;

};

class ast_assign_mapentry : public ast_assign {
private:
    ast_mapaccess* lhs;

    ast_assign_mapentry(ast_mapaccess* lhs, ast_expr* rhs) : ast_assign(), lhs(lhs) {
        this->rhs = rhs;
    }

    ast_assign_mapentry(ast_mapaccess* lhs, ast_expr* rhs, int reduceType) : ast_assign(), lhs(lhs) {
        this->rhs = rhs;
        set_reduce_type(reduceType);
        set_assign_type(GMASSIGN_REDUCE);
    }

public:
    ~ast_assign_mapentry() {
        delete lhs;
    }

    virtual void reproduce(int indLevel);

    void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    bool is_map_entry_assign() {
        return true;
    }

    bool is_target_map_entry() {
        return true;
    }

    bool is_target_scalar() {
        return false;
    }

    virtual int get_lhs_type() {
        return GMASSIGN_LHS_MAP;
    }

    ast_assign_mapentry* to_assign_mapentry() {
        return this;
    }

    ast_mapaccess* get_lhs_mapaccess() {
        return lhs;
    }

    static ast_assign_mapentry* new_mapentry_assign(ast_mapaccess* lhs, ast_expr* rhs) {
        ast_assign_mapentry* newAssign = new ast_assign_mapentry(lhs, rhs);
        return newAssign;
    }

    static ast_assign_mapentry* new_mapentry_reduce_assign(ast_mapaccess* lhs, ast_expr* rhs, int reduceType) {
        ast_assign_mapentry* newAssign = new ast_assign_mapentry(lhs, rhs, reduceType);
        return newAssign;
    }

};

class ast_vardecl: public ast_sent
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
        id->set_instant_assigned(check_instant_assignment(type, init));
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

    static bool check_instant_assignment(ast_typedecl* type, ast_expr* init) {

        if (init == NULL || type == NULL) return false;
        if (!type->is_collection()) return false;
        if (!init->is_field()) return false;
        return true;
    }
};

class ast_return: public ast_sent
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

class ast_foreach: public ast_sent
{
public:
    virtual ~ast_foreach() {
        delete body;
        delete iterator;
        delete source;
        delete source2;
        delete source_field;
        delete cond;
        //delete symbol info
        delete_symtabs();
    }
private:
    ast_foreach() :
            ast_sent(AST_FOREACH), body(NULL), iterator(NULL), source(NULL), source2(NULL), cond(NULL), seq_exe(false), use_reverse(false), iter_type(0) , source_field(NULL), is_src_field(false) {
        create_symtabs();
    }

public:
    static ast_foreach* new_foreach(ast_id* it, ast_id* src, ast_sent* b, int iter_type, ast_expr* cond = NULL) {
        ast_foreach* d = new ast_foreach();
        d->iterator = it;
        d->source = src;
        d->body = b;
        d->iter_type = iter_type;
        d->cond = cond;
        d->is_src_field = false;
        src->set_parent(d);
        it->set_parent(d);
        b->set_parent(d);
        if (cond != NULL) cond->set_parent(d);
        return d;
    }
    static ast_foreach* new_foreach(ast_id* it, ast_field* src_field, ast_sent* b, int iter_type, ast_expr* cond = NULL) {
        ast_foreach* d = new ast_foreach();
        d->iterator = it;
        d->source_field = src_field;
        d->body = b;
        d->iter_type = iter_type;
        d->cond = cond;
        d->is_src_field = true;
        src_field->set_parent(d);
        it->set_parent(d);
        b->set_parent(d);
        if (cond != NULL) cond->set_parent(d);
        return d;
    }

    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    virtual bool is_source_field() {
        return is_src_field;
    }
    virtual ast_field* get_source_field() {
        assert(is_src_field);
        return source_field;
    }
    virtual ast_id* get_source() {
        assert(!is_src_field);
        return source;
    }
    ast_id* get_iterator() {
        return iterator;
    }

    void set_iterator(ast_id* newIterator) {
        iterator = newIterator;
    }

    ast_sent* get_body() {
        return body;
    }
    ast_expr* get_filter() {
        return cond;
    }
    virtual int get_iter_type() {
        return iter_type;
    } // GM_ITERATORS

    virtual void set_iter_type(int i) {
        iter_type = i;
    } // GM_ITERATORS

    virtual ast_id* get_source2() {
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
        assert(body != NULL);
        body->set_parent(this);
    }

    virtual bool has_scope() {
        return true;
    }

    virtual bool is_under_parallel_execution() {
        return is_parallel();  // realy?
    }

    // For is sequential, Foreach is parallel.
    // Optimization may override parallel execution with sequential.

    // sequential execution
    virtual bool is_sequential() {
        return seq_exe;
    }

    virtual void set_sequential(bool b) {
        seq_exe = b;
    }

    // parallel execution
    virtual bool is_parallel() {
        return !is_sequential();
    }

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
    ast_field* source_field;
    int iter_type; // GM_ITERATORS
    ast_expr* cond;
    bool seq_exe;
    bool use_reverse;
    bool is_src_field;
};

// BFS or DFS
class ast_bfs: public ast_sent
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
        //delete iter2;
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
    //ast_id* get_iterator2() {
    //    return iter2;
    //}
    virtual ast_id* get_source() {
        return src;
    }
    virtual ast_id* get_source2() {
        return get_root();
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

    //void set_iterator2(ast_id* id) {
    //    assert(iter2 == NULL);
    //    iter2 = id;
    //}
    
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
        if (b != NULL) b->set_parent(this);
        f_body = b;
    }
    void set_bbody(ast_sentblock* b) {
        if (b != NULL) b->set_parent(this);
        b_body = b;
    }
    virtual void reproduce(int id_level);
    virtual void dump_tree(int id_level);
    virtual void traverse_sent(gm_apply*a, bool is_post, bool is_pre);

    virtual int get_iter_type() {
        return GMITER_NODE_BFS;
    }

    //int get_iter_type2() {
    //    return is_transpose() ? GMTYPE_NODEITER_IN_NBRS : GMTYPE_NODEITER_NBRS;
    //}

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
            ast_sent(AST_BFS), f_body(NULL), b_body(NULL), f_filter(NULL), b_filter(NULL), navigator(NULL), iter(NULL), src(NULL), root(NULL), use_transpose(false), _bfs(true) {
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
    //ast_id* iter2; // iterator used for frontier expansion [xxx] what?
    bool use_transpose;
    bool _bfs;
};

class ast_call: public ast_sent
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
        assert(b_in != NULL);
        b_in->set_parent(this);
    }
    bool is_builtin_call() {
        return is_blt_in;
    }

private:
    ast_expr_builtin* b_in;
    bool is_blt_in;
};

class ast_if: public ast_sent
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
        if (s!=NULL) s->set_parent(this);
        then_part = s;
    }
    void set_else(ast_sent* s) {
        if (s!=NULL) s->set_parent(this);
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

class ast_foreign: public ast_sent
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

class ast_while: public ast_sent
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
class ast_nop: public ast_sent
{
protected:
    ast_nop() :
            ast_sent(AST_NOP), subtype(0) {
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
