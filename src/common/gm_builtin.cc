#include "gm_frontend.h"
#include "gm_misc.h"
#include "gm_builtin.h"
 

static int gm_get_type_from_string(const char* s) {
    assert(s!=NULL);
    if (gm_is_same_string(s, "Graph")) return GMTYPE_GRAPH;
    else if (gm_is_same_string(s, "Node")) return GMTYPE_NODE;
    else if (gm_is_same_string(s, "Edge")) return GMTYPE_EDGE;
    else if (gm_is_same_string(s, "NI_All")) return GMTYPE_NODEITER_ALL;
    else if (gm_is_same_string(s, "EI_All")) return GMTYPE_EDGEITER_ALL;
    else if (gm_is_same_string(s, "NI_Out")) return GMTYPE_NODEITER_NBRS;
    else if (gm_is_same_string(s, "NI_In"))   return GMTYPE_NODEITER_IN_NBRS;
    else if (gm_is_same_string(s, "NI_Up"))   return GMTYPE_NODEITER_UP_NBRS;
    else if (gm_is_same_string(s, "NI_Down")) return GMTYPE_NODEITER_DOWN_NBRS;
    else if (gm_is_same_string(s, "Int")) return GMTYPE_INT;
    else if (gm_is_same_string(s, "Long")) return GMTYPE_LONG;
    else if (gm_is_same_string(s, "Float")) return GMTYPE_FLOAT;
    else if (gm_is_same_string(s, "Double")) return GMTYPE_DOUBLE;
    else if (gm_is_same_string(s, "N_S")) return GMTYPE_NSET;
    else if (gm_is_same_string(s, "E_S")) return GMTYPE_ESET;
    else if (gm_is_same_string(s, "N_O")) return GMTYPE_NORDER;
    else if (gm_is_same_string(s, "E_O")) return GMTYPE_EORDER;
    else if (gm_is_same_string(s, "N_Q")) return GMTYPE_NSEQ;
    else if (gm_is_same_string(s, "E_Q")) return GMTYPE_ESEQ;
    else if (gm_is_same_string(s, "Void"))  return GMTYPE_VOID;
    else if (gm_is_same_string(s, "Bool"))  return GMTYPE_BOOL;
    else assert(false);
}

gm_builtin_def::gm_builtin_def(const gm_builtin_desc_t* def)  {
    this->method_id = def->method_id;

    // parse string
    char* temp = gm_strdup(def->def_string);
    char* temp_org = temp;

    if (temp[0] == '*') { // synonym

        gm_builtin_def* org_def = BUILT_IN.get_last_def();
        assert(org_def!=NULL);

        this->synonym = true;
        this->need_strict = false;	  
        this->org_def = org_def;
        this->src_type = org_def->src_type;  // need source type.
        this->orgname = gm_strdup(&temp[1]);

        // no need.
        //this->res_type = org_def->res_type;

    } else {
        this->synonym = false;

        if (temp[0] == '!') {
            this->need_strict = true;
            temp = temp+1;
        }
        else {
            this->need_strict = false;	  
        }

        // parse and fill
        char *p;
        p= strtok(temp, ":");
        if (p[0] == '_')
            src_type = GMTYPE_VOID; // top-level
        else
            this->src_type = gm_get_type_from_string(p);

        p = strtok(NULL, ":");
        this->orgname = gm_strdup(p);
        p = strtok(NULL, ":");
        this->res_type = gm_get_type_from_string(p);
        p = strtok(NULL, ":");
        if (p==NULL) this->num_args = 0;
        else this->num_args = atoi(p);
        if (num_args > 0) {
            this->arg_types = new int[num_args];
            for(int i=0;i<num_args;i++) {
                p = strtok(NULL, ":");
                this->arg_types[i] = gm_get_type_from_string(p);
            }
        }

        //-----------------------------------------------------------
        // now parse the extra info [todo]
        //-----------------------------------------------------------
        char* extra_info = strdup(def->extra_info);

        p = strtok(extra_info, ":");
        char* p2 = strtok(NULL, ":");
        while ((p!=NULL) && (p2!=NULL))
        {
            char* key = p;
            if (gm_is_same_string(p2,"true"))
            {
                add_info_bool(key, true);
                assert(find_info_bool(key) == true);
            } else if (gm_is_same_string(p2,"false")){
                add_info_bool(key, false);
            } else {
                add_info_int(key, atoi(p2));
            }
            p = strtok(NULL, ":");
            p2 = strtok(NULL, ":");
        }
    }

    delete [] temp_org;
}

