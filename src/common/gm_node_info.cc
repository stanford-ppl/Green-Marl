#include "gm_ast.h"
#include "gm_typecheck.h"

//-----------------------------------------
// extra information status
//-----------------------------------------
ast_extra_info::ast_extra_info(): 
    ival(0), bval(false), fval(0), 
    ptr1(NULL), ptr2(NULL) {}
   
ast_extra_info::ast_extra_info(bool b): 
    ival(0), bval(b), fval(0), 
    ptr1(NULL), ptr2(NULL) {}

ast_extra_info::ast_extra_info(int i): 
    ival(i), bval(false), fval(0), 
    ptr1(NULL), ptr2(NULL) {}

ast_extra_info::ast_extra_info(float f): 
    ival(0), bval(false), fval(f), 
    ptr1(NULL), ptr2(NULL) {}

ast_extra_info::ast_extra_info(void*p1, void*p2):
    ival(0), bval(false), fval(0), 
    ptr1(p1), ptr2(p2) {}

ast_extra_info* ast_extra_info::copy() 
{
    ast_extra_info* i = new ast_extra_info(); 
    i->base_copy(this); 
    return i;
}
void ast_extra_info::base_copy(ast_extra_info* from) 
{ 
    *this = *from; 
}

ast_extra_info_string::ast_extra_info_string() : str(NULL) 
{}
ast_extra_info_string::ast_extra_info_string(const char* org) 
{ 
    str = gm_strdup(org);
}
ast_extra_info_string::~ast_extra_info_string()
{
    delete [] str;
}
const char* ast_extra_info_string::get_string()
{
    return (const char*) str;
}
ast_extra_info* ast_extra_info_string::copy() 
{
    ast_extra_info_string * s = new ast_extra_info_string(str);
    s->base_copy(this);
    return s;
}



//=========================================
// ast_node
//=========================================
ast_extra_info* ast_node::find_info(const char* id) 
{
    std::string s(id);
    std::map<std::string, ast_extra_info*>::iterator i = extra.find(s);
    if (i == extra.end()) return NULL;
    else return i->second;
}
bool ast_node::has_info(const char* id)
{
    std::string s(id);
    return (extra.find(s) != extra.end());
}

bool ast_node::find_info_bool(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->bval;
}
int ast_node::find_info_int(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->ival;
}
float ast_node::find_info_float(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->fval;
}
void* ast_node::find_info_ptr(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->ptr1;
}
void* ast_node::find_info_ptr2(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->ptr2;
}
const char* ast_node::find_info_string(const char* id)
{
    ast_extra_info_string* info = 
        (ast_extra_info_string*) find_info(id);
    if (info == NULL) 
        return "";  // or NULL string?
    else 
        return info->get_string();
}

void ast_node::add_info(const char* id, ast_extra_info* e)
{
    // should I delete repeated entry?
    std::string s(id);
    extra[s] = e;
}

void ast_node::add_info_int(const char* id, int i)
{
    ast_extra_info* e = find_info(id);
    if (e == NULL) 
        add_info(id, new ast_extra_info(i));
    else 
        e->ival = i;
}

void ast_node::add_info_bool(const char* id, bool b)
{
    ast_extra_info* e = find_info(id);
    if (e == NULL) 
        add_info(id, new ast_extra_info(b));
    else 
        e->bval = b;
}
void ast_node::add_info_float(const char* id, float f)
{
    ast_extra_info* e = find_info(id);
    if (e == NULL) 
        add_info(id, new ast_extra_info(f));
    else 
        e->fval = f;
}
// str is copied
void ast_node::add_info_string(const char* id, const char* str)
{
    add_info(id, new ast_extra_info_string(str));
}

void ast_node::add_info_ptr(const char* id, void* ptr1, void* ptr2)
{
    add_info(id, new ast_extra_info(ptr1, ptr2));
}

void ast_node::remove_info(const char* id) 
{
    std::string s(id);
    extra.erase(s); // [XXX] need delete extra_info object
}
void ast_node::remove_all_info() 
{
    extra.clear();
}

bool ast_node::has_info_set(const char* id) {
    return (find_info(id) != NULL);
}
void ast_node::add_info_set_element(const char* id, void* element)
{
    if (find_info(id) == NULL) {
        ast_extra_info_set* INFO = new ast_extra_info_set();
        add_info(id, INFO);
    }
    std::set<void*> & S = ((ast_extra_info_set*) find_info(id))->get_set();
    S.insert(element);
}
std::set<void*>& ast_node::get_info_set(const char* id)
{
    ast_extra_info_set* INFO = ((ast_extra_info_set*) find_info(id));
    assert(INFO != NULL);
    return INFO->get_set();
}

