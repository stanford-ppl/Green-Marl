#include "gm.h"

/* Convert one file format into another (with properties)
 *    ./gm_format_converter <in_schema.txt> 
 *
 */
void print_schema_file_format()
{   
    fprintf(stderr, "#Schema File Format:\n");
    fprintf(stderr, "\t# begins line-comment\n");
    fprintf(stderr, "\t# no distinguish between upper character and lower character\n");
    fprintf(stderr, "\t# <type> is one of : int, long, float, double, bool, node, edge\n");
    fprintf(stderr, "\t# <name> is alpha-numeric string\n");
    fprintf(stderr, "\t\n");
    fprintf(stderr, "\t# First section begin with keyword INPUT\n");
    fprintf(stderr, "\t# Then specify all node properties, in one line, after NODE: \n");
    fprintf(stderr, "\t# like NODE: <type>(<name>), <type>(<name>), ...\n");
    fprintf(stderr, "\t# Then specify all edge properties, in one line, after EDGE: \n");
    fprintf(stderr, "\t# like EDGE: <type>(<name>), <type>(<name>), ...\n");
    fprintf(stderr, "INPUT\n");
    fprintf(stderr, "NODE: int(X), long, node(Y)\n");
    fprintf(stderr, "EDGE: int(Z), double, double\n");
    fprintf(stderr, "\t\n");

    /*
    fprintf(stderr, "\t# Second section is optional, and it begin with keyword OUTPUT\n");
    fprintf(stderr, "\t# (If there is no OUTPUT section, all the input properties are saved\n");
    fprintf(stderr, "OUTPUT\n");
    fprintf(stderr, "\t# Then specify all (output) node properties, in one line, after NODE: \n");
    fprintf(stderr, "\t# NODE: <name>, <name>, ...\n");
    fprintf(stderr, "NODE: X, Y\n");
    fprintf(stderr, "\t# Then specify all (output) node properties, in one line, after EDGE: \n");
    fprintf(stderr, "\t# EDGE: <name>, <name>, ...\n");
    fprintf(stderr, "EDGE: \n");
    */
}

bool parse_schema_file(char* filename, gm_default_usermain& Main);

bool output_schema_defined = false;
std::vector<std::string> input_schema_name_node;
std::vector<VALUE_TYPE>  input_schema_type_node;
std::vector<std::string> input_schema_name_edge;
std::vector<VALUE_TYPE>  input_schema_type_edge;
//std::vector<std::string> output_schema_names; // to be added
//
int get_schema_name_index(std::string &S, bool is_node)
{
    std::vector<std::string> &input_schema_name =
        is_node ? input_schema_name_node : input_schema_name_edge;
    for(int i=0;i<(int)input_schema_name.size(); i++)
    {
        if (input_schema_name[i] == S)
            return i;
    }
    return -1;
}

// push copy of name and type into vectors
void push_name_and_type(std::string &S, VALUE_TYPE type, bool is_node)
{
    std::vector<std::string> &input_schema_name =
        is_node ? input_schema_name_node : input_schema_name_edge;
    std::vector<VALUE_TYPE> &input_schema_type =
        is_node ? input_schema_type_node : input_schema_type_edge;

    input_schema_name.push_back(S);
    input_schema_type.push_back(type);

    //printf("adding %s, %d\n", S.c_str(), type);
}

void redefine_options(gm_useropt& OPTIONS, char* name)
{
    OPTIONS.set_execname(name);
    OPTIONS.remove_all();

    OPTIONS.add_argument("InputFile",  GMTYPE_END,  "Input filename");
    OPTIONS.add_argument("OutputFile",  GMTYPE_END, "Output filename");
    OPTIONS.add_argument("SchemaFile",  GMTYPE_END, "Schema filename");

    OPTIONS.add_option("GMOutputFormat",  GMTYPE_END,  NULL, "ADJ, EBIN, EDGE_LIST\n");
    OPTIONS.add_option("GMInputFormat",  GMTYPE_END,   NULL, "ADJ, EBIN, EDGE_LIST\n");
}

void create_schema_definitions(gm_default_usermain& Main)
{
    if (output_schema_defined)
    {
        assert(false);
    }
    else
    {
        // output schema same as input schema
        for(int i=0;i<(int)input_schema_name_node.size(); i++)
        {
            Main.declare_property(input_schema_name_node[i].c_str(), input_schema_type_node[i], true, true, GM_NODEPROP);
        }
        for(int i=0;i<(int)input_schema_name_edge.size(); i++)
        {
            Main.declare_property(input_schema_name_edge[i].c_str(), input_schema_type_edge[i], true, true, GM_EDGEPROP);
        }
    }
}

bool process_arguments(int argc, char** argv, gm_default_usermain& Main)
{
    gm_useropt& OPTIONS = Main.get_options();
    redefine_options(OPTIONS, argv[0]);

    if (!OPTIONS.parse_command_args(argc, argv))
        goto err_return;

    if (OPTIONS.is_option_defined("?"))
        goto err_return;

    if (OPTIONS.get_num_args_defined() <  OPTIONS.get_num_args_declared()) 
    {
        printf("Error: need more arguements\n");
        goto err_return;
    }

    if (!Main.determine_formats())
    {
        printf("Error in determining formats\n");
        return false;
    }

    // parse schema file
    if (parse_schema_file((char*)(OPTIONS.get_arg(2)), Main) == false) {
        fprintf(stderr,"Error in reading schema_file\n");
        print_schema_file_format();
        return false;
    }

    // create schema definitions from parsed result
    create_schema_definitions(Main);
    Main.create_property_in_out_schema();
    return true;

err_return:
    OPTIONS.print_help(); fflush(stdout);
    print_schema_file_format();
    return false;
}

