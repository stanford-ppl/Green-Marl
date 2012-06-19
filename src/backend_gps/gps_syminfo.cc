#include "gps_syminfo.h"

void gps_syminfo::add_usage_in_BB(int bb_no, int usage, int context, int reduce_type) {
    // if not already in
    for (int i = 0; i < (int) used_BB.size(); i++) {
        if ((used_BB[i] == bb_no) && (used_type[i] == usage) && (used_context[i] == context)) return;
    }

    if (last_BB == -1)
        last_BB = bb_no;

    else if (last_BB != bb_no) _used_in_multiple_BB = true;

    used_BB.push_back(bb_no);
    used_type.push_back(usage);
    used_context.push_back(context);

    if (usage == GPS_SYM_USED_AS_RHS) {
        used_as_rhs = true;
    } else if (usage == GPS_SYM_USED_AS_LHS) {
        used_as_lhs = true;
    } else if (usage == GPS_SYM_USED_AS_REDUCE) {
        used_as_reduce = true;
        reduce_op_type = reduce_type;
    } else {
        assert(false);
    }

    switch (context) {
        case GPS_CONTEXT_MASTER:
            used_in_master = true;
            break;
        case GPS_CONTEXT_VERTEX:
            used_in_vertex = true;
            break;
            //case GPS_CONTEXT_SENDER: used_in_sender = true; break;
        case GPS_CONTEXT_RECEIVER:
            used_in_receiver = true;
            break;
        default:
            assert(false);
            break;
    }

    // reduce_op_type = reduce_type; //TODO why was this here? only set if symbol is used as reduce (see 13 lines back)
    // reduce_op_type is initialized to GMREDUCE_NULL and can only be overwritten by one reduce operation, this is checked
    // in gm_reduce_error_check.cc
}
