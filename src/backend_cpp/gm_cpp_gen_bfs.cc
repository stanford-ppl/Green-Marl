
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

// TODO
#define NODE_T          "node_t"

void gm_cpp_gen::generate_bfs_top()
{
    sprintf(temp, "_bfs_helper _BFS;"); Body.pushln(temp);
    if (is_target_omp()) {
        sprintf(temp, "_BFS.set_num_threads(%s);", MAX_THREADS); Body.pushln(temp); 
    } 
    else {
        sprintf(temp, "_BFS.set_num_threads(1);"); Body.pushln(temp); 
    }
    Body.NL();
}

void gm_cpp_gen::generate_bfs_init(ast_bfs* bfs)
{
    i_temp = TEMP_GEN.getTempName("i");
    char temp[2048];
    // TODO name conflict in C++ world
    //Body.NL();
    Body.pushln("do { // BFS");
    sprintf(temp, "_BFS.set_num_nodes(%s);",get_lib()->max_node_index(bfs->get_source())); Body.pushln(temp);

    if (is_target_omp()) {
        Body.pushln("#pragma omp parallel ");
        Body.pushln("{");
        Body.pushln("#pragma omp for nowait");
    }
    sprintf(temp, "for (int %s = 0; %s < _BFS.BIT_END; %s++)", i_temp, i_temp, i_temp); Body.pushln(temp);
    sprintf(temp, "    _BFS.visited[%s] = 0;", i_temp); Body.pushln(temp);
    if (is_target_omp()) {
        Body.pushln("#pragma omp for nowait");
    }
    sprintf(temp, "for (int %s = 0; %s < _BFS.get_num_nodes(); %s++)", i_temp, i_temp, i_temp); Body.pushln(temp);
    sprintf(temp, "    _BFS.level[%s] = 0;", i_temp); Body.pushln(temp);
    if (is_target_omp()) {
        Body.pushln("}");
    }


    const char* root_name = bfs->get_root()->get_genname();
    sprintf(temp,"_BFS.set_root(%s);", root_name); Body.pushln(temp);

    Body.NL();
}

void gm_cpp_gen::generate_bfs_finish(ast_bfs* bfs)
{
    Body.pushln("} while(false);  // End of BFS");
    Body.NL();

    delete [] i_temp;
}


#define FROM_QUEUE  true
#define FROM_LEVEL  false
#define TO_QUEUE  true
#define TO_LEVEL  false
#define IS_PARALLEL true
#define IS_SEQUENTIAL   false
void gm_cpp_gen::generate_bfs_main(ast_bfs* bfs)
{
    Body.pushln("// main bfs iteration");
    sprintf(temp,"while (!_BFS.is_finished) {"); Body.pushln(temp);
      sprintf(temp,"switch(_BFS.state) {"); Body.pushln(temp);
        sprintf(temp,"case _bfs_helper::ST_SEQ:"); Body.pushln(temp);
            Body.pushln("{int _local_cnt = 0;");
            generate_bfs_iteration_first(bfs, FROM_QUEUE, IS_SEQUENTIAL);
                generate_bfs_expansion(bfs, TO_QUEUE, IS_SEQUENTIAL);
                generate_bfs_body_fw(bfs);
            Body.pushln("}");
            generate_bfs_end_level(bfs, TO_QUEUE, IS_SEQUENTIAL);
        Body.pushln("}");
        Body.pushln("break;\n");

        sprintf(temp,"case _bfs_helper::ST_QUE:"); Body.pushln(temp);
            if (is_target_omp()) {
                Body.pushln("#pragma omp parallel ");
            }
            Body.pushln("{int _local_cnt = 0;");
            generate_bfs_iteration_first(bfs, FROM_QUEUE, IS_PARALLEL);
                generate_bfs_expansion(bfs, TO_QUEUE, IS_PARALLEL);
                generate_bfs_body_fw(bfs);
            Body.pushln("}");
            generate_bfs_end_level(bfs, TO_QUEUE, IS_PARALLEL);
        Body.pushln("}");
        Body.pushln("break;\n");

        sprintf(temp,"case _bfs_helper::ST_TO_RD:"); Body.pushln(temp);
            if (is_target_omp()) {
                Body.pushln("#pragma omp parallel ");
            }
            Body.pushln("{int _local_cnt = 0;");
            generate_bfs_iteration_first(bfs, FROM_QUEUE, IS_PARALLEL);
                generate_bfs_expansion(bfs, TO_LEVEL, IS_PARALLEL);
                generate_bfs_body_fw(bfs);
            Body.pushln("}");
            generate_bfs_end_level(bfs, TO_LEVEL, IS_PARALLEL);
        Body.pushln("}");
        Body.pushln("break;\n");

        sprintf(temp,"case _bfs_helper::ST_RD:"); Body.pushln(temp);
            if (is_target_omp()) {
                Body.pushln("#pragma omp parallel ");
            }
            Body.pushln("{int _local_cnt = 0;");
            generate_bfs_iteration_first(bfs, FROM_LEVEL, IS_PARALLEL);
                generate_bfs_expansion(bfs, TO_LEVEL, IS_PARALLEL);
                generate_bfs_body_fw(bfs);
            Body.pushln("}");
            generate_bfs_end_level(bfs, TO_LEVEL, IS_PARALLEL);
        Body.pushln("}");
        Body.pushln("break;\n");

        sprintf(temp,"case _bfs_helper::ST_TO_QUE:"); Body.pushln(temp);
            if (is_target_omp()) {
                Body.pushln("#pragma omp parallel ");
            }
            Body.pushln("{int _local_cnt = 0;");
            generate_bfs_iteration_first(bfs, FROM_LEVEL, IS_PARALLEL);
                generate_bfs_expansion(bfs, TO_QUEUE, IS_PARALLEL);
                generate_bfs_body_fw(bfs);
            Body.pushln("}"); 
            generate_bfs_end_level(bfs, TO_QUEUE, IS_PARALLEL);
        Body.pushln("}  ");
        Body.pushln("break;\n");
      Body.pushln("} // end of switch");
      Body.pushln("_BFS.choose_method();");
    Body.pushln("} // end of while");
}

