
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

const char* gm_gps_gen::get_type_string(int gm_type)
{
    switch(gm_type) {
        case GMTYPE_INT: return "int";
        case GMTYPE_LONG: return "long";
        case GMTYPE_FLOAT: return "float";
        case GMTYPE_DOUBLE: return "double";
        case GMTYPE_BOOL: return "boolean";
        case GMTYPE_NODE: if (get_lib()->is_node_type_int()) return "int";
                          else return "long";
        default: assert(false);
    }
}

const char* gm_gps_gen::get_type_string(ast_typedecl* T, bool is_master)
{
    if (T->is_primitive() || T->is_node()) 
    {
        return (get_type_string(T->get_typeid()));
    }
    else {
        assert(false); // to be done
    }
    return "???";
}

void gm_gps_gen::generate_expr_minmax(ast_expr* e)
{
    if (e->get_optype() == GMOP_MIN) 
        _Body.push("java.math.min(");
    else if (e->get_optype() == GMOP_MAX) {
        _Body.push("java.math.max(");
    } 
    else {assert(false);}

    generate_expr(e->get_left_op());
    _Body.push(",");
    generate_expr(e->get_right_op());
    _Body.push(")");
}

void gm_gps_gen::generate_lhs_id(ast_id* i) 
{
    _Body.push(i->get_genname());
}
void gm_gps_gen::generate_lhs_field(ast_field* f)
{
    ast_id* prop = f->get_second();
    if (is_master_generate()) {
       assert(false);
    } 
    else if (is_receiver_generate()) {
        if (f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL)
        {
            get_lib()->generate_vertex_prop_access_remote_lhs(prop, Body);
        }
        else 
        {
            get_lib()->generate_vertex_prop_access_lhs(prop, Body);
        }
    }
    else {// vertex generate;
        assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);
        get_lib()->generate_vertex_prop_access_lhs(prop, Body);
    }
}

void gm_gps_gen::generate_rhs_id(ast_id* i) 
{
    if (i->getSymInfo()->getType()->is_node_iterator())
    {
        get_lib()->generate_node_iterator_rhs(i, Body);
    }
    else {
        generate_lhs_id(i);
    }
} 

void gm_gps_gen::generate_rhs_field(ast_field* f)
{
    generate_lhs_field(f);
}


void gm_gps_gen::generate_sent_reduce_assign(ast_assign* a) 
{
    if (is_master_generate())
    {
        assert(false);
    }
    else if (a->is_target_scalar()) 
    {
        // check target is global
        {
            get_lib()->generate_reduce_assign_vertex(a, Body, a->get_reduce_type());
        }
        return;
    }

    //--------------------------------------------------
    // target is vertex-drivven
    // reduction now became normal read/write
    //--------------------------------------------------
    if (a->is_argminmax_assign()) {
        assert((a->get_reduce_type() == GMREDUCE_MIN) ||
                (a->get_reduce_type() == GMREDUCE_MAX));

        Body.push("if (");
        if (a->is_target_scalar())
            generate_rhs_id(a->get_lhs_scala());
        else 
            generate_rhs_field(a->get_lhs_field());

        if (a->get_reduce_type() == GMREDUCE_MIN)
            Body.push(" > ");
        else
            Body.push(" < ");
        
        generate_expr(a->get_rhs());
        Body.pushln(") {");
        if (a->is_target_scalar())
            generate_lhs_id(a->get_lhs_scala());
        else 
            generate_lhs_field(a->get_lhs_field());
        Body.push(" = ");
        generate_expr(a->get_rhs());
        Body.pushln(";");

        std::list<ast_node*>& lhs_list = a->get_lhs_list();
        std::list<ast_expr*>& rhs_list = a->get_rhs_list();
        std::list<ast_node*>::iterator I;
        std::list<ast_expr*>::iterator J;
        J = rhs_list.begin();
        for(I=lhs_list.begin(); I!=lhs_list.end(); I++, J++)
        {
            ast_node* n = *I;
            if (n->get_nodetype()==AST_ID) {
                generate_lhs_id((ast_id*)n);
            }
            else {
                generate_lhs_field((ast_field*) n);
            }
            Body.push(" = ");
            generate_expr(*J);
            Body.pushln(";");
        }

        Body.pushln("}");
    }
    else {
        if (a->is_target_scalar())
        {
            generate_lhs_id(a->get_lhs_scala());
        }
        else {
            generate_lhs_field(a->get_lhs_field());
        }

        Body.push(" = ");

        if ((a->get_reduce_type() == GMREDUCE_PLUS) ||
            (a->get_reduce_type() == GMREDUCE_MULT) ||
            (a->get_reduce_type() == GMREDUCE_AND) ||
            (a->get_reduce_type() == GMREDUCE_OR))
        {
            if (a->is_target_scalar())
                generate_rhs_id(a->get_lhs_scala());
            else 
                generate_rhs_field(a->get_lhs_field());

            switch(a->get_reduce_type()) {
                case GMREDUCE_PLUS: Body.push(" + ("); break;
                case GMREDUCE_MULT: Body.push(" * ("); break;
                case GMREDUCE_AND: Body.push(" && ("); break;
                case GMREDUCE_OR: Body.push(" || ("); break;
                default: assert(false);
            }
            generate_expr(a->get_rhs());
            Body.pushln(");");
        }
        else if ((a->get_reduce_type() == GMREDUCE_MIN) ||
             (a->get_reduce_type() == GMREDUCE_MAX))
        {
            if (a->get_reduce_type() == GMREDUCE_MIN) 
                Body.push("java.lang.Min(");
            else
                Body.push("java.lang.Max(");

            if (a->is_target_scalar())
                generate_rhs_id(a->get_lhs_scala());
            else 
                generate_rhs_field(a->get_lhs_field());
            Body.push(",");
            generate_expr(a->get_rhs());

            Body.pushln(");");
        }
        else {
            assert(false);
        }
    }
}


