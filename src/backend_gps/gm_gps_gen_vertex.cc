

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
extern void gm_redirect_reproduce(FILE *f); 
extern void gm_baseindent_reproduce(int i); 
extern void gm_flush_reproduce(); 

void gm_gps_gen::do_generate_vertex()
{
    set_master_generate(false);
    do_generate_vertex_class();
    do_generate_vertex_property_class();
    do_generate_message_class();
}


void gm_gps_gen::do_generate_vertex_property_class()
{
    Body.pushln("//----------------------------------------------");
    Body.pushln("// Vertex Property Class");
    Body.pushln("//----------------------------------------------");
   char temp[1024];
   ast_procdef* proc = FE.get_current_proc(); assert(proc != NULL);
   sprintf(temp, 
           "public static class VertexData extends MinaWritable {"
           //,proc->get_procname()->get_genname()
           ); 
  Body.pushln(temp);

  // list out property
  Body.pushln("// node properties");
  gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();
  std::set<gm_symtab_entry*>& prop = info->get_prop_symbols();
  std::set<gm_symtab_entry* >::iterator I;
  for(I=prop.begin(); I!=prop.end(); I++)
  {
    gm_symtab_entry* sym = *I;
    gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
    sprintf(temp, "%s %s;",
        get_type_string(sym->getType()->get_target_type(), 
            is_master_generate()),
        sym->getId()->get_genname());

    Body.pushln(temp);
  }

  Body.NL();
  get_lib()->generate_vertex_prop_class_details(prop, Body);

  Body.pushln("} // end of vertex-data"); // end of class
  Body.NL();

}

void gm_gps_gen::do_generate_message_class()
{
    Body.pushln("//----------------------------------------------");
    Body.pushln("// Message Data ");
    Body.pushln("//----------------------------------------------");

   char temp[1024];
   ast_procdef* proc = FE.get_current_proc(); assert(proc != NULL);
   gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();
   sprintf(temp, 
           "public static class MessageData extends MinaWritable {"
           ); 
   Body.pushln(temp);
    
   if (info->is_single_message()) {
        Body.pushln("//single messge type; argument ignored");
        Body.pushln("public MessageData(byte type) {}");
   }
   else {
        Body.pushln("byte m_type;");
        Body.pushln("public MessageData(byte type) {m_type = type;}");
   }
   Body.NL();

   do_generate_message_class_default_constructor();

   get_lib()->generate_message_class_details(info, Body);


   Body.pushln("} // end of message-data");
   Body.NL();
}

void gm_gps_gen::do_generate_message_class_default_constructor()
{
    Body.NL();
    Body.pushln("public MessageData() {");
    Body.pushln("// default constructor that is required for constructing a "
		"representative instance for IncomingMessageStore.");
    Body.pushln("}");
}

void gm_gps_gen::do_generate_vertex_class()
{
    char temp[1024];
    const char* proc_name = FE.get_current_proc()
        ->get_procname()->get_genname();
    Body.pushln("//----------------------------------------------");
    Body.pushln("// Main Vertex Class");
    Body.pushln("//----------------------------------------------");
    sprintf(temp,"public static class %sVertex", proc_name);
    Body.pushln(temp);
    Body.push_indent();
    sprintf(temp,
            "extends Vertex< %s.VertexData, %s.MessageData > {",
            proc_name,
            proc_name
            );
    Body.pushln(temp);
    Body.pop_indent();

    do_generate_vertex_constructor();
    do_generate_vertex_get_initial_state_method();
    do_generate_vertex_states();

    Body.pushln("} // end of Vertex");
    Body.NL();

    Body.pushln("//----------------------------------------------");
    Body.pushln("// Factory Class");
    Body.pushln("//----------------------------------------------");
    sprintf(temp,"public static class %sVertexFactory", proc_name);
    Body.pushln(temp);
    Body.push_indent();
    sprintf(temp,
            "extends VertexFactory< %s.VertexData, %s.MessageData > {",
            proc_name,
            proc_name
            );
    Body.pushln(temp);
    Body.pop_indent();
    Body.pushln("@Override");
    sprintf(temp,
            "public Vertex< %s.VertexData, %s.MessageData > newInstance(CommandLine line) {",
            proc_name,
            proc_name
            );
    Body.pushln(temp);
    sprintf(temp,"return new %sVertex(line);", proc_name);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("} // end of VertexFactory");
    Body.NL();

}

void gm_gps_gen::do_generate_vertex_constructor()
{
    char temp[1024];
    const char* proc_name = FE.get_current_proc()->get_procname()->get_genname();
    Body.NL();
    sprintf(temp,"public %sVertex(CommandLine line) {",proc_name);
    Body.pushln(temp);
    Body.pushln("// todo: how to tell if we should parse the command lines or not");
    Body.pushln("// --> no need. master will parse the command line and sent it to the workers");
    Body.pushln("}");
}

void gm_gps_gen::do_generate_vertex_get_initial_state_method()
{
    Body.NL();
    Body.pushln("@Override");
    Body.pushln("public VertexData getInitialValue(int id) {");
    Body.pushln("return new VertexData();");
    Body.pushln("}");
}

