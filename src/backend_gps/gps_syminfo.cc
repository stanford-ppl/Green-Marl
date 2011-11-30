#include "gps_syminfo.h"

void gps_syminfo::add_usage_in_BB(int bb_no, int usage)
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
}
