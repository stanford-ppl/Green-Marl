
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_frontend_api.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

struct gm_op_rule_t {
    int left_category;
    int right_category;
    int result_category;
};

static std::map<int, std::list<gm_op_rule_t> > gm_op_rules;


enum gm_category_enum_t {
    INT,
    NUMERIC ,
    NUMERIC_INF, 
    NUMERIC_LARGER,
    BOOLEAN ,
    NODE ,
    EDGE,
    STRICT_SAME
};
       
static void gm_add_op_rule(int op_type, int l, int r, int result)
{
    std::list<gm_op_rule_t>& rules = gm_op_rules[op_type];

    gm_op_rule_t T;
    T.left_category = l;
    T.right_category = r;
    T.result_category = result;

    rules.push_back(T);
}

void gm_frontend::init_op_type_rules()
{
    gm_add_op_rule(GMOP_MULT, NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_DIV,  NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_MOD, INT, INT, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_MAX, NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_MIN, NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_ADD, NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_SUB, NUMERIC, NUMERIC, NUMERIC_LARGER);
    gm_add_op_rule(GMOP_AND, BOOLEAN, BOOLEAN, BOOLEAN);
    gm_add_op_rule(GMOP_OR,  BOOLEAN, BOOLEAN, BOOLEAN);
    
    gm_add_op_rule(GMOP_EQ,  NODE,  NODE, BOOLEAN);
    gm_add_op_rule(GMOP_EQ,  EDGE,  EDGE, BOOLEAN);
    gm_add_op_rule(GMOP_EQ,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);
    gm_add_op_rule(GMOP_EQ,  STRICT_SAME,  STRICT_SAME, BOOLEAN);

    gm_add_op_rule(GMOP_NEQ,  NODE,  NODE, BOOLEAN);
    gm_add_op_rule(GMOP_NEQ,  EDGE,  EDGE, BOOLEAN);
    gm_add_op_rule(GMOP_NEQ,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);
    gm_add_op_rule(GMOP_NEQ,  STRICT_SAME,  STRICT_SAME, BOOLEAN);

    gm_add_op_rule(GMOP_LE,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);
    gm_add_op_rule(GMOP_GE,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);
    gm_add_op_rule(GMOP_LT,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);
    gm_add_op_rule(GMOP_GT,  NUMERIC_INF, NUMERIC_INF, BOOLEAN);

    // ternary operator (left and right)
    gm_add_op_rule(GMOP_TER,  STRICT_SAME,  STRICT_SAME,  STRICT_SAME);
    gm_add_op_rule(GMOP_TER,  NODE,        NODE,        NODE);
    gm_add_op_rule(GMOP_TER,  EDGE,        EDGE,        EDGE);
    gm_add_op_rule(GMOP_TER,  NUMERIC_INF, NUMERIC_INF, NUMERIC_LARGER);

}

static bool is_appliable(int category, int type)
{
    switch(category)
    {
        case INT:     return gm_is_int_type(type) || gm_is_long_type(type);
        case NUMERIC:     return gm_is_numeric_type(type);
        case NUMERIC_INF: return gm_is_numeric_type(type) || gm_is_inf_type(type);
        case BOOLEAN: return gm_is_boolean_type(type);
        case NODE: return gm_is_node_compatible_type(type);
        case EDGE: return gm_is_edge_compatible_type(type);
        case STRICT_SAME: return true;
        default: assert(false);
     }
}

static int get_result_type(int category, int l_type, int r_type)
{
    switch(category)
    {
        case BOOLEAN: return GMTYPE_BOOL; 
        case NODE:    return GMTYPE_NODE;
        case EDGE:    return GMTYPE_EDGE;
        case NUMERIC_LARGER: return std::max(l_type, r_type);
        case STRICT_SAME: return l_type; // l_type == r_type
        deafult: assert(false);
    }
}

// returns is okay
bool gm_check_biop_rule(int op_type, int l_type, int r_type, int& result_type)
{
    result_type = GMTYPE_UNKNOWN;
    if (gm_is_unknown_type(l_type) || gm_is_unknown_type(r_type)) 
        return false;

    // find and apply rules
    std::list<gm_op_rule_t>& rules = gm_op_rules[op_type];
    std::list<gm_op_rule_t>::iterator I;
    for(I=rules.begin();I!=rules.end(); I++) {
        gm_op_rule_t& T = *I;
        if (!is_appliable(T.left_category, l_type)) continue;
        if (!is_appliable(T.right_category, r_type)) continue;

        if (T.left_category == STRICT_SAME) {
            assert (T.right_category == STRICT_SAME); 
            if (l_type != r_type) continue;
        }
        result_type =  get_result_type(T.result_category, l_type, r_type);
        return true;
    }

    return false; // cannot find applicable opeator
}

