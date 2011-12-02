#include "gps_syminfo.h"

void gps_syminfo::add_usage_in_BB(int bb_no, int usage, bool is_vertex_BB)
{
    // if not already in
    for(int i=0;i<used_BB.size();i++) {
        if ((used_BB[i] == bb_no) && (used_type[i] == usage))
            return;
    }

    if (last_BB == -1) 
        last_BB = bb_no;

    else if (last_BB != bb_no)
        _used_in_multiple_BB = true;

    used_BB.push_back(bb_no);
    used_type.push_back(usage);

    if (usage == GPS_SYM_USED_AS_RHS) 
        used_as_rhs = true;
    else if (usage == GPS_SYM_USED_AS_LHS)
        used_as_lhs = true;
    else if (usage == GPS_SYM_USED_AS_REDUCE)
        used_as_reduce = true;
    else
        assert(false);

    if (is_vertex_BB)
        used_in_vertex = true;
    else
        used_in_master = true;
}
