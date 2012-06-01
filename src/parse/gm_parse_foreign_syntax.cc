
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include "gm_ast.h"

#define BEGIN(X, from_next, S) {\
    X##_begin = (from_next) ? curr_ptr+1 : curr_ptr;   \
    X##_begin_line = line; \
    X##_begin_col = (from_next) ? col+1 : col; \
    state =  S; \
}

#define END(X, till_curr, S) {\
    X##_end = (till_curr) ? curr_ptr : curr_ptr-1;   \
    assert(X##_begin != NULL) ;                      \
    assert(X##_end >= X##_begin);                    \
}

#define FINISH_ELEMENT() {                          \
    if (TEXT_begin == NULL) {                       \
        std::string S;                              \
        /*printf("parsed text:%s\n", S.c_str()); */   \
        parsed_foreign.push_back(S);                \
    } else {                                                    \
        assert (TEXT_end >= TEXT_begin );                       \
        std::string S(TEXT_begin, (TEXT_end - TEXT_begin +1));  \
        /*printf("parsed text:%s\n", S.c_str());*/      \
        parsed_foreign.push_back(S);                            \
    }                                               \
    ast_id* id1=NULL;                               \
    if (ID_begin == NULL) {                         \
        parsed_gm.push_back(NULL);                  \
    } else {                                                    \
        assert (ID_end >= ID_begin );                       \
        std::string S(ID_begin, (ID_end - ID_begin +1));        \
        id1 = ast_id::new_id(S.c_str(), ID_begin_line, ID_begin_col); \
        if (FIELD_begin == NULL) {                  \
            id1->set_parent(this);                  \
            parsed_gm.push_back(id1);               \
        }                                           \
        else {                                      \
            assert (FIELD_end >= FIELD_begin );                       \
            std::string S(                          \
                    FIELD_begin, (FIELD_end - FIELD_begin +1));     \
            ast_id* id2 = ast_id::new_id(                           \
                    S.c_str(), FIELD_begin_line, FIELD_begin_col);  \
            ast_field* field = ast_field::new_field(id1, id2);      \
            field->set_parent(this);                \
            parsed_gm.push_back(field);             \
        }                                           \
    }                                               \
    ID_begin = NULL;                                \
    FIELD_begin = NULL;                             \
    TEXT_begin = NULL;                              \
}

