
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

//------------------------------------------------------
// hoist up target-type sentence as far as possible 
// (in the same sent block)
// target-type should not be a var-decl
//------------------------------------------------------
class gm_hoist_normal_sent_t: public gm_apply
{
public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_SENTBLOCK)
            return true;

        ast_sentblock* sb = (ast_sentblock*) s;

        std::list<ast_sent*> sents = sb->get_sents();  // make a copy of sentence list (right?)
        std::list<ast_sent*>::iterator i_out;
        for(i_out = sents.begin(); i_out != sents.end(); i_out ++) 
        {
            //--------------------------------------
            // find target assign sentence
            //--------------------------------------
            ast_sent* target = *i_out;
            if (!check_target((ast_assign*)target))
                continue;

            std::list<ast_sent*>& sents2 = sb->get_sents(); // most up-to-date list
            std::list<ast_sent*> stack;
            ast_sent* top_position = NULL;

            //--------------------------------------
            // now find the possible upmost position
            //--------------------------------------
            std::list<ast_sent*>::iterator i_in;
            for(i_in = sents2.begin();i_in != sents2.end(); i_in++)
            {
                ast_sent* S = *i_in;
                if (S == target) break; 
                if (stack.size() == 0) {
                    // does not need to add into the queue
                    if (check_trivial_pred(S)) {
                        top_position = S;
                        continue;
                    }
                }
                stack.push_front(S);
            }

            //------------------------------------------------------------------------
            // stack contains list of sentences that have to check
            // top_position points the end of sentences that should precede target.
            //
            // Check dependency
            //------------------------------------------------------------------------
            while(stack.size() > 0) {
                ast_sent *S = stack.front();
                stack.pop_front();
                if (gm_has_dependency(S, target)) {
                    top_position = S;
                    break;
                }
            }

            gm_ripoff_sent( target, GM_NOFIX_SYMTAB);
            if (top_position == NULL) {// move it to the beginning 
                gm_insert_sent_begin_of_sb(sb, target, GM_NOFIX_SYMTAB);
            }
            else {// move to the top
                gm_add_sent_after(top_position, target, GM_NOFIX_SYMTAB);
            }
        }
        return true;
    }

protected:
    virtual bool check_target(ast_sent *s)=0; // if s is a sentence, you want to hoist up
    virtual bool check_trivial_pred(ast_sent* p)=0; // if p is a trivial precedessor to your target
};

