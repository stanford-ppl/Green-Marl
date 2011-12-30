#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

#define HELPERS "gm_helper_functions.h"

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


void gm_cpp_gen::do_generate_begin()
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
    Header.NL();

    //----------------------------------------
    // Body
    //----------------------------------------
    sprintf(temp, "%s.h", fname);
    add_include(temp, Body, false);
    Body.NL();
}


void gm_cpp_gen::do_generate_end()
{
    Header.NL();
    Header.pushln("#endif");
}


void gm_cpp_gen::generate_proc(ast_procdef* proc)
{
    //-------------------------------
    // declare function name 
    //-------------------------------
    generate_proc_decl(proc, false);  // declare in header file

    //-------------------------------
    // BFS definitions
    //-------------------------------
    if (proc->find_info_bool(CPPBE_INFO_HAS_BFS)) 
    {
        ast_extra_info_list* L = (ast_extra_info_list*) proc->find_info(CPPBE_INFO_BFS_LIST); 
        assert(L!=NULL);
        std::list<void*>::iterator I;
        Body.NL();
        Body.pushln("// BFS/DFS definitions for the procedure");
        for(I=L->get_list().begin(); I!=L->get_list().end(); I++)
        {
            ast_bfs* bfs = (ast_bfs*) *I;
            generate_bfs_def(bfs);
        }
    }

    //-------------------------------
    // function definition
    //-------------------------------
    generate_proc_decl(proc, true);   // declare in body file
    generate_sent(proc->get_body());
    Body.NL();

    return ;
}

void gm_cpp_gen::generate_proc_decl(ast_procdef* proc, bool is_body_file)
{
     // declare in the header or body
     gm_code_writer& Out = is_body_file ? Body : Header;

     // return type
     Out.push_spc(get_type_string(proc->get_return_type()));
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

            ast_typedecl* T = (*i)->get_type();
            Out.push(get_type_string(T) );
            if (T->is_primitive() || T->is_property())
                Out.push(" ");
            else
                Out.push("& ");

            assert((*i)->get_idlist()->get_length() == 1);
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

            Out.push(get_type_string((*i)->get_type()) );
            Out.push_spc("&");
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
     if (!is_body_file)
        Out.push(';');

     Out.NL();
     return;
}

void gm_cpp_gen::generate_idlist(ast_idlist* idl)
{
    int z = idl->get_length();
    for(int i=0; i < z;i++) {
        ast_id* id = idl->get_item(i);
        generate_lhs_id(id);
        if (i < z-1) Body.push_spc(',');
    }
    return;
}

void gm_cpp_gen::generate_lhs_id(ast_id* id) 
{
    Body.push(id->get_genname());
}
void gm_cpp_gen::generate_rhs_id(ast_id* id)  
{
    generate_lhs_id(id);
}
void gm_cpp_gen::generate_lhs_field(ast_field* f)
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
    return; 
}

void gm_cpp_gen::generate_rhs_field(ast_field* f)
{
    generate_lhs_field(f);
}


const char* gm_cpp_gen::get_type_string(int prim_type)
{
    switch(prim_type) {
        case GMTYPE_BYTE:  return "int8_t"  ;
        case GMTYPE_SHORT: return "int16_t" ;
        case GMTYPE_INT:   return "int32_t" ;
        case GMTYPE_LONG:  return "int64_t" ;
        case GMTYPE_FLOAT: return  "float" ;
        case GMTYPE_DOUBLE: return "double";
        case GMTYPE_BOOL: return "bool";
        default: assert(false); return "??";
    }
}

