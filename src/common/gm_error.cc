#include "gm_error.h"

static char* curr_file=NULL;
static char* curr_proc=NULL;
static bool need_print = true;
static bool parse_error = false;

void GM_set_parse_error(bool b) {parse_error = b;}
bool GM_is_parse_error() {return parse_error;}

void gm_set_current_filename(char* fname)  {curr_file = fname;}
char* gm_get_current_filename() {return curr_file;}
void gm_set_curr_procname(char* pname) {curr_proc=pname; need_print=true;}

void gm_print_error_header()
{
    if (need_print == false) return;
    if (curr_file!=NULL) printf("%s:", curr_file);
    if (curr_proc == NULL) printf("At top level:\n");
    else printf("In procedure %s:\n", curr_proc);
    need_print = false;
}

void gm_type_error(int errno, ast_id* id, const char* str1, const char* str2)
{
    gm_print_error_header();
    if (curr_file!=NULL) printf("%s:", curr_file);
    printf("%d: %d: error: ", id->get_line(), id->get_col());
    switch(errno) {
        case GM_ERROR_INVALID_ITERATOR_FOR_RARROW:
            printf("%s cannot be used in Edge() syntax.\n", id->get_orgname()); break;
        case GM_ERROR_INVALID_GROUP_DRIVER:
            printf("%s cannot be used outside group assignment.\n", id->get_orgname()); break;
        case GM_ERROR_UNDEFINED:
            printf("%s is not defined.\n", id->get_orgname()); break;
        case GM_ERROR_UNDEFINED_FIELD:
            printf("Property name %s is not defined.\n", id->get_orgname()); break;
        //case GM_ERROR_MULTIPLE_TARGET:
        //    printf("%s is bound to multiple graphs.\n", id->get_orgname()); break;
        case GM_ERROR_WRONG_PROPERTY:
            printf("%s is not defined as a %s\n", id->get_orgname(), str1); break;
        case GM_ERROR_NONGRAPH_FIELD:
            printf("%s is neither node, edge, nor graph.\n", id->get_orgname()); break;
        case GM_ERROR_READONLY:
            printf("It is not allowed to write into %s \n", id->get_orgname()); break;
        case GM_ERROR_NEED_ORDER:
            printf("Need ordered set for reverse-order iteration (%s is not)\n", id->get_orgname()); break;
        //case GM_ERROR_INVALID_ITERATOR:
        //    printf("Iterator not valid for the source %s \n", id->get_orgname()); break;

        case GM_ERROR_NEED_NODE_ITERATION:
            printf("Iteration should start from a node instead of %s\n", id->get_orgname()); break;
        case GM_ERROR_NEED_BFS_ITERATION:
            printf("Leveled Iteration should start from a bfs iterator instead of %s \n", id->get_orgname()); break;

        case GM_ERROR_NEED_ITERATOR:
            printf("%s is not an iterator name\n", id->get_orgname()); break;

        case GM_ERROR_UNKNOWN:
        default:
            assert(false);
            printf("Unknown error 1\n"); break;
    }
}
void gm_type_error(int errno, int l, int c, const char* str1, const char* str2, const char* str3)
{
    gm_print_error_header();

    if (curr_file!=NULL) printf("%s:", curr_file);
    printf("%d: %d: error: ", l, c);
    switch(errno) {
        case GM_ERROR_PAR_RETURN:
            printf("return inside parallel consistency\n");
            break;
        case GM_ERROR_GROUP_REDUCTION:
            printf("Group assignment cannot be a reduction\n");
            break;
        case GM_ERROR_INVALID_ARGMAX_COUNT:
            printf("Number of lhs does not match to number of rhs: %s\n", str1);
            break;
        case GM_ERROR_INCONSISTENT_ARGMAX:
            printf("LHS list of argmiax assignment is not consistent; They should be all scalar or have same driver\n");
            break;


        case GM_ERROR_NESTED_BFS:
            printf("Currently, nested bfs/dfs is not supported\n");
            break;
        case GM_ERROR_NEED_PRIMITIVE:
            printf("need primitive type.\n");
            break;
        case GM_ERROR_INVALID_OUTPUT_TYPE:
            printf("Invalid type for an output parameter, or return.\n"); break;
        case GM_ERROR_INVALID_BUILTIN:
            printf("Invalid built-in:%s\n", str1);
            break;
        case GM_ERROR_INVALID_BUILTIN_ARG_COUNT:
            printf("Argument number mismatch for built-in:%s\n", str1);
            break;
        case GM_ERROR_INVALID_BUILTIN_ARG_TYPE:
            printf("type mismatch for built-in:%s, arg_no:%s \n", str1,str2);
            break;
        case GM_ERROR_OPERATOR_MISMATCH:
            printf("Operator %s applied to an unsupported type (%s)\n", str1, str2);break;
        case GM_ERROR_OPERATOR_MISMATCH2:
            printf("Operator %s cannot be applied to (%s, %s)\n", str1, str2, str3);break;
        case GM_ERROR_TYPE_CONVERSION:
            printf("Type conversion can be only applied to primitive types\n");break;
        case GM_ERROR_TYPE_CONVERSION_BOOL_NUM:
            printf("Type conversion cannot be  applied between numeric and boolean types\n");break;
        case GM_ERROR_TARGET_MISMATCH:
            printf("Assignment to different Graphs\n");break;
        case GM_ERROR_ASSIGN_TYPE_MISMATCH:
            printf("Typemismatch in Assignment. LHS:%s, RHS:%s \n", str1, str2 );break;
        case GM_ERROR_COMPARE_MISMATCH:
            printf("Typemismatch in Comparison. LHS:%s, RHS:%s \n", str1, str2 );break;
        case GM_ERROR_NEED_BOOLEAN:
            printf("Need boolean expression.\n"); break;
        case GM_ERROR_UNBOUND_REDUCE:
            printf("Cannot determine bound to Reduce(Defer) assignment\n"); break;

        case GM_ERROR_DOUBLE_BOUND_ITOR:
            printf("Reduce(Defer) Target already bound to a different iterator: %s\n", str1); break;
        case GM_ERROR_DOUBLE_BOUND_OP:
            printf("Reduce(Defer) Target already bound to a different operator: %s\n", str1); break;
        case GM_ERROR_GRAPH_REDUCE:
            printf("Can not do reduce (defer) assignment to graph variable :%s\n", str1); break;
        case GM_ERROR_GROUP_MISMATCH:
            printf("Group assignment error (assigning node_prop into edge_prop or vice versa)\n");break;
        case GM_ERROR_RETURN_FOR_VOID:
            printf("Cannot have return value for void procedure\n"); break;
        case GM_ERROR_NO_VOID_RETURN:
            printf("Need Return Value\n"); break;
        case GM_ERROR_RETURN_MISMATCH:
            printf("Return type mismatch: required(%s), found(%s)\n",str1,str2 ); break;

        case GM_ERROR_REQUIRE_BOOLEAN_REDUCE:
            printf("Boolean type required for reduction\n",str1,str2 ); break;
        case GM_ERROR_REQUIRE_NUMERIC_REDUCE:
            printf("Numeric type required for reduction\n",str1,str2 ); break;

        default:
            printf("Unknown error 2\n"); break;
    }
}

