#include "gm_frontend.h"
#include "gm_misc.h"
#include "gm_builtin.h"


static int gm_get_type_from_string(const char* s) {
    assert(s!=NULL);
    if (gm_is_same_string(s, "Graph")) return GMTYPE_GRAPH;
    else if (gm_is_same_string(s, "Node")) return GMTYPE_NODE;
    else if (gm_is_same_string(s, "Edge")) return GMTYPE_EDGE;
    else if (gm_is_same_string(s, "NodeI")) return GMTYPE_NODEITER_ALL;
    else if (gm_is_same_string(s, "EdgeI")) return GMTYPE_EDGEITER_ALL;
    else if (gm_is_same_string(s, "Int")) return GMTYPE_INT;
    else if (gm_is_same_string(s, "Long")) return GMTYPE_LONG;
    else if (gm_is_same_string(s, "Float")) return GMTYPE_FLOAT;
    else if (gm_is_same_string(s, "Double")) return GMTYPE_DOUBLE;
    else if (gm_is_same_string(s, "NodeS")) return GMTYPE_NSET;
    else if (gm_is_same_string(s, "EdgeS")) return GMTYPE_ESET;
    else if (gm_is_same_string(s, "NodeO")) return GMTYPE_NORDER;
    else if (gm_is_same_string(s, "EdgeO")) return GMTYPE_EORDER;
    else if (gm_is_same_string(s, "NodeQ")) return GMTYPE_NSEQ;
    else if (gm_is_same_string(s, "EdgeQ")) return GMTYPE_ESEQ;
    else if (gm_is_same_string(s, "Void"))  return GMTYPE_VOID;
    else if (gm_is_same_string(s, "Bool"))  return GMTYPE_BOOL;
    else assert(false);
}

gm_builtin_def::gm_builtin_def(const gm_builtin_desc_t* def)  {
    this->method_id = def->method_id;

    // parse string
    char* temp = gm_strdup(def->def_string);

    if (temp[0] == '*') { // synonym

        gm_builtin_def* org_def = BUILT_IN.get_last_def();
        assert(org_def!=NULL);

        this->synonym = true;
        this->org_def = org_def;
        this->src_type = org_def->src_type;  // need source type.
        this->orgname = gm_strdup(&temp[1]);

        // no need.
        //this->res_type = org_def->res_type;

    } else {
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


    }

    delete [] temp;
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


gm_builtin_def* gm_builtin_manager::find_builtin_def(int source_type, const char* orgname, bool is_strict)
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

gm_builtin_def* gm_builtin_manager::find_builtin_def(int source_type, int id, bool is_strict)
{
    std::list<gm_builtin_def*>::iterator i;
    for(i=defs.begin(); i!=defs.end(); i++)
    {
        gm_builtin_def* d  = *i;
        if (d->get_method_id() != id) continue;

        int def_src = d->get_source_type_summary();
        if (def_src == source_type) goto found;

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