const char* gm_cpp_gen::get_type_string(ast_typedecl* t)
{
    if ((t==NULL) || (t->is_void())) {
        return "void";
    }

    if (t->is_primitive()) {
        return get_type_string(t->get_typeid());
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
        return get_lib()->get_type_string(t);

    return "ERROR";
}


void gm_cpp_gen::generate_sent_foreach(ast_foreach* f)
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


    get_lib()->generate_iteration_header(f, need_init_inside);
    bool need_level_check = gm_is_iteration_on_updown_levels(iter_type);
    if (need_init_inside)
    {
        Body.pushln("{");
        get_lib()->generate_iteration_header_init(f);

        // [xxx]
        if (need_level_check) {
            sprintf(temp, "if (_BFS.level[%s] != (_BFS.curr_level %c 1)) continue;",
                    f->get_iterator()->get_genname(),
                    gm_is_iteration_on_up_neighbors(iter_type) ? '-' : '+');
            Body.pushln(temp);
        }

        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            generate_sent(f->get_body());
        }
        else {
            generate_sent_block((ast_sentblock*)f->get_body(), false);
        }
        Body.pushln("}");

    } else {
        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            Body.push_indent();
        }
        generate_sent(f->get_body());
        if (f->get_body()->get_nodetype() != AST_SENTBLOCK) {
            Body.pop_indent();
            Body.NL();
        }
    } 

    return;
}

void gm_cpp_gen::generate_sent_call(ast_call* c)
{
    assert(c->is_builtin_call());
    generate_expr_builtin(c->get_builtin());
    Body.pushln(";");
}


// t: type (node_prop)
// id: field name
void gm_cpp_gen::declare_prop_def(ast_typedecl* t, ast_id * id)
{
    ast_typedecl* t2  = t->get_target_type();
    assert(t2!=NULL);
    assert(t2->is_primitive());

    Body.push(" = ");
    switch(t2->getTypeSummary()) {
        case GMTYPE_INT:    Body.push(ALLOCATE_INT); break;
        case GMTYPE_LONG:   Body.push(ALLOCATE_LONG); break;
        case GMTYPE_BOOL:   Body.push(ALLOCATE_BOOL); break;
        case GMTYPE_DOUBLE: Body.push(ALLOCATE_DOUBLE); break;
        case GMTYPE_FLOAT:  Body.push(ALLOCATE_FLOAT); break;
        default: assert(false);
    }
    Body.push('(');
    if (t->is_node_property()) {
        Body.push(get_lib()->max_node_index(t->get_target_graph_id()));
    } else if (t->is_edge_property()) {
        Body.push(get_lib()->max_edge_index(t->get_target_graph_id()));
    }
    Body.push(',');
    Body.push(THREAD_ID);
    Body.pushln("());");

    /*
    Body.push(" = new ");
    Body.push(get_type_string(t2));
    Body.push(" [ ");
    if (t->is_node_property()) {
        Body.push(get_lib()->max_node_index(t->get_target_graph_id()));
    } else if (t->is_edge_property()) {
        Body.push(get_lib()->max_edge_index(t->get_target_graph_id()));
    }
    else {assert(false);}
    Body.push("]; ");

    sprintf(temp,"%s(%s, %s());", SAVE_PTR, id->get_genname(), THREAD_ID);
    Body.pushln(temp);
    */

    // regeister to memory controller 
}

void gm_cpp_gen::generate_sent_vardecl(ast_vardecl* v)
{
    ast_typedecl* t = v->get_type();
    Body.push_spc(get_type_string(t));

    if (t->is_property()) {
        ast_idlist* idl = v->get_idlist();
        assert(idl->get_length() == 1);
        generate_lhs_id(idl->get_item(0));
        declare_prop_def(t, idl->get_item(0));

    } else if (t->is_collection()) {
        ast_idlist* idl = v->get_idlist();
        assert(idl->get_length() == 1);
        generate_lhs_id(idl->get_item(0));
        get_lib()->add_set_def(idl->get_item(0));
    } else {
        generate_idlist(v->get_idlist());
        Body.pushln(";");
    }
    return; 
}

void gm_cpp_gen::generate_sent_block(ast_sentblock* sb)
{
    generate_sent_block(sb, true);
}

void gm_cpp_gen::generate_sent_block_enter(ast_sentblock* sb)
{
    if (sb->find_info_bool(CPPBE_INFO_IS_PROC_ENTRY))
    {
        // initialize procedure
        Body.pushln("//Initialize runtime (if required)");
        sprintf(temp,"%s();", RT_INIT);
        Body.pushln(temp);
        Body.NL();
    }
}