void gm_gps_gen::generate_sent_assign(ast_assign *a)
{
    // normal assign
    if (is_master_generate()) {
        this->gm_code_generator::generate_sent_assign(a);
    }
    else if (a->is_target_scalar())
    {
        ast_id* i = a->get_lhs_scala();

        gps_syminfo* syminfo = (gps_syminfo*) 
            i->getSymInfo()->find_info(TAG_BB_USAGE);

        // normal assign
        //if ((syminfo == NULL) || (!syminfo->is_used_in_multiple_BB()))
        if (!syminfo->is_scoped_global())
        {
            this->gm_code_generator::generate_sent_assign(a);
            return;
        }
        else {
            // write to global scalar

            // to be done
            printf("error: %s\n", i->get_genname());
            assert(false);
        }
    }
    else 
    {
        ast_field* f = a->get_lhs_field(); 

        // temporary
        assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);

        this->gm_code_generator::generate_sent_assign(a);
    }
}

void gm_gps_gen::generate_sent_return(ast_return *r)
{
    if (r->get_expr() != NULL) {
        _Body.push(GPS_RET_VALUE);
        _Body.push(" = ");
        generate_expr(r->get_expr());
        _Body.pushln(";");
    }
}

void gm_gps_gen::generate_sent_foreach(ast_foreach* fe)
{
    // must be a sending foreach
    assert(gm_is_iteration_on_out_neighbors(fe->get_iter_type()) ||
           gm_is_iteration_on_down_neighbors(fe->get_iter_type()));

    get_lib()->generate_message_send(fe, Body);
}

void gm_gps_gen::generate_expr_builtin(ast_expr *e)
{
    ast_expr_builtin * be = (ast_expr_builtin*) e;
    gm_builtin_def* def = be->get_builtin_def(); 
    std::list<ast_expr*>& ARGS = be->get_args();
    
    switch(def->get_method_id())
    {
    case GM_BLTIN_TOP_DRAND:         // rand function
    case GM_BLTIN_TOP_IRAND:         // rand function
        get_lib()->generate_expr_builtin(be, Body, is_master_generate());
        break;

    case GM_BLTIN_TOP_LOG:           // log function
        Body.push("Math.log(");
        assert(ARGS.front()!=NULL);
        generate_expr(ARGS.front());
        Body.push(")");
        break;
    case GM_BLTIN_TOP_EXP:           // exp function
        Body.push("Math.exp(");
        assert(ARGS.front()!=NULL);
        generate_expr(ARGS.front());
        Body.push(")");
        break;
    case GM_BLTIN_TOP_POW:           // pow function
        Body.push("Math.pow(");
        assert(ARGS.front()!=NULL);
        generate_expr(ARGS.front());
        Body.push(",");
        assert(ARGS.back()!=NULL);
        generate_expr(ARGS.back());
        Body.push(")");
        break;
    default:
        assert(false);
    }
}

void gm_gps_gen::generate_expr_abs(ast_expr * e)
{
    Body.push("Math.abs(");
    generate_expr(e->get_left_op());
    Body.push(")");
}


void gm_gps_gen::generate_expr_inf(ast_expr *e)
{
    char temp[256];
    assert(e->get_opclass() == GMEXPR_INF);
    int t = e->get_type_summary();
    switch(t) {
       case GMTYPE_INF:
       case GMTYPE_INF_INT:
            sprintf(temp,"Integer.%s",e->is_plus_inf()?"MAX_VALUE":"MIN_VALUE"); // temporary
            break;
        case GMTYPE_INF_LONG:
                    sprintf(temp,"Long.%s",e->is_plus_inf()?"MAX_VALUE":"MIN_VALUE"); // temporary
                    break;
        case GMTYPE_INF_FLOAT:
            sprintf(temp,"Float.%s",e->is_plus_inf()?"MAX_VALUE":"MIN_VALUE"); // temporary
            break;
        case GMTYPE_INF_DOUBLE:
            sprintf(temp,"Double.%s",e->is_plus_inf()?"MAX_VALUE":"MIN_VALUE"); // temporary
            break;
        default: 
                    sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); // temporary
                    break;
         }
    _Body.push(temp);
    return ;
}

