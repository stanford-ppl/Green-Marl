#ifndef GM_TYPECHECK
#define GM_TYPECHECK

#include "gm_ast.h"
#include <vector>

#define GM_READ_AVAILABLE       true
#define GM_READ_NOT_AVAILABLE   false
#define GM_WRITE_AVAILABLE       true
#define GM_WRITE_NOT_AVAILABLE   false

// symbol table entry
class gm_symtab_entry {
    friend class gm_symtab;

    private:
        gm_symtab_entry() : id(NULL), type(NULL) {}

    public: 
        // always call with a copy of ID
        gm_symtab_entry(ast_id* _id, ast_typedecl* _type, bool _isRA=true, bool _isWA=true) : 
            id(_id), type(_type), isRA(_isRA), isWA(_isWA), isArg(false) {
                id->setSymInfo(this, true);
                assert(type != NULL);
                assert(id->name != NULL);
            } 
        virtual ~gm_symtab_entry() {
            delete id;
            delete type;
            std::map<std::string, ast_extra_info*>::iterator i; 
            for(i=extra.begin();i!=extra.end();i++) {
                delete i->second;
            }
        }

        ast_typedecl* getType() {return type;}
        ast_id* getId() {return id;}

        bool isReadable() {return (isRA==GM_READ_AVAILABLE);}
        bool isWriteable() {return (isWA==GM_WRITE_AVAILABLE);}
        bool isArgument()  {return isArg;}

        // defined in gm_node_info.cc
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
        void add_info_ptr(const char* id, void* ptr1, void*ptr2=NULL);
        void add_info_float(const char* id, float f);
        void add_info_string(const char* id, const char* str);
        void remove_info(const char* id);
        void remove_all_info();

        void setArgument(bool b) {isArg = b;}


    private:
        ast_id* id;
        ast_typedecl* type;
        bool isRA;
        bool isWA;
        bool isArg;
        std::map<std::string , ast_extra_info*> extra;
};

static enum {
    GM_SYMTAB_ARG,        // argument
    GM_SYMTAB_VAR,        // variable
    GM_SYMTAB_FIELD,      // node/edge property
    GM_SYMTAB_PROC,       // procedures
} SYMTAB_TYPES;



// symbol table
class gm_symtab {
    public:
        gm_symtab(int _symtab_type, ast_node* _ast) : 
            parent(NULL), 
            symtab_type(_symtab_type), 
            ast(_ast) {}
        int get_symtab_type() {return symtab_type;}

        virtual ~gm_symtab() {
            /*
            for(int i=0;i<entries.size(); i++) {
                gm_symtab_entry* e = entries[i];
                delete e;
            }
            */
            std::set<gm_symtab_entry*>::iterator I;
            for(I=entries.begin(); I!=entries.end(); I++) {
                gm_symtab_entry* e = *I;
                delete e;
            }
            //printf("delete:%p\n", this);
        }

        void set_parent(gm_symtab* p) {parent = p;}
        gm_symtab* get_parent() {return parent;}
        ast_node* get_ast() {return ast;}

        // if old entry is not found, copy of id and copy of typedecl is added into the table
        bool check_duplicate_and_add_symbol(ast_id* id, 
                ast_typedecl* type, 
                gm_symtab_entry*& old_def,
                bool isRA=true, bool isWA=true)
        { 
            assert(id->getSymInfo() == NULL);
            //printf("check duplicate for %s\n", id->get_orgname());
            old_def = find_symbol(id); 
            if (old_def != NULL) return false;
            add_entry(id, type, isRA, isWA); // copy is created inside 
            return true;
        }

        bool check_and_add_symbol(ast_id* id, 
                ast_typedecl* type, 
                gm_symtab_entry*& old_def,
                bool isRA=true, bool isWA=true)
        {
            return check_duplicate_and_add_symbol(id, type, old_def, isRA, isWA);
        }


        gm_symtab_entry* find_symbol(ast_id* id) {
            //for(int i=0;i<entries.size(); i++) {
                //printf("this:%p\n", this);
            std::set<gm_symtab_entry*>::iterator I;
            for(I=entries.begin(); I!=entries.end(); I++) {
                gm_symtab_entry* e = *I;
                //gm_symtab_entry* e = entries[i];
                const char* c = e->id->get_orgname();
                const char* c2 = id->get_orgname();
                if (!strcmp(c,c2)) return e;
            }
            if (parent == NULL) return NULL;
            return parent->find_symbol(id);
        }

