
#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

void gm_giraphlib::generate_message_send_for_random_write(
    ast_sentblock* sb, gm_symtab_entry* sym, 
    gm_code_writer& Body) 
{
    Body.push("sendMsg(");
    get_main()->generate_rhs_id(sym->getId());
    Body.push(",");
    Body.push(get_random_write_message_name(sym));
    Body.pushln(");");
}
