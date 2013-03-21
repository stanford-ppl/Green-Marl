#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
//-----------------------------------------------------------------
// Check random access
//-----------------------------------------------------------------

class gps_check_random_read_t : public gm_apply
{
public:
    gps_check_random_read_t() {
        set_for_expr(true);
        _error = false;
    }

    bool is_error() {
        return _error;
    }

    bool apply(ast_expr* f) {
        // random read always happens by field or builtin
        if ((f->get_opclass() == GMEXPR_FIELD) || (f->get_opclass() == GMEXPR_BUILTIN)) {
            if (f->find_info_int(GPS_INT_EXPR_SCOPE) == GPS_NEW_SCOPE_RANDOM) {
                gm_symtab_entry *driver =
                        (f->get_opclass() == GMEXPR_FIELD) ? f->get_field()->get_first()->getSymInfo() : 
                            ((ast_expr_builtin*) f)->get_driver()->getSymInfo();

                if (driver->getType()->is_graph()) return true;

                // Random Read
                if ((f->get_opclass() == GMEXPR_FIELD))
                    printf("%s.%s\n", f->get_field()->get_first()->get_genname(), f->get_field()->get_second()->get_genname());
                else
                    printf("%s->..()\n", ((ast_expr_builtin*) f)->get_driver()->get_genname());

                gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_READ, f->get_line(), f->get_col(), "");
                _error = true;
            }
        }
        return true;
    }
private:
    bool _error;

};

void gm_gps_new_check_random_read::process(ast_procdef* proc) {
    gps_check_random_read_t T;
    proc->traverse_pre(&T);

    set_okay(!T.is_error());
}