void gm_type_error(int errno, ast_id* id1, ast_id* id2)
{
    gm_print_error_header();
    if (curr_file!=NULL) printf("%s:", curr_file);
    printf("%d: %d: error: ", id1->get_line(),id1->get_col());
    switch(errno) {
        case GM_ERROR_NONGRAPH_TARGET:
            printf("%s is not a graph type object\n", id1->get_orgname()); break;
        case GM_ERROR_NONSET_TARGET:
            printf("%s is not a collection type object\n", id1->get_orgname()); break;
        case GM_ERROR_NONNODE_TARGET:
            printf("%s is not a node-compatible type object\n", id1->get_orgname()); break;
        case GM_ERROR_TARGET_GRAPH_MISMATCH:
            printf("%s and %s are not bound to the same graph\n", 
                    id1->get_orgname(), id2->get_orgname()); break;
        case GM_ERROR_UNDEFINED_FIELD_GRAPH:
            printf("Property %s is not defined for graph %s.\n", id1->get_orgname(), id2->get_orgname()); break;
        case GM_ERROR_DUPLICATE:
            printf("%s is defined more than one time. (First defined in line %d : %d)\n",
                    id1->get_orgname(), id2->get_line(), id2->get_col());
            break;
        case GM_ERROR_TARGET_MISMATCH:
            printf("Target Graphs mismatches %s, %s.\n", id1->get_orgname(), id2->get_orgname());break;
        case GM_ERROR_UNKNOWN:
        default:
            printf("Unknown error 3\n"); break;
    }
}