int main(int argc, char** argv)
{
    gm_default_usermain Main;
    if (!process_arguments(argc, argv, Main))
        return EXIT_FAILURE;

    if (!Main.do_preprocess()) { // load graph
        return EXIT_FAILURE;
    }

    if (!Main.do_postprocess()) { // store graph
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


static char linebuf[4096];
static int ptr;
static bool line_finished;
static int counter = 0;

void skip_space()
{
    if (line_finished) return;

    while(true) {
       char c = linebuf[ptr];
       if ((c == ' ') || (c=='\t') || (c=='\r')) {ptr++; continue;}
       if ((c == '#') || (c=='\n')) {line_finished = true; return;}
       return;
    }
}

char* get_alpha_numeric()
{
    char temp[1024];
    skip_space();
    if (line_finished) return NULL;
    int idx = 0;

    char c = tolower(linebuf[ptr]);
    if (!isalpha(c)) return NULL;
    temp[idx++] = c;
    ptr++;

    while(true) {
        char c = tolower(linebuf[ptr]);
        if (!isalpha(c) && !isdigit(c)) {
            temp[idx] = '\0';
            return strdup(temp);
        }
        ptr++;
        temp[idx++] = c;
    }
}
bool look_for_token(char token)
{
    skip_space();
    if (line_finished) return false;
    char c = tolower(linebuf[ptr]);
    if (c == token) {
        ptr ++;  // consume
        return true;
    }
    else 
        return false;
}

bool get_nonempty_line(FILE *f)
{
   while (true) {
        char* p = fgets(linebuf, 4096, f);
        if (p == NULL) return false; // end of line
        line_finished = false;
        ptr = 0;

        // eat initial white space
        skip_space();
        if (!line_finished) return true;
   }
}

bool look_for_keyword(const char* KW)
{
    char* word =  get_alpha_numeric();
    if ((word != NULL) && (!strcmp(word, KW)))
        return true;
    else
        return false;
}



bool process_keyword_line(FILE*f, const char* KW, bool print_error)
{
    if (!get_nonempty_line(f)) goto error_ret;
    if (!look_for_keyword(KW)) goto error_ret;
    return true;

error_ret:
    if (print_error) {
        fprintf(stderr, "cannot find keyword: %s\n", KW);
    }
    return false;
}

VALUE_TYPE parse_gm_type(const char* c)
{
    if (!strcmp(c, "int")) {return GMTYPE_INT;}
    else if (!strcmp(c, "bool"))   {return GMTYPE_BOOL;}
    else if (!strcmp(c, "node"))   {return GMTYPE_NODE;}
    else if (!strcmp(c, "edge"))   {return GMTYPE_EDGE;}
    else if (!strcmp(c, "long"))   {return GMTYPE_LONG;}
    else if (!strcmp(c, "double")) {return GMTYPE_DOUBLE;}
    else if (!strcmp(c, "float"))  {return GMTYPE_FLOAT;}
    else return GMTYPE_END;
}

bool process_input_schema(FILE*f, bool is_node)
{
    const char* keyword = is_node ? "node" : "edge";
    if (!get_nonempty_line(f)) {
        fprintf(stderr, "cannot find %s definition line \n", keyword);
        return false;
    }
    if (!look_for_keyword(keyword)){
        fprintf(stderr, "cannot find line begin with '%s:' \n", keyword);
        return false;
    }
    if (!look_for_token(':')) {
        fprintf(stderr, "cannot find line begin with '%s::' \n", keyword);
        return false;
    }

    while(true)
    {
        char* type = get_alpha_numeric();
        if (type == NULL) break;
        VALUE_TYPE t = parse_gm_type(type);
        if (t == GMTYPE_END) { // error type
            fprintf(stderr, "undefined type: %s\n", type);
            return false;
        }

        // check if name is defined
        if (look_for_token('(')) {
            char* name = get_alpha_numeric();
            if (name == NULL) {
                fprintf(stderr, "cannot find property name after '('\n");
                return false;
            }
            std::string S(name); 
            if (get_schema_name_index(S, is_node) != -1) {
                fprintf(stderr, "name %s has been already defined\n", name);
                return false;
            }

            if (!look_for_token(')')) {
                fprintf(stderr, "unclosed parenthesis after %s\n", name);
                return false;
            }

            push_name_and_type(S, t, is_node);
        }
        else {
            char temp[128];
            sprintf(temp, "__prop%d", counter++);
            std::string S(temp); 
            push_name_and_type(S, t, is_node);
        }

        //',' is optional
        look_for_token(',');
    }

    return true;
}


bool parse_schema_file(char* filename, gm_default_usermain& Main) 
{
    FILE *f = fopen(filename, "r");
    if (f==NULL) {
        fprintf(stderr, "cannot open schema_file: %s\n", filename);
        return false;
    }

    if (!process_keyword_line(f, "input", true)) goto error;
    if (!process_input_schema(f, true)) goto error;
    if (!process_input_schema(f, false)) goto error;

    // no output section. ignore 
    if (!process_keyword_line(f, "output", false)) return true;

    return true;

error:
    fclose(f);
    return false;
}
