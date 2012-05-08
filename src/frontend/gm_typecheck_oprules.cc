
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_frontend_api.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//------------------------------------------------------------------------
// CLASS NUMERIC OP

//------------------------------------------------------------------------
enum {
  INT_OP,        // %
  NUMERIC_OP,    // +,-,*,/, Max, Min
  BOOL_OP,    // And, Or
  COMP_OP,       // <,<=,>,>=
  EQ_OP,         // == !=
  TER_OP,        // ? t1 : t2
  ASSIGN_OP,        // =
};

enum {
  T_INT,
  T_BOOL,
  T_NUMERIC,
  T_NUMERIC_INF,    // NUMERIC + INF
  T_COMPATIBLE,
};

enum {
    RESULT_COERCION,        // coercion
    RESULT_LEFT,
    RESULT_BOOL
};

enum {
    COERCION_ALL,          // coercion
    COERCION_RIGHT,        // coercion
    COERCION_NO,           // no-coercion
};

struct gm_type_rule {
gm_type_rule(int O,int T1, int T2, int R, int C) : opclass(O), type1(T1), type2(T2), result_type(R), coercion_rule(C) {}
    int opclass;
    int type1;
    int type2;
    int result_type;
    int coercion_rule;
};

std::vector<gm_type_rule> GM_TYPE_RULES;
#define NEW_RULE(O, T1,T2, TR, C) \
{ gm_type_rule R(O, T1, T2, TR, C); GM_TYPE_RULES.push_back(R); }


static void init_op_rules()
{
    NEW_RULE(INT_OP,      T_INT,            T_INT,          RESULT_COERCION,    COERCION_ALL);
    NEW_RULE(NUMERIC_OP,  T_NUMERIC,        T_NUMERIC,      RESULT_COERCION,    COERCION_ALL);
    NEW_RULE(BOOL_OP,     T_BOOL,           T_BOOL,         RESULT_BOOL,        COERCION_NO);
    NEW_RULE(COMP_OP,      T_NUMERIC_INF,    T_NUMERIC_INF,  RESULT_BOOL,        COERCION_ALL);
    NEW_RULE(COMP_OP,      T_COMPATIBLE,    T_COMPATIBLE,    RESULT_BOOL,        COERCION_NO);

    NEW_RULE(EQ_OP,        T_NUMERIC_INF,    T_NUMERIC_INF,  RESULT_BOOL,        COERCION_ALL);
    NEW_RULE(EQ_OP,        T_COMPATIBLE,     T_COMPATIBLE,   RESULT_BOOL,        COERCION_NO);

    NEW_RULE(TER_OP,       T_COMPATIBLE,     T_COMPATIBLE,   RESULT_LEFT,        COERCION_NO);
    NEW_RULE(TER_OP,       T_NUMERIC_INF,    T_NUMERIC_INF,  RESULT_LEFT,        COERCION_ALL);

    NEW_RULE(ASSIGN_OP,    T_NUMERIC_INF,    T_NUMERIC_INF,  RESULT_LEFT,        COERCION_RIGHT);
    NEW_RULE(ASSIGN_OP,    T_COMPATIBLE,     T_COMPATIBLE,   RESULT_LEFT,        COERCION_RIGHT);
};
void gm_frontend::init_op_type_rules()
{
    init_op_rules();
}
void gm_frontend::init() {
    init_op_type_rules();
}


static inline bool in_typeclass(int tclass, int type)
{
    if (tclass == T_INT)            {return gm_is_long_type(type) || gm_is_int_type(type);}
    if (tclass == T_NUMERIC)        {return gm_is_numeric_type(type);}
    if (tclass == T_NUMERIC_INF)    {return gm_is_inf_type(type) || gm_is_numeric_type(type);}
    if (tclass == T_BOOL)           {return gm_is_boolean_type(type);}
    assert(false);
}