        int get_num_symbols() {return entries.size();}

        //std::vector<gm_symtab_entry*>& get_entries() {return entries;}
        std::set<gm_symtab_entry*>& get_entries() {return entries;}
        // return true if entry is in the table
        bool is_entry_in_the_tab(gm_symtab_entry *e) {
            /*
            std::vector<gm_symtab_entry*>::iterator i;
            for(i=entries.begin(); i!=entries.end();i++)
                if (*i == e) return true;
            return false;
            */
            return entries.find(e) != entries.end();
        }
        void remove_entry_in_the_tab(gm_symtab_entry *e) {
            /*
            std::vector<gm_symtab_entry*>::iterator i;
            for(i=entries.begin(); i!=entries.end();i++)
                if (*i == e) break;
            if (i!=entries.end())
                entries.erase(i);
            */
            entries.erase(e);
        }

        // merge table A into this. table A is emptied.
        // (assertion: name conflict has been resolved before calling this function)
        void merge(gm_symtab* A) {
            assert(A!=NULL);
            std::set<gm_symtab_entry*>::iterator i;
            for(i=A->entries.begin(); i!= A->entries.end();i++) {
                //entries.push_back(*i);
                entries.insert(*i);
            }
            A->entries.clear();
        }

        // add symbol entry
        // (assertion: name conflict has been resolved)
        void add_symbol(gm_symtab_entry* e) {
            entries.insert(e);
        }

    private:
        // copy of (id) and copy of (type) is added into a new symbol entry
        void add_entry(ast_id* id, ast_typedecl* type, bool isRA=true, bool isWA=true) { 
            ast_id* id_copy = id->copy();
            ast_typedecl* type_copy = type->copy();
            gm_symtab_entry* e  = new gm_symtab_entry(id_copy, type_copy, isRA, isWA);

            id->setSymInfo(e);
            //entries.push_back(e);
            entries.insert(e);
        }


    private:
        std::set<gm_symtab_entry*> entries;
        gm_symtab* parent;
        int symtab_type;
        ast_node* ast; // where this belongs to 

};

//------------------------------------------
// static scope
//------------------------------------------
class gm_scope
{
    public:
        gm_scope() :  for_group_expr(false), G(NULL), node_prop(false), RT(NULL), tg(NULL) {}
        virtual ~gm_scope() {}

    public:
        void push_symtabs(gm_symtab* v, gm_symtab* f, gm_symtab* p) { 
            var_syms.push_back(v);
            field_syms.push_back(f);
            proc_syms.push_back(p);
        }
        void pop_symtabs() {
            var_syms.pop_back();
            field_syms.pop_back();
            proc_syms.pop_back();
        }
        gm_symtab* get_varsyms() {return var_syms.back();}
        gm_symtab* get_fieldsyms() {return field_syms.back();}
        gm_symtab* get_procsyms() {return proc_syms.back();}

        std::list<gm_symtab*> var_syms;
        std::list<gm_symtab*> field_syms;
        std::list<gm_symtab*> proc_syms;


    //----------------------------------------------------------------
    // temporary information, during typechecking
    //----------------------------------------------------------------
    private:
        // for group assignment
        bool for_group_expr;
        gm_symtab_entry *G;     // target symbol for group-assignment
        bool node_prop;         // true if n_p, false if e_p.

        // for retuern type-check
        ast_typedecl* RT; // return type

        // for target graph matching check
        gm_symtab_entry* tg;


    public: 
        bool is_for_group_expr() {return for_group_expr;}
        bool is_for_node_prop() {return node_prop;}
        gm_symtab_entry* get_target_sym() {return G;}
        void set_group_expr(bool for_group, gm_symtab_entry *g= NULL, bool np=false) {
            for_group_expr = for_group;G = g; node_prop = np;}

        void set_return_type(ast_typedecl* R) {RT = R;}
        ast_typedecl* get_return_type() {return RT;}

        void set_target_graph(gm_symtab_entry* e) {tg = e;}
        gm_symtab_entry* get_target_graph() {return tg;}
};
#endif
