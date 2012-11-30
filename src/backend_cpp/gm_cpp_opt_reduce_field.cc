#include <stdio.h>
#include <map>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_argopts.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_rw_analysis.h"

class opt_field_reduction_t: public gm_apply
{
private:
    std::list<ast_sent*> targets;
    void apply_transform(ast_foreach* fe);

public:
    // choose targets
    virtual bool apply(ast_sent* sent) {

        // find foreach-loops
        if (sent->get_nodetype() != AST_FOREACH) return true;

        ast_foreach* fe = (ast_foreach*) sent;
        gm_rwinfo_map& B = gm_get_bound_set_info(fe)->bound_set;
        if (B.size() == 0) return true;

        assert(fe->is_parallel());

        gm_rwinfo_map::iterator I;
        for (I = B.begin(); I != B.end(); I++) {
            gm_symtab_entry* e = I->first;
            if (e->getType()->is_property()) {
                targets.push_back(fe);
                break;
            } else {
                continue;
            }
        }

        return true;
    }

    // transform each element in targets
    void transform_targets() {
        std::list<ast_sent*>::iterator I;
        for (I = targets.begin(); I != targets.end(); I++) {
            ast_sent* s = *I;
            assert(s->get_nodetype() == AST_FOREACH);
            ast_foreach* fe = (ast_foreach*) s;
            apply_transform(fe);
        }
    }

    bool has_targets() {
        return targets.size() > 0;
    }

};
