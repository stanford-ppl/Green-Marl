#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

#define HELPERS "gm_helper_functions.h"
#define SET_INC "gm_helper_functions.h"

//--------------------------------------------------------------
// A Back-End for Single Threaded C++ execution
//--------------------------------------------------------------
void gm_cpp_gen::setTargetDir(const char* d)
{
    assert(d!=NULL);
    if (dname != NULL)
        delete [] dname;
    dname = new char[strlen(d)+1];
    strcpy(dname, d);
}

void gm_cpp_gen::setFileName(const char* f)
{
    assert(f!=NULL);
    if (fname != NULL)
        delete [] fname;
    fname = new char[strlen(f)+1];
    strcpy(fname, f);
}


//=======================================================================================
// Text Generation
//=======================================================================================
bool gm_cpp_gen::open_output_files()
{
    char temp[1024];
    assert(dname!=NULL);
    assert(fname!=NULL);

    sprintf(temp, "%s/%s.h", dname, fname);
    f_header = fopen(temp, "w");
    if (f_header == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Header.set_output_file(f_header);

    sprintf(temp, "%s/%s.cc", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    get_lib()->set_code_writer(&Body);

}
void gm_cpp_gen::close_output_files()
{
    if (f_header!=NULL) {Header.flush();fclose(f_header); f_header = NULL;}
    if (f_body!=NULL) {Body.flush();fclose(f_body); f_body = NULL;}
}


void gm_cpp_gen::add_include(
        const char* string, gm_code_writer& Out, 
        bool is_clib, const char* str2)
{
    Out.push("#include ");
    if (is_clib) Out.push('<'); else Out.push('"');
    Out.push(string); Out.push(str2);
    if (is_clib) Out.push('>'); else Out.push('"');
    Out.NL();
}
void gm_cpp_gen::add_ifdef_protection(const char* s)
{
    Header.push("#ifndef GM_GENERATED_CPP_");
    Header.push_to_upper(s);
    Header.pushln("_H");
    Header.push("#define GM_GENERATED_CPP_");
    Header.push_to_upper(s);
    Header.pushln("_H");
    Header.NL();
}

bool gm_cpp_gen::do_generate()
{
    //-----------------------------------
    // source name
    //-----------------------------------
    if (!open_output_files())
        return false;

    const char* NAMES[]= {
        "[Reviving Vardecl]",
        "[Mark Parallel Sentences]",
        "[Generting Codes]"};

    const int COUNT = sizeof(NAMES)/sizeof(const char*);
    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);
        switch(i) {
            case 0: 
                do_restore_vardecl(); break;
            case 1 :
                do_mark_parallel_sents(); break;
            case 2:
                do_generate_main(); break;

            case COUNT:
            default:
                assert(false);
        }

        gm_end_minor_compiler_stage();
    }
    return true;
}

bool gm_cpp_gen::do_restore_vardecl()
{
    std::vector<ast_procdef*>& top_levels = FE.get_procs(); 
    std::vector<ast_procdef*>::iterator it;
    for(it = top_levels.begin(); it!=top_levels.end(); it++)
    {
        ast_procdef* proc = *it;
        FE.restore_vardecl(proc);
    }

    return true;
}

bool gm_cpp_gen::do_mark_parallel_sents()
{
    std::vector<ast_procdef*>& top_levels = FE.get_procs(); 
    std::vector<ast_procdef*>::iterator it;
    for(it = top_levels.begin(); it!=top_levels.end(); it++)
    {
        ast_procdef* proc = *it;
        bool entry_is_seq = true;
        gm_mark_sents_under_parallel_execution(proc->get_body(), entry_is_seq);
    }

    return true;
}

