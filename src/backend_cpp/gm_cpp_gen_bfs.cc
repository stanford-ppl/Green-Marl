
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

void gm_cpp_gen::generate_bfs_body_fw(ast_bfs* bfs)
{
    assert(bfs->get_f_filter() == NULL); // should be changed into if already 

    if (bfs->is_bfs()) {
        Body.push("virtual void visit_fw(");
    } else {
        Body.push("virtual void visit_pre(");
    }
    Body.push(get_lib()->get_type_string(GMTYPE_NODE));
    Body.SPC();
    Body.push(bfs->get_iterator()->get_genname());
    Body.push(") ");
    if ((bfs->get_fbody() == NULL) || (bfs->get_fbody()->get_sents().size() == 0)) {
        Body.pushln("{}");
    }
    else {
        Body.NL();
        ast_sentblock *sb = bfs->get_fbody();
        generate_sent_block(sb);
        Body.NL();
    }
}

void gm_cpp_gen::generate_bfs_body_bw(ast_bfs* bfs)
{
    assert(bfs->get_b_filter() == NULL); // should be changed into if already

    if (bfs->is_bfs()) {
        Body.push("virtual void visit_rv(");
    } else {
        Body.push("virtual void visit_post(");
    }

    Body.push(get_lib()->get_type_string(GMTYPE_NODE));
    Body.SPC();
    Body.push(bfs->get_iterator()->get_genname());
    Body.push(") ");
    if (bfs->get_bbody() == NULL) {
        Body.pushln("{}");
    }
    else {
        Body.NL();
        ast_sentblock *sb = bfs->get_bbody();
        generate_sent_block(sb);
        Body.NL();
    }
}

void gm_cpp_gen::generate_bfs_navigator(ast_bfs* bfs)
{
    Body.push("virtual bool check_navigator(");
    Body.push(get_lib()->get_type_string(GMTYPE_NODE));
    Body.SPC();
    Body.push(bfs->get_iterator()->get_genname());
    Body.push(", ");
    Body.push(get_lib()->get_type_string(GMTYPE_EDGE));
    Body.SPC();
    const char* alias_name = bfs->get_iterator()->getSymInfo()->find_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR);
    assert(alias_name != NULL);
    assert(strlen(alias_name) > 0);
    Body.push(alias_name);
    Body.push(") ");
    if (bfs->get_navigator() == NULL) {
        Body.pushln("{return true;}");

    }
    else {
        Body.NL();
        Body.pushln("{");
        Body.push("return (");
        ast_expr *nv = bfs->get_navigator();
        generate_expr(nv);
        Body.pushln(");");
        Body.pushln("}");
    }
}

static const char* bool_string(bool b) {
    if (b) return "true";
    else return "false";
}

