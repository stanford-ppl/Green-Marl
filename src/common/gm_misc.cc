
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"

const char* gm_get_nodetype_string(int t)
{
    switch(t) {
        case AST_ID: return "AST_ID";
        case AST_FIELD: return "AST_FIELD";
        case AST_PROCDEF: return "AST_PROCDEF";
        case AST_EXPR: return "AST_EXPR";
        case AST_EXPR_RDC: return "AST_EXPR_RDC";
        case AST_EXPR_BUILTIN: return "AST_EXPR_BUILTIN";
        case AST_EXPR_FOREIGN: return "AST_EXPR_FOREIGN";
        case AST_SENT: return "AST_SENT";
        case AST_SENTBLOCK: return "AST_SENTBLOCK";
        case AST_ASSIGN: return "AST_ASSIGN";
        case AST_VARDECL: return "AST_VARDECL";
        case AST_FOREACH: return "AST_FOREACH";
        case AST_IF: return "AST_IF";
        case AST_WHILE: return "AST_WHILE";
        case AST_RETURN: return "AST_RETURN";
        case AST_BFS: return "AST_BFS";
        case AST_CALL: return "AST_CALL";
        case AST_FOREIGN: return "AST_FOREIGN";
        case AST_NOP: return "AST_NOP";
        default: return "?";
    }
}

// for debug-print only
const char* gm_get_type_string(int t)
{
    switch(t) {
        case GMTYPE_GRAPH: return "Graph";
        case GMTYPE_BYTE: return "Byte";
        case GMTYPE_SHORT: return "Short";
        case GMTYPE_INT: return "Int";
        case GMTYPE_LONG: return "Long";
        case GMTYPE_FLOAT: return "Float";
        case GMTYPE_DOUBLE: return "Double";
        case GMTYPE_NODEPROP: return "NP";
        case GMTYPE_EDGEPROP:return "EP";
        case GMTYPE_NODE: return "Node";
        case GMTYPE_EDGE:return "Edge";
        case GMTYPE_NODEITER_ALL: return "Node::I(ALL)";
        case GMTYPE_NODEITER_NBRS: return "Node::I(NBR)";
        case GMTYPE_NODEITER_IN_NBRS: return "Node::I(IN_NBR)";
        case GMTYPE_NODEITER_BFS:     return "Node::I(BFS)";
        case GMTYPE_NODEITER_UP_NBRS:   return "Node::I(+1)";
        case GMTYPE_NODEITER_DOWN_NBRS:   return "Node::I(-1)";
        case GMTYPE_EDGEITER_ALL:return "EdgeI";
        case GMTYPE_EDGEITER_NBRS:return "EdgeI";
        case GMTYPE_BOOL: return "Bool";
        case GMTYPE_NSET: return "Node_Set";
        case GMTYPE_NORDER: return "Node_Order";
        case GMTYPE_NSEQ: return "Node_Sequence";
        case GMTYPE_ESET: return "Edge_Set";
        case GMTYPE_EORDER: return "Edge_Order";
        case GMTYPE_ESEQ: return "Edge_Sequence";
        case GMTYPE_INF:
        case GMTYPE_INF_INT:
        case GMTYPE_INF_LONG:
        case GMTYPE_INF_DOUBLE:
        case GMTYPE_INF_FLOAT: return "INF";
        case GMTYPE_NODEITER_SET:return "N_S::I";
        case GMTYPE_NODEITER_SEQ:return "N_Q::I";
        case GMTYPE_NODEITER_ORDER:return "N_O::I";
        case GMTYPE_EDGEITER_SET:return "E_S::I";
        case GMTYPE_EDGEITER_SEQ:return "E_Q::I";
        case GMTYPE_EDGEITER_ORDER:return "E_O::I";
        case GMTYPE_ITER_ANY:return "Collection::I";
        case GMTYPE_VOID: return "Void";
                         
        default: /*printf("%d\n",t); assert(false);*/return "Unknown";
    }
}
const char* gm_get_iter_type_string(int t)
{
    switch(t) {
    case GMTYPE_NODEITER_ALL: return "Nodes";
    case GMTYPE_NODEITER_NBRS: return "Nbrs";
    case GMTYPE_EDGEITER_ALL:return "Edges";
    case GMTYPE_EDGEITER_NBRS:return "Nbr_Edges";
    case GMTYPE_NODEITER_UP_NBRS:return "UpNbrs";
    case GMTYPE_NODEITER_DOWN_NBRS:return "DownNbrs";
    case GMTYPE_NODEITER_IN_NBRS:return "InNbrs";
    case GMTYPE_NODEITER_COMMON_NBRS:return "CommonNbrs";
                                 

    case GMTYPE_NODEITER_SET:return "Items";
    case GMTYPE_NODEITER_SEQ:return "Items";
    case GMTYPE_NODEITER_ORDER:return "Items";
    case GMTYPE_ITER_ANY:return "Items";



    default: assert(false); return "Unknown";
    }
}