void  gm_conf_error(int errno, gm_symtab_entry* target, ast_id* ev1, ast_id* ev2, bool is_warning)
{
    gm_print_error_header();
    if (curr_file!=NULL) printf("%s:", curr_file);

    if (is_warning)
        printf("%d: %d: warn: ", ev1->get_line(), ev1->get_col());
    else
        printf("%d: %d: error: ", ev1->get_line(), ev1->get_col());

    ast_id* target_id = target->getId();
    char* name = target_id->get_orgname();

    switch(errno) {
        case GM_ERROR_READ_WRITE_CONFLICT:
            printf("Property %s may have read-write conflict: read at line:%d, write at line:%d\n", 
                   name,ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_WRITE_WRITE_CONFLICT:
            printf("Property %s may have write-write conflict: write at line:%d, write at line:%d\n", 
                   name,ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_READ_REDUCE_CONFLICT:
            printf("Property %s may have read-reduce conflict: read at line:%d, reduce at line:%d\n", 
                   name,ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_WRITE_REDUCE_CONFLICT:
            printf("Property %s may have write-reduce conflict: write at line:%d, reduce at line:%d\n", 
                   name,ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_READ_MUTATE_CONFLICT:
	    printf("Property %s may have read-mutate conflict: read at line:%d, mutate at line:%d\n", name, ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_WRITE_MUTATE_CONFLICT:
	    printf("Property %s may have write-mutate conflict: write at line:%d, mutate at line:%d\n", name, ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_MUTATE_MUTATE_CONFLICT:
	    printf("Property %s may have mutate-mutate conflict: mutate at line:%d, mutate at line:%d\n", name, ev1->get_line(), ev2->get_line());
            break;
        case GM_ERROR_UNKNOWN:
        default:
            assert(false);
            printf("Unknown error 3\n"); break;
    }
}

// todo: should be differend error routines for different back-ends
void gm_backend_error(int errno, const char* str1, const char* str2)
{
    gm_print_error_header();
    if (curr_file!=NULL) printf("%s:", curr_file);
    switch(errno) {
        case GM_ERROR_FILEWRITE_ERROR:
            printf("Error: cannot open file %s for write\n", str1);
            break;
        case GM_ERROR_GPS_NUM_PROCS:
            printf("Error: There must be one and only one procedure\n");
            break;
        case GM_ERROR_GPS_PROC_NAME:
            printf("Error: The name of the procedure(%s) must match with the name of file (%s)\n", str1, str2);
            break;
        default:
            assert(false);
            printf("Unknown backend error\n"); break;
    }
}
void gm_backend_error(int errno, int l, int c, const char* str1)
{
    gm_print_error_header();
    if (curr_file!=NULL) printf("%s:", curr_file);
    printf("%d: %d: error: ", l, c);
    switch(errno) {
        case GM_ERROR_GPS_EDGE_SEND_VERSIONS:
            printf("Communicating multiple versions of edge value in one message: %s\n", str1);
            break;
        case GM_ERROR_GPS_EDGE_WRITE_RHS:
            printf("Unacceptible driver for rhs in edge property writing: %s\n", str1);
            break;
        case GM_ERROR_GPS_EDGE_WRITE_CONDITIONAL:
            printf("Write to edge property should not be conditional.\n");
            break;
        case GM_ERROR_GPS_EDGE_INIT:
            printf("Edge variable should be initialize only to out_nbr.ToEdge().\n");
            break;
        case GM_ERROR_GPS_EDGE_WRITE_RANDOM:
            printf("Random writing of edge values is not supported.\n");
            break;
        case GM_ERROR_GPS_EDGE_READ_RANDOM:
            printf("Random reading of edge values is not supported.\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_WRITE_CONDITIONAL:
            printf("Random node write cannot happen inside a conditional block\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_WRITE_USE_SCOPE:
            printf("Random node write should happen in the outer loop\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_WRITE_DEF_SCOPE:
            printf("Random node write should destinated to a out-scoped node variable\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_WRITE_REDEF:
            printf("Random node destination has been re-defined\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_READ:
            printf("Random node read is not supported\n");
            break;
        case GM_ERROR_GPS_RANDOM_NODE_WRITE:
            printf("Random node write is not supported\n");
            break;
        case GM_ERROR_GPS_UNSUPPORTED_OP:
            printf("%s operation is not supported\n", str1);
            break;
        case GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER:
            printf("Only node-wide parallel iteration is supported in master mode\n");
            break;
        case GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX:
            printf("Only neighbor-wide iteration is supported in vertex mode\n");
            break;
        case GM_ERROR_GPS_NEED_PARALLEL:
            printf("Only parallel iteration is avaiable\n");
            break;
        case GM_ERROR_GPS_NBR_LOOP_INSIDE_WHILE:
            printf("Inner loop cannot be inside extra loop or if statement.\n");
            break;
        case GM_ERROR_GPS_UNSUPPORTED_COLLECTION:
            printf("%s is an unsupported collection type\n", str1);
            break;
        case GM_ERROR_GPS_NO_GRAPH:
            printf("There should be at least one graph defined at the entry function\n");
            break;
        case GM_ERROR_GPS_MULTIPLE_GRAPH:
            printf("There should only one graph defined at the entry function\n");
            break;
        case GM_ERROR_GPS_PULL_SYNTAX:
            printf("Syntax is based in 'Pulling' and cannot be transformed into 'Pushing'\n");
            break;
        case GM_ERROR_GPS_NBR_LOOP_TOO_DEEP:
            printf("Loop depth too deep\n");
            break;
        case GM_ERROR_GPS_MULTIPLE_INNER_LOOP:
            printf("There can be only one inner loop\n");
            break;
        case GM_ERROR_CPP_UNSUPPORTED_SYNTAX:
            printf("The compiler does not support nested %s\n",str1);
            break;
    }
}
