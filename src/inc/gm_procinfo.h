#ifndef GM_PROCINFO_H
#define GM_PROCINFO_H

#include "gm_ast.h"
#include "gm_misc.h"

// additional information about procedure

class gm_procinfo {
    
public:
    gm_procinfo(ast_procdef* d) {
        body = d;
        temp_no = 0;
    }

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

        //flat.add_word(temp);

        return gm_strdup(temp);
    }




private:
    ast_procdef* body;
    int temp_no;

    gm_vocabulary flat; // flat

};



#endif