static inline bool in_opclass(int oclass, int op)
{
    if (oclass == INT_OP)       {return (op== GMOP_MOD);}
    if (oclass == NUMERIC_OP)   {return gm_is_numeric_op(op);}
    if (oclass == BOOL_OP)      {return gm_is_boolean_op(op);}
    if (oclass == COMP_OP)      {return gm_is_less_op(op);}
    if (oclass == EQ_OP)        {return gm_is_eq_op(op);}
    if (oclass == TER_OP)       {return gm_is_ternary_op(op);}
    if (oclass == ASSIGN_OP)    {return (op==GMOP_ASSIGN);}
    assert(false);
}

static inline bool is_applicable_rule(gm_type_rule& R, int op, int type1, int type2)
{
    if  (in_opclass(R.opclass, op)) {
        if (R.type1 == T_COMPATIBLE) 
            return gm_is_same_type(type1,type2) || 
                   gm_is_same_node_or_edge_compatible_type(type1, type2);
        else {
            bool b =  in_typeclass(R.type1, type1) && in_typeclass(R.type2, type2);
             
            return b;
        }
    }
    return false;
}


bool gm_is_t2_larger_than_t1(int t1, int t2)
{
    if ((t1 == GMTYPE_INT)   && (t2 == GMTYPE_LONG)) return true;
    if ((t1 == GMTYPE_FLOAT) && (t2 == GMTYPE_DOUBLE)) return true;
    return false;
}


// return false if coercion cannot be done.
// (lose of precision should be checked separately)
static inline void apply_coercion(int c_type, int t1, int t2, int& t1_new, int& t2_new, bool& t1_warn, bool& t2_warn)
{
    t1_new = t1;
    t2_new = t2;
    t1_warn = false;
    t2_warn = false;
    if (t1 == t2) return; 

    if (c_type == COERCION_NO) return ;

    // left or right can be converted
    if (c_type == COERCION_ALL) {
        if (gm_is_inf_type(t1)) {t1_new = t2; return;}
        if (gm_is_inf_type(t2)) {t2_new = t1; return;}

        // type-up. (i.e. INT -> LONG)
        if (gm_is_t2_larger_than_t1(t1,t2)) { t1_new = t2; return ; }
        if (gm_is_t2_larger_than_t1(t2,t1)) { t2_new = t1; return ; }

        // crossing boundary (i.e. <int/long> <-> <float/double>))))
        if (gm_is_float_type(t1)) {
            t2_new = t1;
            t2_warn = true;

        } else {
            assert(gm_is_float_type(t2));
            t1_new = t2;
            t1_warn = true;
        }

        return;
    }

    // only rhs is allowed to be coerced.
    if (c_type == COERCION_RIGHT) {
        if (gm_is_inf_type(t2)) {t2_new = t1; return;}

        t2_new = t1;

        if ((t1!= t2) && (!gm_is_t2_larger_than_t1(t2, t1))) {
            t2_warn = true;
        }
        return;
    }

    assert(false);
    return ;
}


bool gm_is_compatible_type(int op, int t1, int t2,
                            int& op_result_type, int& t1_new, int& t2_new,
                            bool& t1_warn, bool& t2_warn)
{
    // special case 
    if (gm_is_foreign_expr_type(t1) && gm_is_foreign_expr_type(t2)) 
    {
        op_result_type = t1;
        t1_warn = t2_warn = false;
        return true;
    }

    if (gm_is_foreign_expr_type(t1)) {t1 = t2;} // believe that foreign-expression is type-compatible
    if (gm_is_foreign_expr_type(t2)) {t2 = t1;} 

    for(int i =  0; i < (int)GM_TYPE_RULES.size(); i++) 
    {
        gm_type_rule& R = GM_TYPE_RULES[i];


        if (is_applicable_rule(R, op, t1, t2))
        {
            // apply coercion
            apply_coercion(R.coercion_rule, t1, t2, t1_new, t2_new, t1_warn, t2_warn);

            // get result type
            if (R.result_type == RESULT_BOOL) {
                op_result_type = GMTYPE_BOOL;
            }
            else {
                op_result_type = t1_new; // always LHS type after coercion
            }

            return true;
        }
    }

    return false;
}
