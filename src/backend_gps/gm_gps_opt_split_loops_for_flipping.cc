#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

// defined in opt_find_nested_foreach_loops.cc
extern void gm_gps_find_double_nested_loops(ast_node* p, std::map<ast_foreach*, ast_foreach*>& MAP);

//----------------------------------------------------
// N_P<Int>(G) _tmp_S;
// Foreach (n: G.Nodes) {
//   n._tmp_S = 0;
//   Foreach( t: n.InNbrs) {
//       n._tmp_S += t.A;
//   }
//   n.B = n._tmp_S * alpha + beta;
// }
// ===>  
// Foreach (n: G.Nodes) {
//   n._tmp_S = 0;
// }
// Foreach (n: G.Nodes) {
//   Foreach( t: n.InNbrs) {
//       n._tmp_S += t.A;
//   }
// }
// Foreach (n: G.Nodes) {
//   n.B = n._tmp_S * alpha + beta;
// }
//----------------------------------------------------
//


//---------------------------------------
//
// Foreach(n.) {
//    <optional if>   // no variables defined in between
//       Foreach(t) {
//         n.X = ...
//       }
// }
//---------------------------------------
static void filter_target_loops(
    std::map<ast_foreach*, ast_foreach*>& SRC,
    std::set<ast_foreach*>& SET)
{

    std::map<ast_foreach*, ast_foreach*>::iterator I;
    for(I=SRC.begin(); I!=SRC.end(); I++)
    {
        ast_foreach* in = I->first;
        ast_foreach* out = I->second;
        if (out == NULL) continue;

        gm_symtab_entry* out_iter = out->get_iterator()->getSymInfo();


        bool is_target = false;
        // check if inner loop requires flipping
        gm_rwinfo_map& WMAP = gm_get_write_set(in);
        gm_rwinfo_map::iterator M;
        for(M = WMAP.begin(); M!=WMAP.end(); M++) {
            gm_rwinfo_list* LIST = M->second;
            gm_rwinfo_list::iterator I;
            for(I=LIST->begin();I!=LIST->end(); I++) {
                gm_rwinfo* info = *I;
                if ((info->driver == NULL) && (info->access_range == GM_RANGE_RANDOM)) {
                    is_target = true;
                    continue;
                }
                else if (info->driver == NULL) continue;

                if (info->driver != out_iter) {is_target = false; break;}
                if (info->driver == out_iter) {is_target = true; continue;}
            }
        }

        printf(" loop for %s -> %c \n", in->get_iterator()->get_genname(), is_target?'Y':'N');
        if (!is_target) continue;

        bool meet_if = false;
        ast_node* current = in;
        // move up until meet the outer loop
        while(true) {
            ast_node* parent = current->get_parent();
            assert(parent!=NULL);
            if (parent->get_nodetype() == AST_IF) {
                if (meet_if) {is_target = false; break;}
                else if (((ast_if*)parent)->get_else() == NULL) {meet_if = true;}
                else { is_target = false; break;}
            }
            else if (parent->get_nodetype() == AST_SENTBLOCK) {
                // todo: this is a little bit to restrictive.
                // we can relax this by changing more scalars into node properties.
                ast_sentblock* sb = (ast_sentblock*) parent;
                if ((sb->get_symtab_field()->get_num_symbols() > 0) ||
                    (sb->get_symtab_var()->get_num_symbols() > 0)) {
                    /*
                    printf("CCC %d %d\n",
                sb->get_symtab_field()->get_num_symbols(), 
                sb->get_symtab_var()->get_num_symbols()) ;
                    std::set<gm_symtab_entry*>& E = sb->get_symtab_var()->get_entries();
                    std::set<gm_symtab_entry*>::iterator I;
                    for(I=E.begin(); I!=E.end(); I++) {
                        gm_symtab_entry*e = *I;
                        printf("%s\n", e->getId()->get_genname());
                    }
                    */
                    is_target = false;
                    break;
                }
            }
            else if (parent->get_nodetype() == AST_FOREACH) {
                ast_foreach* fe = (ast_foreach*) parent;
                if (fe == out) break;
                else {is_target = false; break;}
            }
            else {
                is_target = false;
                break;
            }
            current = parent;
        }
        
        printf(" loop for %s -> %c \n", in->get_iterator()->get_genname(), is_target?'Y':'N');
        if (!is_target) continue;
        SET.insert(in);
    }
}

//--------------------------------------------
// example>
//    FE(n:) 
//    {
//        A; 
//        {
//              B;
//              If() 
//              {
//                   C;
//                   Foreach(t:) {
//                   }
//                   D;
//              }
//              E;
//         }
//         F;
//    }
// =====>
//    FE(n:) 
//    {
//        A; 
//        {
//              B;
//              If() 
//              {
//                   C;
//    }   }     }
//
//    FE(n1:) 
//    {  {      If() {
//                   Foreach(t:) {
//                   }
//              }
//    }   }
//
//    FE(n2:) 
//    {  {      If() 
//              {
//                  D;
//              }
//              E;
//        }
//        F;
//    }   
//------------------------------------------        
static ast_node* reconstruct_old_new(std::list<ast_node*>& frame, 
        std::map<ast_sentblock*,  std::list<ast_sent*> >& siblings,
        bool is_old);