bool gm_cpp_gen::do_generate_main()
{
    //----------------------------------
    // header
    //----------------------------------
    add_ifdef_protection(fname);
    add_include("stdio.h", Header);
    add_include("stdlib.h", Header);
    add_include("stdint.h",Header);
    add_include("float.h",Header);
    add_include("limits.h",Header);
    add_include("cmath", Header);
    add_include("algorithm", Header);
    if (is_target_omp()) 
        add_include("omp.h", Header);
    add_include(HELPERS, Header, false);
    add_include(get_lib()->get_header_info(), Header, false);
    add_include(SET_INC, Header, false);
    Header.NL();

    //----------------------------------------
    // .cc
    //----------------------------------------
    sprintf(temp, "%s.h", fname);
    add_include(temp, Body, false);
    Body.NL();

    std::vector<ast_procdef*>& top_levels = FE.get_procs(); 
    std::vector<ast_procdef*>::iterator it;
    for(it = top_levels.begin(); it!=top_levels.end(); it++)
    {
        generate(*it);
        Body.NL();
    }

    Header.NL();
    Header.pushln("#endif");

    close_output_files();

    return true;
}


bool gm_cpp_gen::generate(ast_procdef* proc)
{
    bool b;
    //-------------------------------
    // declare function name 
    //-------------------------------
    b = generate_proc_decl(proc, false);  // header
    b &=  generate_proc_decl(proc, true);   // body
    if (!b) return false;

    b &= generate(proc->get_body());

    return b;
}

bool gm_cpp_gen::generate_proc_decl(ast_procdef* proc, bool is_def)
{
     // declare in the header or body
     gm_code_writer& Out = is_def ? Body : Header;

     // return type
     Out.push_spc(get_type_string(proc->get_return_type(), GMUSE_RETURN));
     Out.push( proc->get_procname()->get_genname() );
     Out.push('(');

     int max_arg_per_line = 2;
     int arg_curr=0;
     int remain_args = proc->get_in_args().size() + proc->get_out_args().size();
    {
        std::list<ast_argdecl*>& lst = proc->get_in_args();
        std::list<ast_argdecl*>::iterator i;
        for(i=lst.begin();i!=lst.end(); i++) 
        {
            remain_args--;
            arg_curr++;

            Out.push_spc(get_type_string((*i)->get_type(), GMUSE_INARG) );
            Out.push((*i)->get_idlist()->get_item(0)->get_genname());
            if (remain_args > 0) {
                Out.push(", ");
            }

            if ((arg_curr == max_arg_per_line) && (remain_args > 0)) {
                Out.NL();
                arg_curr = 0;
            }
        }
    }
    {
        std::list<ast_argdecl*>& lst = proc->get_out_args();
        std::list<ast_argdecl*>::iterator i;
        for(i=lst.begin();i!=lst.end(); i++) 
        {
            remain_args--;
            arg_curr++;

            Out.push_spc(get_type_string((*i)->get_type(), GMUSE_OUTARG) );
            Out.push((*i)->get_idlist()->get_item(0)->get_genname());
            if (remain_args > 0) {
                Out.push(", ");
            }
            if ((arg_curr == max_arg_per_line) && (remain_args > 0)) {
                Out.NL();
             arg_curr = 0;
            }
        }
    }

     Out.push(')');
     if (!is_def)
        Out.push(';');

     Out.NL();
     return true;
}

bool gm_cpp_gen::generate(ast_idlist* idl)
{
    int z = idl->get_length();
    for(int i=0; i < z;i++) {
        ast_id* id = idl->get_item(i);
        generate(id);
        if (i < z-1) Body.push_spc(',');
    }
    return true;
}
bool gm_cpp_gen::generate(ast_id* id) {
    Body.push(id->get_genname());
    return true;
}

const char* gm_cpp_gen::get_type_string(int prim_type, int usage)
{
    bool is_out = (usage == GMUSE_OUTARG);
        switch(prim_type) {
            case GMTYPE_BYTE:  return !is_out ? "char_t" :"char_t&";
            case GMTYPE_SHORT: return !is_out? "int16_t" : "int16_t&";
            case GMTYPE_INT:   return !is_out? "int32_t" : "int32_t&";
            case GMTYPE_LONG:  return !is_out? "int64_t" : "int64_t&";
            case GMTYPE_FLOAT: return !is_out? "float" : "float&";
            case GMTYPE_DOUBLE: return !is_out? "double" : "double&";
            case GMTYPE_BOOL: return !is_out? "bool" : "bool&";
            default: assert(false); return "??";
        }
}

