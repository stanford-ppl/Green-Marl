#ifndef GM_PROCINFO_H
#define GM_PROCINFO_H

#include "gm_ast.h"
#include "gm_misc.h"

// additional information about procedure

class gm_backend_info { 
};  // empty class

class gm_procinfo {
    
public:
    gm_procinfo(ast_procdef* d) {
        body = d;
        temp_no = 0;
        be_info = NULL;
    }
    virtual ~gm_procinfo() {}

    //--------------------------------------------
    // vocaburary and temp name generator
    //--------------------------------------------
    gm_vocabulary& get_voca() {return flat;} 
    void add_voca(char* n) {flat.add_word(n);}
    void clear_voca()      {flat.clear();}
    bool isin_voca(char* n) {return flat.has_word(n);}

    //----------------------------------------------------------
    // create a temporary name, that does not conflict with any
    // word in the vocaburary
    // return a new string (caller should delete it later)
    //----------------------------------------------------------
    char* generate_temp_name(const char* base, 
            gm_vocabulary* extra  = NULL,
            bool try_org_name_first = false
            ) 
    {
        char temp[2048]; // should be enough
        if (try_org_name_first)
            sprintf(temp, "%s", base); // try base name first
        else
            sprintf(temp, "%s%d", base, temp_no++); // try base name first
        do {
            if (flat.has_word(temp)) {
                sprintf(temp, "%s%d", base, temp_no++);
            }
            else if ((extra != NULL) && (extra->has_word(temp))) {
                sprintf(temp, "%s%d", base, temp_no++);
            }
            else {
                break;
            }

        } while (true);
        return gm_strdup(temp);
    }

    //-------------------------------------------------------------------
    // adding extra information to procdef
    //-------------------------------------------------------------------
    bool has_info(const char* id) {return body->has_info(id);}
    ast_extra_info* find_info(const char*id) {return body->find_info(id);}
    bool find_info_bool(const char* id) {return body->find_info_bool(id);}
    const char* find_info_string(const char* id) {return body->find_info_string(id);}
    float find_info_float(const char* id) {return body->find_info_float(id);}
    int find_info_int(const char* id) {return body->find_info_int(id);}
    void* find_info_ptr(const char* id) {return body->find_info_ptr(id);}
    void* find_info_ptr2(const char* id) {return body->find_info_ptr2(id);}
    void add_info(const char* id, ast_extra_info* e) {body->add_info(id, e);}
    void add_info_int(const char* id, int i) {body->add_info_int(id, i);}
    void add_info_bool(const char* id, bool b) {body->add_info_bool(id, b);}
    void add_info_ptr(const char* id, void* ptr1, void*ptr2=NULL) {body->add_info_ptr(id, ptr1, ptr2);}
    void add_info_float(const char* id, float f) {body->add_info_float(id, f);}
    void add_info_string(const char* id, const char* str) {body->add_info_string(id, str);}
    void remove_info(const char* id) {body->remove_info(id);}
    void remove_all_info() {body->remove_all_info();}

    ast_procdef* get_body() {return body;}

    //--------------------------------------------------------------
    // Any backend strucut
    //--------------------------------------------------------------
    gm_backend_info* get_be_info() {return be_info;}
    void set_be_info(gm_backend_info* i) {be_info = i;}

private:
    gm_procinfo() {} // cannot create without body
    ast_procdef* body;
    int temp_no;
    gm_backend_info* be_info;

    gm_vocabulary flat; // flat

};



#endif