void gm_cpp_gen::generate_bfs_def(ast_bfs* bfs) 
{
    const char* bfs_name = bfs->find_info_string(CPPBE_INFO_BFS_NAME);
    const char* level_t = "short";
    const char* use_multithread = bool_string(is_target_omp());
    const char* save_child = bool_string(bfs->find_info_bool(CPPBE_INFO_USE_DOWN_NBR)); 
    const char* use_reverse_edge = bool_string(bfs->is_transpose());
    const char* has_navigator = bool_string(bfs->get_navigator()!=NULL);

    const char* has_pre_visit = bool_string((bfs->get_fbody()!=NULL) && 
                                 (bfs->get_fbody()->get_sents().size() >=1));

    const char* has_post_visit = bool_string((bfs->get_bbody()!=NULL) && 
                                  (bfs->get_bbody()->get_sents().size() >=1));


    ast_extra_info_set* info = (ast_extra_info_set*)  bfs->find_info(CPPBE_INFO_BFS_SYMBOLS);
    std::set<void*>& SET = info->get_set();
    std::set<void*>::iterator S;
    gm_symtab_entry* graph_sym = (gm_symtab_entry*) (*(SET.begin()));
    const char* template_name = (bfs->is_bfs() ? BFS_TEMPLATE : DFS_TEMPLATE);

    sprintf(temp, "class %s : public %s", bfs_name, template_name);
    Body.pushln(temp);
    Body.push_indent();
    if (bfs->is_bfs()) {
        sprintf(temp, "<%s, %s, %s, %s, %s>",
            level_t, use_multithread, has_navigator, use_reverse_edge, save_child ); 
    } else {
        sprintf(temp, "<%s, %s, %s, %s>",
            has_pre_visit, has_post_visit, has_navigator, use_reverse_edge);
    }

    Body.pushln(temp);
    Body.pop_indent();
    Body.pushln("{");

    Body.pop_indent();
    Body.pushln("public:");
    Body.push_indent();

    sprintf(temp, "%s(", bfs_name);
    Body.push(temp);

    //------------------------------------------
    // constructor
    //------------------------------------------
    bool is_first = true;
    int total = SET.size();
    int i =0;
    int NL = 3;
    for(S=SET.begin(); S!=SET.end(); S++, i++) {
        if (!is_first) 
            Body.push(", ");
        if ((i>0) && (i != total) && ((i%NL)==0)) Body.NL();
        is_first = false;
        gm_symtab_entry* sym = (gm_symtab_entry*)(*S);
        ast_typedecl* t = sym->getType();
        Body.push(get_type_string(t));
        Body.push("&");
        Body.push(" _");
        Body.push(sym->getId()->get_genname());
    }
    Body.pushln(")");
    sprintf(temp, ": %s", template_name);
    Body.push(temp);
    if (bfs->is_bfs()) {
        sprintf(temp, "<%s, %s, %s, %s, %s>",
            level_t, use_multithread, has_navigator, use_reverse_edge, save_child ); 
    } else {
        sprintf(temp, "<%s, %s, %s, %s>",
            has_pre_visit, has_post_visit, has_navigator, use_reverse_edge);
    }
    Body.push(temp);
    sprintf(temp, "(_%s),",
            graph_sym->getId()->get_genname()); 
    Body.pushln(temp);

    // init list
    is_first = true;
    NL = 6;
    i = 0;
    for(S=SET.begin(); S!=SET.end(); S++, i++) {
        if (!is_first) Body.push(", ");
        if ((i>0) && (i != total) && ((i%NL)==0)) Body.NL();
        is_first = false;
        gm_symtab_entry* sym = (gm_symtab_entry*)(*S);
        Body.push(sym->getId()->get_genname());
        Body.push('(');
        Body.push('_');
        Body.push(sym->getId()->get_genname());
        Body.push(')');
    }
    Body.pushln("{}");
    Body.NL();

    //-------------------------------------------------
    // list of scope variables
    //-------------------------------------------------
    Body.pop_indent();
    Body.pushln("private:  // list of varaibles");
    Body.push_indent();
    for(S=SET.begin(); S!=SET.end(); S++, i++) {
        gm_symtab_entry* sym = (gm_symtab_entry*)(*S);
        ast_typedecl* t = sym->getType();
        Body.push(get_type_string(t));
        Body.push("& ");
        Body.push(sym->getId()->get_genname());
        Body.pushln(";");
    }
    Body.NL();

    Body.pop_indent();
    Body.pushln("protected:");
    Body.push_indent();

    ast_id* iter = bfs->get_iterator();
    const char* a_name = FE.voca_temp_name_and_add(iter->get_orgname(),"_idx");
    iter->getSymInfo()->add_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR, a_name);
    delete [] a_name;

    generate_bfs_body_fw(bfs);
    generate_bfs_body_bw(bfs);
    generate_bfs_navigator(bfs);
    Body.NL();

    Body.NL();
    Body.pushln("};");
    Body.NL();
}



void gm_cpp_gen::generate_sent_bfs(ast_bfs* bfs)
{
    Body.NL();

    //-------------------------------------------
    // (1) create BFS object
    //-------------------------------------------
    const char* bfs_name = bfs->find_info_string(CPPBE_INFO_BFS_NAME);
    const char* bfs_inst_name = bfs->is_bfs() ? 
       FE.voca_temp_name_and_add("_BFS", "") :
       FE.voca_temp_name_and_add("_DFS", ""); 
    sprintf(temp, "%s %s", bfs_name, bfs_inst_name); 
    Body.push(temp);
    Body.push('(');

    //-------------------------------------------
    // give every entry that are used
    //-------------------------------------------
    ast_extra_info_set* syms = (ast_extra_info_set*) bfs->find_info(CPPBE_INFO_BFS_SYMBOLS);
    assert(syms != NULL);
    std::set<void*>& S  = syms->get_set();
    std::set<void*>::iterator I;
    bool is_first = true;
    for (I = S.begin(); I!=S.end(); I++)
    {
        if (!is_first) {Body.push(", ");}
        is_first = false;
        gm_symtab_entry* e = (gm_symtab_entry*) *I; 
        Body.push( e->getId()->get_genname());
    }
    Body.pushln(");");

    //-------------------------------------------
    // (2) Make a call to it
    //-------------------------------------------
    if (bfs->is_bfs())
    {
        sprintf(temp, "%s.%s(%s, %s());", 
            bfs_inst_name, PREPARE, bfs->get_root()->get_genname(),
            MAX_THREADS);
        Body.pushln(temp);
        sprintf(temp, "%s.%s();", bfs_inst_name, DO_BFS_FORWARD); 
        Body.pushln(temp);

        if (bfs->get_bbody() != NULL)
        {
            sprintf(temp, "%s.%s();", bfs_inst_name, DO_BFS_REVERSE); 
            Body.pushln(temp);
        }
    }
    else { // DFS
        sprintf(temp, "%s.%s(%s);", 
            bfs_inst_name, PREPARE, bfs->get_root()->get_genname());
        Body.pushln(temp);
        sprintf(temp, "%s.%s();", bfs_inst_name, DO_DFS); 
        Body.pushln(temp);
    }

    delete [] bfs_inst_name;
    return; 
}