const char* gm_get_op_string (int op_type) {
    const char* opstr = (op_type == GMOP_MULT)? "*" : 
                        (op_type == GMOP_DIV)? "/" : 
                        (op_type == GMOP_SUB)? "-" : 
                        (op_type == GMOP_MOD)? "%" : 
                        (op_type == GMOP_ADD)? "+" : 
                        (op_type == GMOP_NEG)? "-" : 
                        (op_type == GMOP_AND)? "&&" : 
                        (op_type == GMOP_OR)? "||" : 
                        (op_type == GMOP_NOT)? "!" : 
                        (op_type == GMOP_EQ)? "==" : 
                        (op_type == GMOP_NEQ)? "!=" : 
                        (op_type == GMOP_GT)? ">" : 
                        (op_type == GMOP_LT)? "<" : 
                        (op_type == GMOP_GE)? ">=" : 
                        (op_type == GMOP_LE)? "<=" : 
                        (op_type == GMOP_ABS)? "|" : 
                        (op_type == GMOP_TYPEC)? "(type_conversion)" : 
                        "??";
    return opstr;
}
const char* gm_get_reduce_string(int rop_type) {
    const char* opstr = (rop_type == GMREDUCE_PLUS)? "+=" : 
                        (rop_type == GMREDUCE_MULT)? "*=" : 
                        (rop_type == GMREDUCE_MIN)? "min=" : 
                        (rop_type == GMREDUCE_MAX)? "max=" : 
                        (rop_type == GMREDUCE_AND)? "&=" : 
                        (rop_type == GMREDUCE_OR)?  "|=" : 
                        (rop_type == GMREDUCE_DEFER)? "<=" : 
                        "??";
    return opstr;
}
const char* gm_get_reduce_expr_string(int rop_type) {
    const char* opstr = (rop_type == GMREDUCE_PLUS)? "Sum" : 
                        (rop_type == GMREDUCE_MULT)? "Product" : 
                        (rop_type == GMREDUCE_MIN)? "Min" : 
                        (rop_type == GMREDUCE_MAX)? "Max" : 
                        (rop_type == GMREDUCE_AND)? "All" : 
                        (rop_type == GMREDUCE_OR)?  "Exist" : 
                        "??";
    return opstr;
}


int gm_get_op_pred(int op_type) {
    return GM_OPPRED_LEVEL[op_type];
}

bool gm_need_paranthesis(int this_op, int up_op, bool is_right)
{
    if (up_op == GMOP_TER)
    {
        // for clarity I prefer adding ()s, except chained ternary-ops.
        // example (A+B>C) ? (D+1) : (A+C>D) ? (E+1) : (F+1)
        if ((this_op == GMOP_TER) && is_right) return false;
        else return true;
    }
    else
    {
        if (gm_get_op_pred(this_op) > 
            gm_get_op_pred(up_op))
            return true;

        else if (gm_get_op_pred(this_op) == 
                  gm_get_op_pred(up_op) && is_right)
            return true;

        else 
            return false;
    }
}


void gm_path_parser::parsePath(const char* fullpath)
{
    int sz = strlen(fullpath);
    if (_sz < (sz+1)) {
        delete_arrays();
        create_arrays(sz+1);
    }
    char* _temp = new char[sz+1];
    strcpy(_temp, fullpath);

    //----------------------------------------
    // parse path name
    //----------------------------------------
    int count = 0;
    char* p_last = NULL;
    char* p = strtok(_temp, "/"); // ignore windows for now
    while (p!=NULL) {
        p_last = p;
        p = strtok(NULL, "/"); // not thread safe you know.
    }
    int index0;
    if (p_last == NULL) {
        strcpy(_path,"");
        p_last = _temp;
        index0 = 0;
    } else {

        index0 = ((int) (p_last - _temp)) ;
        strncpy(_path, fullpath, index0 );
        _path[index0] = '\0';
    }
    const char *h_begin = &fullpath[index0];

    //----------------------------------------
    // parse ext name
    //----------------------------------------
    char* p_begin = p_last;
    p = strtok(p_begin, "."); 
    p_last = NULL;
    while (p!=NULL) {
        p_last = p;
        p = strtok(NULL, "."); // strtok is not thread safe, you know.
    }
    int index1;
    if (p_begin == p_last) {
        strcpy(_ext,"");
        strcpy(_fname, h_begin);
    } else {
        strcpy(_ext, p_last);

        index1 = ((int) (p_last - p_begin)); 
        if (index1 > 1)
            strncpy(_fname, h_begin, index1 -1);
        _fname[index1-1] = '\0';
    }

    if (gm_is_same_string(_path, "")) sprintf(_path,".");

}

char* gm_strdup(const char *c) {
    assert(c!=NULL);
    char* z = new char[strlen(c)+1];
    strcpy(z,c);
    return z;
}

int gm_get_iter_type_from_set_type(int set_type)
{
    switch(set_type)
    {
        case GMTYPE_NSET: return GMTYPE_NODEITER_SET;
        case GMTYPE_NSEQ: return GMTYPE_NODEITER_SEQ;
        case GMTYPE_NORDER: return GMTYPE_NODEITER_ORDER;
        case GMTYPE_ESET: return GMTYPE_NODEITER_SET;
        case GMTYPE_ESEQ: return GMTYPE_NODEITER_SEQ;
        case GMTYPE_EORDER: return GMTYPE_NODEITER_ORDER;
        default: assert(false);
    }
}