const char* gm_cpp_gen::get_type_string(ast_typedecl* t, int usage)
{
    bool is_out = (usage == GMUSE_OUTARG);
    if (t==NULL) {
        assert(usage == GMUSE_RETURN);
        return "void";
    }

    if (t->is_primitive()) {
            return get_type_string(t->get_typeid(), usage);
    }
    else if (t->is_property()) {
        ast_typedecl* t2 = t->get_target_type();
        assert(t2!=NULL);
        if (t2->is_primitive()) {
            switch(t2->get_typeid()) {
                case GMTYPE_BYTE:  return "char_t*";
                case GMTYPE_SHORT: return "int16_t*";
                case GMTYPE_INT:   return "int32_t*";
                case GMTYPE_LONG:  return "int64_t*";
                case GMTYPE_FLOAT: return "float*";
                case GMTYPE_DOUBLE: return "double*";
                case GMTYPE_BOOL:   return "bool*";
                default: assert(false);
            }
        }
        else {
           assert(false);
        }
    }
    else 
        return get_lib()->get_type_string(t, usage);

    return "ERROR";
}

bool gm_cpp_gen::generate(ast_sent* s)
{
    // todo: check how this becomes after code transformation
    int lines = s->get_empty_lines_before();
    for(int i=0;i<lines;i++) {
        Body.NL();
    }

    switch(s->get_nodetype())
    {
        case AST_VARDECL:
            return generate((ast_vardecl*) s);
        case AST_CALL:
            return generate((ast_call*) s);
        case AST_SENTBLOCK:
            return generate((ast_sentblock*) s);
        case AST_ASSIGN:
            return generate((ast_assign*) s);
        case AST_FOREACH:
            return generate((ast_foreach*) s);
        case AST_IF:
            return generate((ast_if*) s);
        case AST_WHILE:
            return generate((ast_while*) s);
        case AST_NOP:
            return generate((ast_nop*) s);
        case AST_RETURN:
            return generate((ast_return*) s);
        case AST_BFS:
            return generate((ast_bfs*) s);
        default:
            printf("\n Generation Code not defined For Sentence:\n");
            s->reproduce(0); // for compiler debug
            printf("\n");
            assert(false);
            break;
    }
    return true;
}

class collect_syms_t : public gm_apply {
public: 
    virtual bool apply(gm_symtab_entry* e, int type) { 
        if ((type == GM_SYMTAB_VAR) || (type == GM_SYMTAB_FIELD)) 
            _all.push_back(e);
        return true;
    }
    std::list<gm_symtab_entry*> _all;
};

bool gm_cpp_gen::generate(ast_foreach* f)
{
    
    int ptr,indent;
    bool need_init_before = get_lib()->need_init_before_iteration_header(f);

    if (need_init_before)
    {
        assert(f->get_parent() -> get_nodetype() == AST_SENTBLOCK);
        get_lib()->generate_init_before_iteration_header(f);
    }

    //Body.NL();
    if (f->is_parallel()) {
        Body.NL();
        prepare_parallel_for();
    }

    // Iteration header generation is dependent on library
    bool need_init_inside;
    int iter_type = f->get_iter_type();;


    bool b = get_lib()->generate_iteration_header(f, need_init_inside);
    bool need_level_check = gm_is_iteration_on_updown_levels(iter_type);
    if (need_init_inside)
    {
        Body.pushln("{");
        get_lib()->generate_iteration_header_init(f);

        if (need_level_check) {
            sprintf(temp, "if (_BFS.level[%s] != (_BFS.curr_level %c 1)) continue;",
                    f->get_iterator()->get_genname(),
                    gm_is_iteration_on_up_neighbors(iter_type) ? '-' : '+');
            Body.pushln(temp);
        }

        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            b &= generate(f->get_body());
        }
        else {
            b &= generate((ast_sentblock*)f->get_body(), false);
        }
        Body.pushln("}");
    } else {
        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            Body.push_indent();
        }
        b &= generate(f->get_body());
        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            Body.pop_indent();
            Body.NL();
        }
    } 

    if (f->is_parallel()) {
        generate_parallel_for_header(f->get_body());
    }

    return b;
}

bool gm_cpp_gen::generate(ast_call* c)
{
    assert(c->is_builtin_call());
    bool b = generate(c->get_builtin());
    Body.pushln(";");
    return b;
}

