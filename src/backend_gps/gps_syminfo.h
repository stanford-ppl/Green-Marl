
#ifndef GPS_SYMINFO
#define GPS_SYMINFO

#include "gm_typecheck.h"

enum {
    GPS_SYM_USED_AS_RHS,
    GPS_SYM_USED_AS_LHS,
    GPS_SYM_USED_AS_REDUCE,
};

class gps_syminfo : public ast_extra_info
{
    public:
        gps_syminfo(bool sc): _scalar(sc) {_used_in_multiple_BB = false; last_BB = -1; _is_arg = false;}
        virtual ~gps_syminfo() {}

        void set_is_argument(bool b) {_is_arg = b;}
        bool is_argument() {return _is_arg;}
        bool is_scalar() {return _scalar;}
        void add_usage_in_BB(int bb_no, int usage);

        bool is_used_in_multiple_BB() {return _used_in_multiple_BB;}

    private:
        bool _used_in_multiple_BB;
        int last_BB;
        bool _scalar;
        bool _is_arg;
        std::vector<int> used_BB;
        std::vector<int> used_type;
};

#endif