void gm_cpp_gen::generate_bfs_main_back(ast_bfs* bfs)
{
    if (bfs->get_bbody() != NULL) {
        Body.NL();
        Body.pushln("// Reverse order iteration");

        Body.pushln("while (_BFS.curr_level-- > 0) {");

            if (is_target_omp()) {
                Body.pushln("#pragma omp parallel for");
            }
            bool b;
            get_lib()->generate_iteration_header(bfs->get_iterator(), GMTYPE_NODEITER_ALL, bfs->get_source(), true, b);
            Body.push("{ ");
            sprintf(temp, "if ((_BFS.level[%s] != _BFS.curr_level)) continue;", 
                bfs->get_iterator()->get_genname(), bfs->get_iterator()->get_genname()); Body.pushln(temp);

            generate_bfs_body_bw(bfs);
            Body.pushln("}");
        Body.pushln("}");
        
        // root level
        Body.pushln("//root level");
        sprintf(temp, "%s %s = %s;", NODE_T, bfs->get_iterator()->get_genname(), bfs->get_root()->get_genname());
        Body.pushln(temp);
        generate_bfs_body_bw(bfs);

        // end of reverse iteration");
        Body.NL();
    }
}

void gm_cpp_gen::generate_bfs_iteration_first(ast_bfs* bfs, bool from_queue, bool is_parallel)
{
    if (is_parallel) {
         if (is_target_omp()) {
                Body.pushln("#pragma omp for ");
         }
    }
    if (from_queue) {
        sprintf(temp, "for(int %s=0; %s < _BFS.cnt_curr; %s++) {", i_temp, i_temp, i_temp); Body.pushln(temp);
        sprintf(temp, "%s %s = _BFS.curr_Q[%s];", NODE_T, 
                bfs->get_iterator()->get_genname(), i_temp); Body.pushln(temp);
    } else {
        bool b;
        get_lib()->generate_iteration_header(bfs->get_iterator(), GMTYPE_NODEITER_ALL, bfs->get_source(), true, b);
        Body.push("{ ");
        sprintf(temp, "if (_BFS.level[%s] != _BFS.curr_level) continue;", 
                bfs->get_iterator()->get_genname()); Body.pushln(temp);
    }
}
void gm_cpp_gen::generate_bfs_body_fw(ast_bfs* bfs)
{
    // filters should have been changed into if
    if (bfs->get_fbody() != NULL) {
        Body.NL();
        Body.pushln("// body");
        ast_sentblock *sb = bfs->get_fbody();
        generate(sb, false);
    } else {
        Body.pushln("// NULL body");
    }
}

void gm_cpp_gen::generate_bfs_body_bw(ast_bfs* bfs)
{
    if (bfs->get_fbody() != NULL) {
        Body.NL();
        Body.pushln("// body");
        ast_sentblock *sb = bfs->get_bbody();
        generate(sb, false);
    } else {
        Body.pushln("// NULL body");
    }
}

void gm_cpp_gen::generate_bfs_expansion(ast_bfs* bfs, bool to_queue, bool is_parallel)
{
    Body.NL();
    Body.pushln("// expansion (test and test-set)");
    assert(bfs->get_edge_cond() == NULL); // [todo] for the time being
    assert(bfs->get_iterator2() != NULL);

    // expansion using iter2
    bool b;
    get_lib()->generate_iteration_header(bfs->get_iterator2(), bfs->get_iter_type2(), bfs->get_iterator(), is_parallel, b);
    Body.pushln("{");
    if (b) {
        get_lib()->generate_iteration_header_init(bfs->get_iterator2(), bfs->get_iter_type2(), bfs->get_iterator(), is_parallel); 
    }

    // generate filter
    if (bfs->get_node_cond() != NULL) {
        Body.push("if (!(");
        generate(bfs->get_node_cond());
        Body.pushln(")) // filter");
        Body.pushln("    continue;");
    }

    // using bitmap check 
    char* name = bfs->get_iterator2()->get_genname();
    if (!is_parallel) {
        sprintf(temp, "_BFS.add_next_seq(%s);", name);
        Body.pushln(temp);
    } else if (to_queue) {
        sprintf(temp, "_BFS.add_next_local_q(%s, %s, _local_cnt);", 
                name, THREAD_ID);
        Body.pushln(temp);
    } else {
        sprintf(temp, "_BFS.add_next_level(%s, _local_cnt);", 
                name);
        Body.pushln(temp);
    }
    Body.pushln("}");
}

void gm_cpp_gen::generate_bfs_end_level(ast_bfs* bfs, bool to_queue, bool is_parallel)
{
    if (to_queue && is_parallel) {
        sprintf(temp, "_BFS.copy_local_q(%s, _local_cnt);", THREAD_ID);
        Body.pushln(temp);
    }
    else if (is_parallel) {
        sprintf(temp, "_BFS.increase_next_count(%s, _local_cnt);", THREAD_ID);
        Body.pushln(temp);
    }
}


bool gm_cpp_gen::generate(ast_bfs* bfs)
{
    generate_bfs_init(bfs);

    generate_bfs_main(bfs);

    generate_bfs_main_back(bfs);

    generate_bfs_finish(bfs);

    return true;
}
