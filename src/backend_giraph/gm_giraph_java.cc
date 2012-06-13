#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

void gm_giraph_gen::generate_rhs_id(ast_id* i)
{
    if (i->getSymInfo()->getType()->is_node_iterator())
    {
        if (i->getSymInfo()->find_info_bool(GPS_FLAG_COMM_SYMBOL))
        {
            if  (!this->is_receiver_generate())
            {
                get_lib()->generate_node_iterator_rhs(i, Body);
            }
            else {
                generate_lhs_id(i);
            }
        }
        else {
            //generate_lhs_id(i);
            get_lib()->generate_node_iterator_rhs(i, Body);
        }
    }
    else {
        generate_lhs_id(i);
    }
}

void gm_giraph_gen::generate_sent_foreach(ast_foreach* fe)
{
    // must be a sending foreach
    assert(gm_is_iteration_on_out_neighbors(fe->get_iter_type()) ||
           gm_is_iteration_on_in_neighbors(fe->get_iter_type()) ||
           gm_is_iteration_on_down_neighbors(fe->get_iter_type())
           );

    get_lib()->generate_message_send(fe, Body);
}

void gm_giraph_gen::generate_sent_reduce_assign(ast_assign* a)
{
    if (is_master_generate())
    {
        // [to be done]
        assert(false);
    }

    if (a->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN) != NULL)
    {
        if (!is_receiver_generate())
        {
            // generate random write messaging
            get_lib()->generate_message_payload_packing_for_random_write(a, Body);
            return ;
        }
    }

    if (a->is_target_scalar())
    {
        // check target is global
        {
            get_lib()->generate_reduce_assign_vertex(a, Body, a->get_reduce_type());
        }
        return;
    }

    //--------------------------------------------------
    // target is vertex-driven
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
                default: assert(false); break;
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

void gm_giraph_gen::generate_expr_builtin(ast_expr *e)
{
    ast_expr_builtin * be = (ast_expr_builtin*) e;
    gm_builtin_def* def = be->get_builtin_def();
    std::list<ast_expr*>& ARGS = be->get_args();

    switch(def->get_method_id())
    {
    case GM_BLTIN_TOP_DRAND:         // rand function
    case GM_BLTIN_TOP_IRAND:         // rand function
    case GM_BLTIN_GRAPH_NUM_NODES:
    case GM_BLTIN_NODE_DEGREE:
    case GM_BLTIN_NODE_IN_DEGREE:
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
        assert(false); break;
    }
}