static void split_the_loop(ast_foreach* in)
{
    ast_foreach* out = NULL;

    printf("splitting loop for %s\n", in->get_iterator()->get_genname());

    //----------------------------------------------------------------
    // find the current frame structure
    //----------------------------------------------------------------
    std::list<ast_node*> frame;
    std::map<ast_sentblock*,  std::list<ast_sent*> > older_siblings;
    std::map<ast_sentblock*,  std::list<ast_sent*> > younger_siblings;

    ast_node* current = in;
    bool need_young = false;
    bool need_old = false;
    while (true) {
        ast_node* node  = current->get_parent(); assert(node != NULL);
        if (node->get_nodetype() == AST_IF) {
            frame.push_back(node);
            current = node;
            continue;
        }
        else if (node->get_nodetype() == AST_FOREACH) {
            out = (ast_foreach*) node;
            break;
        }
        else if (node->get_nodetype() == AST_SENTBLOCK) {
            ast_sentblock* sb = (ast_sentblock*) node;
            frame.push_back(sb);

            // add elder and younger brothers
            std::list<ast_sent*>& All = sb->get_sents();
            std::list<ast_sent*> OLD;
            std::list<ast_sent*> YOUNG; 
            std::list<ast_sent*>::iterator I;
            bool older = true;
            for(I=All.begin(); I!=All.end(); I++) {
                ast_sent* s = *I;
                if (s == current) {
                    older = false;
                    continue;
                }
                if (older) OLD.push_back(s);
                else YOUNG.push_back(s);
            }

            All.clear();
            All.push_back((ast_sent*)current);

            assert(older == false);
            older_siblings[sb] = OLD;
            younger_siblings[sb] = YOUNG;
            need_young = need_young || (YOUNG.size() > 0);
            need_old = need_old || (OLD.size() > 0);
            current = node;

            continue; // go up one level
        }
        else {
            assert(false);
        }
    }
    assert(out!=NULL);
    assert(gm_is_iteration_on_all_graph(out->get_iter_type()));
    gm_make_it_belong_to_sentblock(out);
    assert(out->get_parent()->get_nodetype() == AST_SENTBLOCK);

    //----------------------------------------------------------------
    // reconstruct program
    //----------------------------------------------------------------
    std::list<ast_node*>::iterator I;
    if (need_old) {
        ast_node* old = reconstruct_old_new(frame, older_siblings, true);
        assert(old!=NULL);

        ast_foreach* old_loop = gm_new_foreach_after_tc(
                out->get_iterator()->copy(false),
                out->get_source()->copy(true),
                (ast_sent*) old,
                out->get_iter_type());

        gm_add_sent_before(out, old_loop);

        // replace iterator id
        gm_replace_symbol_entry(out->get_iterator()->getSymInfo(), old_loop->get_iterator()->getSymInfo(), old);
    }

    if (need_young) {
        ast_node* old = reconstruct_old_new(frame, younger_siblings, false);
        assert(old!=NULL);
        ast_foreach* new_loop = gm_new_foreach_after_tc(
                out->get_iterator()->copy(false),
                out->get_source()->copy(true),
                (ast_sent*) old,
                out->get_iter_type());

        gm_add_sent_after(out, new_loop);

        // replace iterator id
        gm_replace_symbol_entry(out->get_iterator()->getSymInfo(), new_loop->get_iterator()->getSymInfo(), old);
    }
}

static ast_node* reconstruct_old_new(std::list<ast_node*>& frame, 
        std::map<ast_sentblock*,  std::list<ast_sent*> >& siblings,
        bool is_old)
{

    ast_node* last = NULL;
    // reconstruct hierarchy
    // inmost --> outmost
    std::list<ast_node*>::iterator I;
    for(I=frame.begin(); I!=frame.end(); I++) {
        if ((*I)->get_nodetype() == AST_IF) {
            if (last == NULL) continue; // can ignore this if loop
            ast_if* iff = (ast_if*)(*I);
            ast_if* new_if = ast_if::new_if(
                    iff->get_cond()->copy(true),
                    (ast_sent*) last,
                    NULL
                    );
            new_if->set_line(iff->get_line());
            new_if->set_col(iff->get_col());
            last = new_if;
        }
        else if ((*I)->get_nodetype() == AST_SENTBLOCK) {
            ast_sentblock* sb_org = (ast_sentblock*) (*I);
            ast_sentblock* sb = ast_sentblock::new_sentblock();
            std::list<ast_sent*>& SIB = siblings[sb_org];
            std::list<ast_sent*>::iterator J;
            sb->set_line(sb_org->get_line());
            sb->set_col(sb_org->get_col());

            if (is_old && (last!=NULL)) 
                sb->add_sent((ast_sent*)last);

            for(J=SIB.begin(); J!=SIB.end(); J++) {
                sb->add_sent(*J);
            }

            if (!is_old && (last!=NULL))
                sb->add_sent((ast_sent*)last);

            last = sb;
        }
    }

    return last;
}

void gm_gps_opt_split_loops_for_flipping::process(ast_procdef* p)
{
    //-------------------------------------
    // Find nested loops
    //-------------------------------------
    std::map<ast_foreach*, ast_foreach*> MAP;
    std::set<ast_foreach*> SET;
    gm_gps_find_double_nested_loops(p, MAP);

    //-------------------------------------
    // Find target inner loops
    //-------------------------------------
    filter_target_loops(MAP, SET);

    //-------------------------------------
    //  - Now split the loops
    //-------------------------------------
    std::set<ast_foreach*>::iterator I;
    for(I=SET.begin(); I!=SET.end(); I++)
    {
        split_the_loop(*I);
    }
    
    //-------------------------------------
    // Re-do RW analysis
    //-------------------------------------
    gm_redo_rw_analysis(p->get_body());
}