void gm_cpp_gen::generate_sent_block(ast_sentblock* sb, bool need_br)
{
   if (is_target_omp()) {
        bool is_par_scope = sb->find_info_bool(LABEL_PAR_SCOPE);
        if (is_par_scope) {
            assert(is_under_parallel_sentblock() == false);
            set_under_parallel_sentblock(true);
            need_br = true;
            Body.pushln("#pragma omp parallel");
        }
   }

   if (need_br) Body.pushln("{");

   // sentblock exit
   generate_sent_block_enter(sb);

   std::list<ast_sent*>& sents = sb->get_sents(); 
   std::list<ast_sent*>::iterator it;
   bool vardecl_started=false;
   bool other_started=false;
   for(it = sents.begin(); it!= sents.end(); it++)
   {
       // insert newline after end of VARDECL
       ast_sent* s = *it;
       if (!vardecl_started) {
           if (s->get_nodetype() == AST_VARDECL) 
               vardecl_started = true;
       }
       else {
           if (other_started == false) {
               if (s->get_nodetype() != AST_VARDECL) {
                   Body.NL();
                   other_started = true;
               }
           }
       }
       generate_sent(*it);
   }

   // sentblock exit
   generate_sent_block_exit(sb);


   if (need_br) Body.pushln("}");

   if (is_under_parallel_sentblock())
        set_under_parallel_sentblock(false);

   return ;
}

void gm_cpp_gen::generate_sent_block_exit(ast_sentblock* sb)
{
    bool has_prop_decl = sb->find_info_bool(CPPBE_INFO_HAS_PROPDECL);
    bool is_proc_entry = sb->find_info_bool(CPPBE_INFO_IS_PROC_ENTRY);
    bool has_return_ahead = gm_check_if_end_with_return(sb);

    if (has_prop_decl && !has_return_ahead)
    {
        if (is_proc_entry)
        {
            Body.NL();
            sprintf(temp, "%s();", CLEANUP_PTR);
            Body.pushln(temp);
        }
        else{
            Body.NL();
            gm_symtab* tab = sb->get_symtab_field();
            std::vector<gm_symtab_entry*>& entries = tab->get_entries();
            std::vector<gm_symtab_entry*>::iterator I;
            for(I=entries.begin(); I!=entries.end(); I++)
            {
                gm_symtab_entry *e = *I;
                sprintf(temp, "%s(%s,%s());", DEALLOCATE, e->getId()->get_genname(), THREAD_ID);
                Body.pushln(temp);
            }
        }
    }

}


void gm_cpp_gen::generate_sent_reduce_assign(ast_assign *a)
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
      ast_typedecl* lhs_target_type = (a->get_lhs_type() == GMASSIGN_LHS_SCALA) ? 
          a->get_lhs_scala() -> getTypeInfo() :
          a->get_lhs_field() -> getTypeInfo()->get_target_type() ;

      const char* temp_var_base = (a->get_lhs_type() == GMASSIGN_LHS_SCALA) ? 
          a->get_lhs_scala() -> get_orgname() :
          a->get_lhs_field() -> get_second()->get_orgname();

      int r_type = a->get_reduce_type();

      /*
      if (a->get_lhs_type() == GMASSIGN_LHS_SCALA)
            generate_reduce_main(a->get_lhs_scala(), NULL, a->get_rhs(), temp_var_base, rtype, lhs_target_type);
      else
            generate_reduce_main(NULL, a->get_lhs_field(), a->get_rhs(), temp_var_base, rtype, lhs_target_type);
        */

      const char* temp_var_old;
      const char* temp_var_new;
      bool is_scalar = (a->get_lhs_type() == GMASSIGN_LHS_SCALA);

      temp_var_old = FE.voca_temp_name_and_add(temp_var_base, "_old");
      temp_var_new = FE.voca_temp_name_and_add(temp_var_base, "_new");

      Body.pushln("// reduction");
      Body.pushln("{ ");

        sprintf(temp, "%s %s, %s;", get_type_string(lhs_target_type), temp_var_old, temp_var_new); Body.pushln(temp);

        Body.pushln("do {");
            sprintf(temp, "%s = ", temp_var_old); Body.push(temp); 
            if (is_scalar) generate_rhs_id(a->get_lhs_scala()); 
            else generate_rhs_field(a->get_lhs_field()); 

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

            generate_expr(a->get_rhs()); Body.pushln(");");
            if ((r_type == GMREDUCE_MAX) || (r_type == GMREDUCE_MIN)) {
                sprintf(temp, "if (%s == %s) break;", temp_var_old, temp_var_new); Body.pushln(temp);
            }
        Body.push("} while (_gm_CAS(&("); 
        if (is_scalar) generate_rhs_id(a->get_lhs_scala()); 
        else generate_rhs_field(a->get_lhs_field()); 

        sprintf(temp, "), %s, %s)==false); ", temp_var_old, temp_var_new); Body.pushln(temp);
        Body.pushln("}");

        //add_local_varname(temp_var_old);
        //add_local_varname(temp_var_new);

      delete [] temp_var_new;
      delete [] temp_var_old;

    return;
}