void gm_builtin_def::add_info_int(const char* key, int v)
{
    std::string s(key);
    if (extra_info.find(key) == extra_info.end())
    {
        ast_extra_info I; I.ival = v;
        extra_info[s] = I;
    }
    else {
        extra_info[s].ival = v;
    }
}

void gm_builtin_def::add_info_bool(const char* key, bool v)
{
    std::string s(key);
    if (extra_info.find(key) == extra_info.end())
    {
        ast_extra_info I; I.bval = v;
        extra_info[s] = I;
    }
    else {
        extra_info[s].bval = v;
    }
}

bool gm_builtin_def::has_info(const char* key)
{
    std::string s(key);
    if (extra_info.find(s) == extra_info.end())
        return false;
    return true;
}
int gm_builtin_def::find_info_int(const char* key)
{
    std::string s(key);
    if (extra_info.find(s) == extra_info.end())
        return 0;
    else return extra_info[s].ival;
}
bool gm_builtin_def::find_info_bool(const char* key)
{
    std::string s(key);
    if (extra_info.find(s) == extra_info.end())
        return false;
    else {
        return extra_info[s].bval;
    }

}


gm_builtin_manager::gm_builtin_manager() 
{
    //-----------------------------------------------------
    // construct built-in library by 
    // parsing built-in strings in (gm_builtin.h)
    //-----------------------------------------------------
    int cnt = sizeof(GM_builtins) / sizeof(gm_builtin_desc_t);
    for(int i = 0; i < cnt; i++) {
        gm_builtin_def* d = new gm_builtin_def(&GM_builtins[i]);
        defs.push_back(d); 
        if (!d->is_synonym_def()) last_def = d;
    }
}

gm_builtin_manager::~gm_builtin_manager() {
    std::list<gm_builtin_def*>::iterator i;
    for(i=defs.begin(); i!=defs.end(); i++)
        delete *i;
}


gm_builtin_def* gm_builtin_manager::find_builtin_def(int source_type, const char* orgname)
{
    std::list<gm_builtin_def*>::iterator i;
    for(i=defs.begin(); i!=defs.end(); i++)
    {
        gm_builtin_def* d  = *i;
        int def_src = d->get_source_type_summary();
        if (gm_is_same_string(orgname, d->get_orgname()))
        {
            if (def_src == source_type) {
                if (d->is_synonym_def()) return d->get_org_def();
                else return d;
            }
            bool is_strict = d->need_strict_source_type();
            if (is_strict) continue;
            if (def_src == GMTYPE_VOID) continue;
            assert(!gm_is_prim_type(def_src));

            if (gm_is_same_node_or_edge_compatible_type(def_src, source_type))
            {
                if (d->is_synonym_def()) return d->get_org_def();
                else return d;
            }
        }
    }
    return NULL;
}

gm_builtin_def* gm_builtin_manager::find_builtin_def(int source_type, int id)
{
    bool is_strict;

    std::list<gm_builtin_def*>::iterator i;
    for(i=defs.begin(); i!=defs.end(); i++)
    {
        gm_builtin_def* d  = *i;
        if (d->get_method_id() != id) continue;

        int def_src = d->get_source_type_summary();
        if (def_src == source_type) goto found;

        is_strict = d->need_strict_source_type();

        if (is_strict) continue;
        if (source_type == GMTYPE_VOID) continue;
        if (gm_is_prim_type(def_src)) continue;

        if (!gm_is_same_node_or_edge_compatible_type(def_src, source_type))
            continue;
found:
        if (d->is_synonym_def()) return d->get_org_def();
        else return d;
    }
    return NULL;
}


ast_expr_builtin* ast_expr_builtin::new_builtin_expr(ast_id* id, gm_builtin_def* d, expr_list* t) 
{
    ast_expr_builtin* E = new ast_expr_builtin(); 
    E->expr_class = GMEXPR_BUILTIN;
    E->driver = id; if (id != NULL) id->set_parent(E); // type unknown yet.
    E->def = d;
    E->orgname = gm_strdup(d->get_orgname());
    if (t!= NULL) {
        E->args = t->LIST;  // shallow copy LIST
        // but not set 'up' pointer. 
        std::list<ast_expr*>::iterator I;
        for( I=E->args.begin(); I!=E->args.end(); I++) 
           (*I)->set_parent(E);
        delete t; // t is only temporary, delete it.
    }
    return E; 
}
