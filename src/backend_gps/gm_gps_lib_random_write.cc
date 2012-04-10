
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

void gm_gpslib::generate_message_create_for_random_write(
    ast_sentblock* sb, gm_symtab_entry* sym, 
    gm_code_writer& Body) 
{
  gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();
  int m_type = GPS_COMM_RANDOM_WRITE;

  gm_gps_comm_unit U(m_type, sb, sym);

  gm_gps_communication_size_info& SINFO
      = *(info->find_communication_size_info(U));

  Body.NL();
  Body.push("MessageData ");
  Body.push(get_random_write_message_name(sym));
  Body.push(" = new MessageData(");
  // todo: should this always be a byte?
  sprintf(str_buf,"(byte) %d);",SINFO.msg_class->id);
  Body.pushln(str_buf);
}

void gm_gpslib::generate_message_send_for_random_write(
    ast_sentblock* sb, gm_symtab_entry* sym, 
    gm_code_writer& Body) 
{
    Body.push("sendMessage(");
    get_main()->generate_rhs_id(sym->getId());
    Body.push(",");
    Body.push(get_random_write_message_name(sym));
    Body.pushln(");");
}

class gps_random_write_rhs_t : public gm_apply
{
public:
    gps_random_write_rhs_t(ast_sentblock* _sb, gm_symtab_entry* _sym, gm_gpslib* l, gm_code_writer& b) 
    : sb(_sb), sym(_sym), lib(l), Body(b)
    {
        set_for_expr(true);
        U = new gm_gps_comm_unit(GPS_COMM_RANDOM_WRITE, sb, sym);
        INFO =  
                (gm_gps_beinfo *) FE.get_current_backend_info();
    }

    bool apply(ast_expr* e)
    {
        if (!e->is_id() && !e->is_field())
            return true;

        gm_gps_communication_symbol_info* SS; 
        gm_symtab_entry* tg;

        if (e->is_id())
        {
            tg = e->get_id()->getSymInfo();
        }
        else {
            tg = e->get_field()->get_second()->getSymInfo();
            if (e->get_field()->get_first()->getSymInfo() == sym) 
                return true;
        }

        SS = INFO->find_communication_symbol_info(*U, tg);
        if (SS== NULL) return true;

        const char* mname = lib->get_random_write_message_name(sym);
        Body.push(mname); // should not delete this.
        Body.push(".");

        const char* fname = lib->get_message_field_var_name(
        SS->gm_type, SS->idx);
        Body.push(fname); delete [] fname;
        Body.push(" = ");

        if (e->is_id())
            lib->get_main()->generate_rhs_id(e->get_id());
        else
            lib->generate_vertex_prop_access_rhs(
                    e->get_field()->get_second(),
                    Body);

        Body.pushln(";");
        return true;
    }

private:
    ast_sentblock* sb;
    gm_symtab_entry* sym;
    gm_gps_comm_unit* U;
    gm_gps_beinfo* INFO; 
    gm_gpslib* lib; 
    gm_code_writer& Body;

};


void gm_gpslib::generate_message_payload_packing_for_random_write(ast_assign *a, gm_code_writer& Body)
{
    assert(!a->is_argminmax_assign());
    assert(!a->is_target_scalar());

    ast_sentblock* sb = (ast_sentblock*)a->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN);
    assert(sb!=NULL);

    // driver
    gm_symtab_entry* sym = a->get_lhs_field()->get_first()->getSymInfo();

    // traverse rhs and put values in the message
    //printf("sb:%p, sym:%p\n", sb, sym);
    gps_random_write_rhs_t T (sb, sym, this, Body);

    a->get_rhs()->traverse_post(&T);

}