bool gm_cpp_gen::generate(ast_if* f)
{
    bool b;
    Body.push("if (");
    b &= generate(f->get_cond());
    Body.pushln(")");

    if (f->get_then()->get_nodetype()!=AST_SENTBLOCK) {
        Body.push_indent();
        b &= generate(f->get_then());
        Body.pop_indent();
        //if (f->get_else() == NULL) Body.NL();
    }
    else {
        b &= generate(f->get_then());
    }

    if (f->get_else() == NULL) return b;

    Body.pushln("else ");

    if (f->get_else()->get_nodetype()!=AST_SENTBLOCK) {
        Body.push_indent();
        b &= generate(f->get_else());
        Body.pop_indent();
        //Body.NL();
    }
    else {
        b &= generate(f->get_else());
    }

    return b;
}

bool gm_cpp_gen::generate(ast_while* f)
{
    bool b;
    if (f->is_do_while()) {
        Body.pushln("do ");
        b = generate(f->get_body()); // must be a sentence block
        Body.push("while (");
        b &= generate(f->get_cond());
        Body.pushln(");");
    }
    else {
        Body.push("while (");
        b = generate(f->get_cond());
        Body.pushln(")");
        b &= generate(f->get_body());
    }

    return b;
}


// t: type (node_prop)
// id: field name
void gm_cpp_gen::declare_prop_def(ast_typedecl* t, ast_id * id)
{
    ast_typedecl* t2  = t->get_target_type();
    assert(t2!=NULL);
    assert(t2->is_primitive());

    Body.push(" = new ");
    Body.push(get_type_string(t2, GMUSE_LOCALDEF));
    Body.push(" [ ");
    if (t->is_node_property()) {
        Body.push(get_lib()->max_node_index(t->get_target_graph_id()));
    } else if (t->is_edge_property()) {
        Body.push(get_lib()->max_edge_index(t->get_target_graph_id()));
    }
    else {assert(false);}
    Body.push("]; ");

    sprintf(temp,"_MEM.save(%s, 0, %s);", id->get_genname(), THREAD_ID);
    Body.pushln(temp);

    // regeister to memory controller 
}

bool gm_cpp_gen::generate(ast_vardecl* v)
{
    ast_typedecl* t = v->get_type();
    Body.push_spc(get_type_string(t, GMUSE_LOCALDEF));

    if (t->is_property()) {
        ast_idlist* idl = v->get_idlist();
        assert(idl->get_length() == 1);
        generate(v->get_idlist());
        declare_prop_def(t, idl->get_item(0));

    } else if (t->is_collection()) {
        ast_idlist* idl = v->get_idlist();
        assert(idl->get_length() == 1);
        generate(v->get_idlist());
        get_lib()->add_set_def(idl->get_item(0));
    } else {
        generate(v->get_idlist());
        Body.pushln(";");
    }
    return true;
}
bool gm_cpp_gen::generate(ast_sentblock* sb, bool need_br)
{
   bool b = true;
   //Body.push_indent();

   if (is_target_omp()) {
        ast_extra_info* i = sb->find_info(LABEL_PAR_SCOPE);
        if ((i!= NULL) && (i->bval)) {
            assert(is_under_parallel_sentblock() == false);
            set_under_parallel_sentblock(true);
            need_br = true;
            Body.pushln("#pragma omp parallel");
        }
   }

   if (need_br)
        Body.pushln("{");


   std::list<ast_sent*>& sents = sb->get_sents(); 
   std::list<ast_sent*>::iterator it;
   for(it = sents.begin(); it!= sents.end(); it++)
   {
       b &= generate(*it);
   }

   //Body.pop_indent();
   if (need_br)
        Body.pushln("}");

    if (is_under_parallel_sentblock())
        set_under_parallel_sentblock(false);

   return b;
}


bool gm_cpp_gen::generate_lhs(ast_assign *a)
{
  if (a->get_lhs_type() == GMASSIGN_LHS_SCALA) 
        generate(a->get_lhs_scala());
  else
        generate(a->get_lhs_field());
}