void gm_cpp_gen::generate_sent_return(ast_return *r)
{
    if (FE.get_current_proc()->find_info_bool(CPPBE_INFO_HAS_PROPDECL))
    {
        Body.push(CLEANUP_PTR);
        Body.pushln("();");
    }

    Body.push("return");
    if (r->get_expr() != NULL) {
        Body.SPC();
        generate_expr(r->get_expr());
    }
    Body.pushln("; ");
}

    /*
    bool need_cleanup = FE.get_current_proc()->find_info_bool(LABEL_NEED_MEM);
    if (need_cleanup) { // call cleanup before return
        bool need_para = (r->get_parent()->get_nodetype() != AST_SENTBLOCK);
        if (need_para)
        {
            Body.pop_indent();
            Body.push("{");
        }
        Body.pushln("_MEM.cleanup();");
        Body.push("return ");
        if (r->get_expr() != NULL) {
            generate_expr(r->get_expr());
        }
        Body.pushln("; ");
        if (need_para)
        {
            Body.pushln("}");
            Body.push_indent();
        }
    } else {
        Body.push("return ");
        if (r->get_expr() != NULL) {
            generate_expr(r->get_expr());
        }
        Body.pushln(";");
    }
    */

void gm_cpp_gen::generate_sent_nop(ast_nop* n)
{
    switch(n->get_subtype())
    {
        case NOP_BFS_INIT:
        {
            //generate_bfs_top();
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
            get_lib() -> generate_sent_nop(n);
            break;
        }
    }

    return; 
}

void gm_cpp_gen::generate_expr_inf(ast_expr *e)
{
    char* temp = temp_str;
    assert(e->get_opclass() == GMEXPR_INF);
    int t = e->get_type_summary();
    char* str;
    switch(t) {
       case GMTYPE_INF:
       case GMTYPE_INF_INT:
            sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); 
            break;
        case GMTYPE_INF_LONG:
                    sprintf(temp,"%s",e->is_plus_inf()?"LLONG_MAX":"LLONG_MIN"); 
                    break;
        case GMTYPE_INF_FLOAT:
            sprintf(temp,"%s",e->is_plus_inf()?"FLT_MAX":"FLT_MIN"); 
            break;
        case GMTYPE_INF_DOUBLE:
            sprintf(temp,"%s",e->is_plus_inf()?"DBL_MAX":"DBL_MIN"); 
            break;
        default:
            printf("what type is it? %d", t);
            assert(false);
            sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); 
            break;
         }
    Body.push(temp);
    return ;
}

void gm_cpp_gen::generate_expr_abs(ast_expr *e)
{
    Body.push(" std::abs(");
    generate_expr(e->get_left_op());
    Body.push(") ");
}
void gm_cpp_gen::generate_expr_minmax(ast_expr *e)
{
    if (e->get_optype() == GMOP_MIN) {
        Body.push(" std::min(");
    } else {
        Body.push(" std::max(");
    }
    generate_expr(e->get_left_op());
    Body.push(",");
    generate_expr(e->get_right_op());
    Body.push(") ");
}

void gm_cpp_gen::generate_expr_builtin(ast_expr* e)
{
  get_lib()->generate_expr_builtin((ast_expr_builtin*)e);
}

void gm_cpp_gen::prepare_parallel_for()
{
    if (is_under_parallel_sentblock())
        Body.pushln("#pragma omp for nowait"); // already under parallel region.
    else
        Body.pushln("#pragma omp parallel for");
}