void ast_expr_foreign::parse_foreign_syntax()
{
    // scan through the original text and find '$' symbol
    //int size = strlen(orig_text);

    char* ID_begin = NULL;
    char* ID_end = NULL;                // inclusive
    char* FIELD_begin = NULL;
    char* FIELD_end = NULL;             // inclusive
    char* TEXT_begin = NULL;
    char* TEXT_end = NULL;              // inclusive
    char* curr_ptr = orig_text;

    int  ID_begin_line=0;
    int  ID_begin_col=0;
    int  FIELD_begin_line=0;
    int  FIELD_begin_col=0;
    int  TEXT_begin_line=0;
    int  TEXT_begin_col=0;
    
    int  state = 0;
    int  line = get_line();
    int  col = get_col();

    const int S_TEXT       = 0;
    const int S_ID         = 1;  // $ SEEN
    const int BEFORE_DOT    = 2;  // . WILL BE SEEN has white space
    const int AFTER_DOT     = 3;  // . SEEN has while space
    const int S_FIELD       = 4;

#if 0
    printf("ORG:[%s]\n", orig_text);
#endif

#define FROM_NEXT   true
#define FROM_CURR   false
#define TILL_CURR   true
#define TILL_PREV   false
    BEGIN(TEXT, FROM_CURR, S_TEXT);

    // [TODO] consideration for built-in functions
    // we will make
    //  [TEXT->NODE] -> [TEXT->NODE], -> [TEXT,NODE] 
    //     Text can be ""
    //     ID can be NULL
    while (*curr_ptr != '\0')
    {
        char c = *curr_ptr;
        if (c=='\n') line++;

        switch(state) {
        case S_TEXT:
            if (c == '$') { // begin GM 
                // check if next character is alpha numeric
                char d = *(curr_ptr+1);
                if (isalpha(d) || (d == '_')) {
                    if (curr_ptr == TEXT_begin) {
                        TEXT_begin = NULL;
                        TEXT_end = NULL;
                    }
                    else {
                        END(TEXT,TILL_PREV, S_TEXT);
                    }
                    BEGIN(ID, FROM_NEXT, S_ID);
                }
            }
            break;

        case S_ID:
            if (isalnum(c) || (c=='_'))  {} // do nothing
            else 
            {
                END(ID, TILL_PREV, S_ID);
                if (c == '.') {
                    // check if ID comes
                    bool dot_follow = false;
                    char* p = curr_ptr+1;
                    while (isspace(*p)) p++;
                    if (isalnum(*p) || (*p== '_'))
                    {
                        dot_follow = true;
                    }

                    if (dot_follow) {
                        state = AFTER_DOT;
                    }
                    else {
                        BEGIN(TEXT, FROM_CURR, S_TEXT);
                    }
                }
                else if (isspace(c)) {
                    END(ID, TILL_PREV, S_ID);

                    // look forward and check if '.' follows after space
                    char* p = curr_ptr;
                    bool dot_follow = false;
                    while (isspace(*p)) p++;
                    if (*p == '.') {
                        // check if alpha numeric comes after space
                        p++;
                        while (isspace(*p)) p++;
                        if (isalpha(c) || (c== '_')) dot_follow = true;
                    }
                    if (dot_follow) {
                        state = BEFORE_DOT;
                    } else {
                        FINISH_ELEMENT();
                        BEGIN(TEXT, FROM_CURR, S_TEXT);
                    }
                }
                else {
                    FINISH_ELEMENT();
                    BEGIN(TEXT, FROM_CURR, S_TEXT);
                }
            }
            break;

        case BEFORE_DOT:
            if (isspace(c)) {} // do nothing
            else if (c == '.') {state = AFTER_DOT;}
            else {assert(false);}
            break;

        case AFTER_DOT:
            if (isspace(c)) {} // consume
            else if (isalpha(c) || (c== '_')) {
                BEGIN(FIELD, FROM_CURR, S_FIELD);
            }    
            break;

        case S_FIELD:
            if (isalnum(c) || (c=='_'))  {} // do nothing
            else {
                END(FIELD, TILL_PREV, S_FIELD);
                FINISH_ELEMENT();
                BEGIN(TEXT, FROM_CURR, S_TEXT);
            }
            break;
        }

        curr_ptr++;
        col++;
    }

    // finialize
    switch(state) {
        case S_TEXT:
            if (curr_ptr == TEXT_begin){} //do nothing
            else {
                END(TEXT, TILL_PREV, S_TEXT);
                FINISH_ELEMENT();
            }
            break;
        case S_ID:
            END(FIELD, TILL_PREV, S_FIELD);
            FINISH_ELEMENT();
            break;
        case S_FIELD:
            END(FIELD, TILL_PREV, S_FIELD);
            FINISH_ELEMENT();
            break;
    }

    //-----------------------------------------
    //Let's see what happend
    //-----------------------------------------
#if 0

    std::list<ast_node*>::iterator I;   
    std::list<std::string>::iterator J; 

    J = parsed_foreign.begin();
    I = parsed_gm.begin();
    for(; I!=parsed_gm.end(); I++, J++)
    {
        std::string& S = *J;
        ast_node* node = *I;
        printf("TEXT[%s]::", S.c_str());
        if (node == NULL) 
        {
            printf("(NIL)");
        }
        else if (node->get_nodetype() == AST_FIELD) 
        {
            ast_field* f = (ast_field*) node;
            printf("FIELD[%s.%s(%d.%d)]",
                    f->get_first()->get_orgname(), f->get_second()->get_orgname(),
                    f->get_first()->get_line(), f->get_first()->get_col());
        }
        else if (node->get_nodetype() == AST_ID)
        {
            ast_id* i = (ast_id*) node;
            printf("ID[%s(%d.%d)]",
                    i->get_orgname(), i->get_line(), i->get_col());
        }
        else {
            assert(false);
        }
        printf("-->");
    }
    printf("\n");

#endif
}