bool gm_cpp_gen::generate_reduce_assign(ast_assign *a)
{
  // implement reduction using compare and swap
      //---------------------------------------
      //  { 
      //    <type> OLD, NEW
      //    do {
      //      OLD = LHS;
      //      NEW = LHS <op> RHS;
      //      <optional break>
      //    } while (!__bool_comp_swap(&LHS, OLD, NEW))
      //  }
      //---------------------------------------
      const char* temp_var_old;
      const char* temp_var_new;

      ast_typedecl* lhs_target_type = (a->get_lhs_type() == GMASSIGN_LHS_SCALA) ? 
          a->get_lhs_scala() -> getTypeInfo() :
          a->get_lhs_field() -> getTypeInfo()->get_target_type() ;

      const char* temp_var_base = (a->get_lhs_type() == GMASSIGN_LHS_SCALA) ? 
          a->get_lhs_scala() -> get_orgname() :
          a->get_lhs_field() -> get_second()->get_orgname();

      int rtype = a->get_reduce_type();

      if (a->get_lhs_type() == GMASSIGN_LHS_SCALA)
            return generate_reduce_main(a->get_lhs_scala(), NULL, a->get_rhs(), temp_var_base, rtype, lhs_target_type);
      else
            return generate_reduce_main(NULL, a->get_lhs_field(), a->get_rhs(), temp_var_base, rtype, lhs_target_type);
}

bool gm_cpp_gen::generate_reduce_main(ast_id* lhs1, ast_field* lhs2, ast_expr* rhs, const char* temp_var_base, int r_type, ast_typedecl* lhs_target_type)
{
      const char* temp_var_old;
      const char* temp_var_new;

      temp_var_old = TEMP_GEN.getTempName(temp_var_base, "_old");
      temp_var_new = TEMP_GEN.getTempName(temp_var_base, "_new");

      Body.pushln("// reduction");
      Body.pushln("{ ");
        sprintf(temp, "%s %s, %s;", get_type_string(lhs_target_type, GMUSE_LOCALDEF), temp_var_old, temp_var_new); Body.pushln(temp);

        Body.pushln("do {");
            sprintf(temp, "%s = ", temp_var_old); Body.push(temp); 
            if (lhs1 != NULL) generate(lhs1); else if (lhs2 != NULL) generate(lhs2); else assert(false);
            Body.pushln(";");
            if (r_type = GMREDUCE_PLUS) {
                sprintf(temp, "%s = %s + (", temp_var_new, temp_var_old); Body.push(temp); 
            } else if (r_type == GMREDUCE_MULT) {
                sprintf(temp, "%s = %s * (", temp_var_new, temp_var_old); Body.push(temp); 
            } else if (r_type == GMREDUCE_MAX) {
                sprintf(temp, "%s = std::max (%s, ", temp_var_new, temp_var_old); Body.push(temp); 
            } else if (r_type == GMREDUCE_MIN) {
                sprintf(temp, "%s = std::min (%s, ", temp_var_new, temp_var_old); Body.push(temp); 
            } else {assert(false);}

            generate(rhs); Body.pushln(");");
            if ((r_type == GMREDUCE_MAX) || (r_type == GMREDUCE_MIN)) {
                sprintf(temp, "if (%s == %s) break;", temp_var_old, temp_var_new); Body.pushln(temp);
            }
        Body.push("} while (_gm_CAS(&("); 
            if (lhs1 != NULL) generate(lhs1); else if (lhs2 != NULL) generate(lhs2); else assert(false);
        sprintf(temp, "), %s, %s)==false); ", temp_var_old, temp_var_new); Body.pushln(temp);
        Body.pushln("}");

        //add_local_varname(temp_var_old);
        //add_local_varname(temp_var_new);

      delete [] temp_var_new;
      delete [] temp_var_old;

  return true;
}

bool gm_cpp_gen::generate(ast_return *r)
{
    ast_extra_info* info =  get_current_proc()->find_info(LABEL_NEED_MEM);
    assert(info != NULL);
    bool need_cleanup = info->bval;
    if (need_cleanup) { // call cleanup before return
        if (r->get_parent()->get_nodetype() != AST_SENTBLOCK)
        {
            Body.pop_indent();
            Body.push("{");
        }
        Body.pushln("_MEM.cleanup();");
        Body.push("return ");
        if (r->get_expr() != NULL) {
            generate(r->get_expr());
        }
        Body.pushln("; ");
        if (r->get_parent()->get_nodetype() != AST_SENTBLOCK)
        {
            Body.pushln("}");
            Body.push_indent();
        }
    } else {
        Body.push("return ");
        if (r->get_expr() != NULL) {
            generate(r->get_expr());
        }
        Body.pushln(";");
    }
}

