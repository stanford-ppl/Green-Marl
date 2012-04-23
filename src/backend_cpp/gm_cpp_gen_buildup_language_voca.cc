
#include "gm_backend_cpp.h"

void gm_cpp_gen::build_up_language_voca()
{
    gm_vocabulary& V = get_language_voca();

    // list of c++ reserved words
    V.add_word("int");
    V.add_word("unsigned");
    V.add_word("char");
    V.add_word("void");
    V.add_word("short");
    V.add_word("long");
    V.add_word("while");
    V.add_word("for");
    V.add_word("continue");
    V.add_word("break");
    V.add_word("double");
    V.add_word("float");
    V.add_word("if");
    V.add_word("else");
    V.add_word("do");
    V.add_word("return");
    V.add_word("register");
    V.add_word("volatile");
    V.add_word("public");
    V.add_word("class");
    V.add_word("switch");
    V.add_word("case");
    V.add_word("virtual");
    V.add_word("struct");
    V.add_word("typedef");
    V.add_word("auto");
    V.add_word("int32_t");
    V.add_word("int64_t");
    V.add_word("uint32_t");
    V.add_word("uint64_t");

    // some confisung well-known proc names
    V.add_word("rand");
    V.add_word("printf");
    V.add_word("log");
    V.add_word("exp");
    V.add_word("pow");

    get_lib()->build_up_language_voca(V);
}