bool ast_node::has_info_list(const char* id) {
    return (find_info(id) != NULL);
}
void ast_node::add_info_list_element(const char* id, void* element)
{
    if (find_info(id) == NULL) {
        ast_extra_info_list* INFO = new ast_extra_info_list();
        add_info(id, INFO);
    }
    std::list<void*> & S = ((ast_extra_info_list*) find_info(id))->get_list();
    S.push_back(element);
}
std::list<void*>& ast_node::get_info_list(const char* id)
{
    ast_extra_info_list* INFO = ((ast_extra_info_list*) find_info(id));
    assert(INFO != NULL);
    return INFO->get_list();
}

bool ast_node::has_info_map(const char* id) {
    return (find_info(id) != NULL);
}
void ast_node::add_info_map_key_value(const char* id, void* k, void* v)
{
    if (find_info(id) == NULL) {
        ast_extra_info_map* INFO = new ast_extra_info_map();
        add_info(id, INFO);
    }
    std::map<void*,void*> & S = ((ast_extra_info_map*) find_info(id))->get_map();
    S[k] = v;
}
void* ast_node::find_info_map_value(const char* id, void* key)
{
    if (find_info(id) == NULL) {
        ast_extra_info_map* INFO = new ast_extra_info_map();
        add_info(id, INFO);
    }
    std::map<void*,void*> & S = ((ast_extra_info_map*) find_info(id))->get_map();

    // if not in the map? NULL will be retuned. right?
    if (S.find(key) == S.end()) {
        S[key] = NULL;
    }
    return S[key]; 
}
std::map<void*,void*>& ast_node::get_info_map(const char* id)
{
    ast_extra_info_map* INFO = ((ast_extra_info_map*) find_info(id));
    assert(INFO != NULL);
    return INFO->get_map();
}

void ast_node::copy_info_from(ast_node * n)
{
    std::map<std::string , ast_extra_info*>::iterator I;
    for (I=n->extra.begin(); I!= n->extra.end(); I++)
    {
        std::string s = I->first;
        ast_extra_info* e = I->second;
        if (this->extra.find(s) == this->extra.end()) {
            this->extra[s] = e->copy();
        }
    }
}

//---------------------------------------------------------------------
// implementing same inferface to ast_symtab_entry
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// implementing same inferface to ast_symtab_entry
//---------------------------------------------------------------------

ast_extra_info* gm_symtab_entry::find_info(const char* id) 
{
    std::string s(id);
    std::map<std::string, ast_extra_info*>::iterator i = extra.find(s);
    if (i == extra.end()) return NULL;
    else return i->second;
}
bool gm_symtab_entry::has_info(const char* id)
{
    std::string s(id);
    return (extra.find(s) != extra.end());
}

bool gm_symtab_entry::find_info_bool(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return false; 
    else 
        return info->bval;
}
int gm_symtab_entry::find_info_int(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return 0;
    else 
        return info->ival;
}
float gm_symtab_entry::find_info_float(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return 0;
    else 
        return info->fval;
}
void* gm_symtab_entry::find_info_ptr(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return NULL;
    else 
        return info->ptr1;
}
void* gm_symtab_entry::find_info_ptr2(const char* id)
{
    ast_extra_info* info = find_info(id);
    if (info == NULL) 
        return NULL; 
    else 
        return info->ptr2;
}
const char* gm_symtab_entry::find_info_string(const char* id)
{
    ast_extra_info_string* info = 
        (ast_extra_info_string*) find_info(id);
    if (info == NULL) 
        return "";  // or NULL string?
    else 
        return info->get_string();
}

void gm_symtab_entry::add_info(const char* id, ast_extra_info* e)
{
    // should I delete repeated entry?
    assert(e!=NULL);
    std::string s(id);
    extra[s] = e;
}

void gm_symtab_entry::add_info_int(const char* id, int i)
{
    add_info(id, new ast_extra_info(i));
}

void gm_symtab_entry::add_info_bool(const char* id, bool b)
{
    add_info(id, new ast_extra_info(b));
}
void gm_symtab_entry::add_info_float(const char* id, float f)
{
    add_info(id, new ast_extra_info(f));
}
// str is copied
void gm_symtab_entry::add_info_string(const char* id, const char* str)
{
    add_info(id, new ast_extra_info_string(str));
}

void gm_symtab_entry::add_info_ptr(const char* id, void* ptr1, void* ptr2)
{
    add_info(id, new ast_extra_info(ptr1, ptr2));
}

void gm_symtab_entry::remove_info(const char* id) 
{
    std::string s(id);
    extra.erase(s); // [XXX] need delete extra_info object
}