bool gm_cpp_gen::generate(ast_assign* a)
{
  //------------------------------
  // plain assign
  //------------------------------
  if (a->get_assign_type() == GMASSIGN_NORMAL)
  {
      generate_lhs(a);
      Body.push(" = ");
      generate(a->get_rhs());
      Body.pushln(";");
      return true;
  }
  else if (a->get_assign_type() == GMASSIGN_REDUCE)
  {
     return generate_reduce_assign(a);
  }
  else
  {
     assert(false);
  }
}

bool gm_cpp_gen::generate(ast_nop* n)
{
    switch(n->get_subtype())
    {
        /* If this is something I know, process it myself */
        case NOP_CLEAR_PROP:
        {
            Body.push("_MEM.clear(");
            Body.push(
                    ((nop_propdel*) n)->get_target() -> getId()->get_genname());
            sprintf(temp, ", 0, %s);", THREAD_ID);
            Body.pushln(temp);
            break;
        }

        case NOP_BFS_INIT:
        {
            generate_bfs_top();
            break;
        }

        case NOP_FUNC_EXIT:
        {
            Body.pushln("_MEM.cleanup();");
            break;
        }
        case NOP_MEM_INIT:
        {
            sprintf(temp, "_gm_mem_helper _MEM(%s);", MAX_THREADS);
            Body.pushln(temp);
            break;
        }

        case NOP_REDUCE_SCALAR:
        {
            ((nop_reduce_scalar*) n)->generate(this);
            break;
        }

        /* otherwise ask library to hande it */
        default:
        { 
            return get_lib() -> generate(n);
        }
    }

    return true;
}

bool gm_cpp_gen::generate(ast_field* f)
{
    Body.push( f->get_second()->get_genname());
    Body.push('[');
    if (f->getTypeInfo()->is_node_property())
        Body.push(get_lib()->node_index(f->get_first()));
    else if (f->getTypeInfo()->is_edge_property())
        Body.push(get_lib()->edge_index(f->get_first()));
    else {
        assert(false);
    }
    Body.push(']');
    return true;
}

bool gm_cpp_gen::generate(std::list<ast_expr*>& L)
{
    std::list<ast_expr*>::iterator I;
    int i=0;
    int size = L.size();
    for(I=L.begin(); I!=L.end(); I++, i++) {
        generate(*I);
        if (i!=(size-1))
            Body.push(", ");
    }
}