void gm_gps_gen::do_generate_vertex_states()
{
    char temp[1024];
    const char* proc_name = FE.get_current_proc()->get_procname()->get_genname();
    Body.NL();
    Body.pushln("@Override");
    sprintf(temp,"public void compute(Iterable<%s.MessageData> _msgs, int _superStepNo) {",proc_name);
    Body.pushln(temp);
    Body.pushln("// todo: is there any way to get this value quickly?");
    Body.pushln("// (this can be done by the first node and saved into a static field)");
    get_lib()->generate_receive_state_vertex("_state_vertex", Body);

    Body.pushln("switch(_state_vertex) { ");
    gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();
    std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
    std::list<gm_gps_basic_block*>::iterator I;
    for(I = bb_blocks.begin(); I!= bb_blocks.end(); I++)
    {
        gm_gps_basic_block* b = *I;
        if (!b->is_vertex()) continue;
        int id = b->get_id();
        sprintf(temp,"case %d: _vertex_state_%d(_msgs); break;", id, id);
        Body.pushln(temp);
    }
    Body.pushln("}");

    Body.pushln("}");

    gm_redirect_reproduce(f_body); // for temporary
    gm_baseindent_reproduce(3);
    for(I = bb_blocks.begin(); I!= bb_blocks.end(); I++)
    {
        gm_gps_basic_block* b = *I;
        if (!b->is_vertex()) continue;
        do_generate_vertex_state_body(b);
    }
    gm_redirect_reproduce(stdout);
    gm_baseindent_reproduce(0);
}

void gm_gps_gen::do_generate_vertex_state_receive_global(gm_gps_basic_block *b)
{

    // l. load scalar variable
    std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
    std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
    for(I=symbols.begin(); I!=symbols.end(); I ++)
    {
        gm_symtab_entry* sym = I->first;
        gps_syminfo* local_info = I->second;
        if (!local_info->is_scalar()) continue;

        gps_syminfo* global_info = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        //if (global_info->is_used_in_multiple_BB())
        if (global_info->is_scoped_global())
        {
            if (local_info->is_used_as_rhs()) {
                // receive it from Broadcast
                generate_scalar_var_def(sym, false);
                Body.push(" = ");
                get_lib()->generate_broadcast_receive_vertex(sym->getId(), Body);
                Body.pushln(";");
            }
        }
        else 
        {
            // temporary scalar variables. Define it here
            generate_scalar_var_def(sym, true);
        }
    }
}

void gm_gps_gen::do_generate_vertex_state_body(gm_gps_basic_block *b)
{
    int id = b->get_id();
    int type = b->get_type();

    char temp[1024];
    sprintf(temp,"private void _vertex_state_%d(Iterable<%s.MessageData> _msgs) {", 
            id, 
            FE.get_current_proc()->get_procname()->get_genname());
    Body.pushln(temp);

    assert (type == GM_GPS_BBTYPE_BEGIN_VERTEX); 

    get_lib()->generate_vertex_prop_access_prepare(Body);
    do_generate_vertex_state_receive_global(b);

    //---------------------------------------------------------
    // Generate Receiver Routine
    //---------------------------------------------------------
    if (b->has_receiver_loops())
    {
        set_receiver_generate(true);
        Body.NL();
        Body.pushln("// Begin msg receive");
        Body.pushln("for(MessageData _msg : _msgs) {");

        std::list<ast_foreach*>& R = b->get_receiver_loops();
        std::list<ast_foreach*>::iterator I;
        if (R.size() != 1) {
            gm_baseindent_reproduce(4);
        }
        for(I=R.begin(); I!=R.end(); I++)
        {
            ast_foreach* fe = *I;
            Body.pushln("/*------");
            Body.flush();
            fe->reproduce(0);
            gm_flush_reproduce(); 
            Body.pushln("-----*/");
            get_lib()->generate_message_receive_begin( fe, Body, b, R.size()==1);

            if (fe->get_body()->get_nodetype() == AST_SENTBLOCK) {
                generate_sent_block((ast_sentblock*)fe->get_body(), false);
            } else {
                generate_sent(fe->get_body());
            }

            get_lib()->generate_message_receive_end( fe, Body, R.size()==1);
        }
        set_receiver_generate(false);
        Body.pushln("}");
        Body.NL();
        gm_baseindent_reproduce(3);
    }

    //---------------------------------------------------------
    // Generate Main Routine
    //---------------------------------------------------------
    if (b->get_num_sents() > 0) {
        Body.pushln("/*------");
        Body.flush();
        b->reproduce_sents();
        Body.pushln("-----*/");
        Body.NL();
    
        ast_sent* s = b->get_1st_sent();
        assert(s->get_nodetype() == AST_FOREACH);
        ast_foreach * fe = (ast_foreach*) s;
        ast_sent* body = fe->get_body();

        // 2. generate sents
        generate_sent(body);
    }

    Body.pushln("}");
}

void gm_gps_gen::generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent)
{
    
    //printf("sym type = %s\n", gm_get_type_string(sym->getType()->getTypeSummary()));
    assert(sym->getType()->is_primitive() || sym->getType()->is_node());

    char temp[1024];
    sprintf(temp, "%s %s",
            get_type_string(sym->getType(), is_master_generate()),
            sym->getId()->get_genname()
           );
    Body.push(temp);

    if (finish_sent)
        Body.pushln(";");
}