bool gm_cpp_gen::generate(ast_expr* e)
{
    char temp[128];
    switch (e->get_opclass())
    {
        case GMEXPR_IVAL:
            sprintf(temp,"%ld",e->get_ival()); // to be changed
            Body.push(temp);
            return true;
        case GMEXPR_FVAL:
            sprintf(temp,"%lf",e->get_fval()); // to be changed
            Body.push(temp);
            return true;
        case GMEXPR_BVAL:
            sprintf(temp,"%s",e->get_bval()?"true":"false");
            Body.push(temp);
            return true;
        case GMEXPR_INF:
            {int t = e->get_type_summary();
             char* str;
             switch(t) {
                case GMTYPE_INF:
                case GMTYPE_INF_INT:
                    sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); // temporary
                    break;
                case GMTYPE_INF_LONG:
                    sprintf(temp,"%s",e->is_plus_inf()?"LLONG_MAX":"LLONG_MIN"); // temporary
                    break;
                case GMTYPE_INF_FLOAT:
                    sprintf(temp,"%s",e->is_plus_inf()?"FLT_MAX":"FLT_MIN"); // temporary
                    break;
                case GMTYPE_INF_DOUBLE:
                    sprintf(temp,"%s",e->is_plus_inf()?"DBL_MAX":"DBL_MIN"); // temporary
                    break;
                default: //assert(false);
                //case GMTYPE_INF:
                //case GMTYPE_INF_INT:
                    sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); // temporary
                    break;
                //case GMTYPE_INF_LONG:
             }
           }
            Body.push(temp);
            return true;

        case GMEXPR_ID:
            return generate(e->get_id());
        case GMEXPR_BUILTIN:
            return get_lib()->generate_builtin((ast_expr_builtin*)e);

        case GMEXPR_FIELD:
            return generate(e->get_field());

        case GMEXPR_UOP:
            if (e->get_optype() == GMOP_NEG) {
                Body.push(" -");
                return generate(e->get_left_op());
            } else if (e->get_optype() == GMOP_ABS) {
                Body.push(" std::abs(");
                bool b =  generate(e->get_left_op());
                Body.push(") ");
                return b;
            } else if (e->get_optype() == GMOP_TYPEC) {
                Body.push(" (");
                Body.push(get_type_string(e->get_type_summary(), GMUSE_LOCALDEF));
                Body.push(") ");
                return  generate(e->get_left_op());
            }
            else {
                assert(false);
                return false;
            }
        case GMEXPR_LUOP:
            Body.push(" !");
            return generate(e->get_left_op());

        case GMEXPR_TER:
            {
            bool need_para = 
                (e->get_up_op() == NULL) ? false :
                gm_need_paranthesis(e->get_optype(), e->get_up_op()->get_optype(), e->is_right_op());
            if (need_para) Body.push("(");
            generate(e->get_cond_op());
            Body.push("?");
            generate(e->get_left_op());
            Body.push(":");
            generate(e->get_right_op());
            if (need_para) Body.push(")");
            return true;
            }
    }

    assert(e->is_biop() || e->is_comp()); // numeric or logical

    // binary op
    ast_expr* up = e->get_up_op();
    bool need_para; 
    if ((e->get_optype() == GMOP_MIN) || (e->get_optype() == GMOP_MAX))
    {
        need_para = true;
    }
    else if (up==NULL)
        need_para=false;
    else if (e->is_comp()) {
        need_para = true;  // I found it helpful to add () for comparison
    }
    else if (up->is_biop() || up->is_comp()) {  
        need_para = gm_need_paranthesis(e->get_optype(),up->get_optype(), e->is_right_op());
    } else  {
        need_para = true;
    }

    if ((e->get_optype() == GMOP_MIN) || (e->get_optype() == GMOP_MAX)) {
        if (e->get_optype() == GMOP_MIN) {
            Body.push("std::min");
        } else {
            Body.push("std::max");
        }
    }

    if (need_para) {
        Body.push("(");
    }

    bool l = generate(e->get_left_op());
    Body.SPC();
    if ((e->get_optype() == GMOP_MIN) || (e->get_optype() == GMOP_MAX)) {
        Body.push(", ");
    } else {
        const char* opstr = gm_get_op_string(e->get_optype());
        Body.push_spc(opstr);
    } 
    bool r = generate(e->get_right_op());
    if (need_para) {
        Body.push(")");
    }
    return l&&r;
}

void gm_cpp_gen::prepare_parallel_for()
{
    //_ptr = Body.get_write_ptr(_indent);
    //clear_local_varnames();  // no need

    if (is_under_parallel_sentblock())
        Body.pushln("#pragma omp for nowait"); // TODO why no wait?
    else
        Body.pushln("#pragma omp parallel for");
}

void gm_cpp_gen::generate_parallel_for_header(ast_sent* body)
{
    //int i = sprintf(temp, "#pragma omp parallel for \n");

    // No need

    /*
    // collect symbols in symbol table
    collect_syms_t T;
    gm_traverse_symtabs(body, &T);
    std::list<gm_symtab_entry*>::iterator J;
    for(J=T._all.begin(); J!=T._all.end(); J++)
        add_local_varname((*J)->getId()->get_genname());


    // temporary symbols were collected during code generation
    if (local_names.size() > 0) {
        i += sprintf(&temp[i], "private(");
        std::list<const char*>::iterator I;
        for(I=local_names.begin(); I!=local_names.end(); I++) {
            i+= sprintf(&temp[i], "%s, ", *I);
        }
        temp[i-2] = ')'; // remove final ','
    }
    sprintf(&temp[i], " \n");

    Body.insert_at(_ptr, _indent, temp); 
    clear_local_varnames();
    */
}



