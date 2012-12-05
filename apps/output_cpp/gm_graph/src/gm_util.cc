#include "gm_util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * This is an implementation of the Tokenizer class.
 * This implementation uses the methods in the C++ string class;
 * specifically, find_first_not_of, find_first_of, and substr.
 */

void GM_Tokenizer::reset() {
    current_pos_ = 0;
    next_token_start_ = 0;
    found_next_token_start_ = false;
    number_of_tokens_ = 0;
}

void GM_Tokenizer::findNextTokenStart() {
    if ( ! found_next_token_start_) {
        next_token_start_ = str_.find_first_not_of(delim_, current_pos_);
        found_next_token_start_ = true;
    }
}

std::string GM_Tokenizer::getNextToken() {
    if ( ! found_next_token_start_) {
        findNextTokenStart();
    }
    size_t next_token_end = str_.find_first_of(delim_, next_token_start_);
    current_pos_ = next_token_end; // setting beyond the next token
    found_next_token_start_ = false;
    return str_.substr(next_token_start_, next_token_end - next_token_start_);
}

bool GM_Tokenizer::hasNextToken() {
    if ( ! found_next_token_start_) {
        findNextTokenStart();
    }
    return next_token_start_ != std::string::npos;
}

long GM_Tokenizer::countNumberOfTokens() {
    if (number_of_tokens_ == 0) {
        for (size_t pos = 0; pos != std::string::npos; number_of_tokens_++) {
            pos = str_.find_first_not_of(delim_, pos);
            if (pos == std::string::npos)
                break;
            pos = str_.find_first_of(delim_, pos);
        }
    }
    return number_of_tokens_;
}

/*
 * Method to create arrays based on a given value type and size.
 */
void *getArrayType(VALUE_TYPE vt, int size) {
    switch(vt) {
        case GMTYPE_BOOL: return (void *) new bool[size];
        case GMTYPE_INT: return (void *) new int[size];
        case GMTYPE_LONG: return (void *) new long[size];
        case GMTYPE_FLOAT: return (void *) new float[size];
        case GMTYPE_DOUBLE: return (void *) new double[size];
        case GMTYPE_END: 
        default: assert(false); return NULL; // Control should never reach this case.
    }
    return NULL;
}

void* gmutil_createVectorType(VALUE_TYPE vt) 
{
    switch(vt) {
        case GMTYPE_BOOL: return (void *) new GM_BVECT();
        case GMTYPE_INT: return (void *) new GM_IVECT();
        case GMTYPE_LONG: return (void *) new GM_LVECT();
        case GMTYPE_FLOAT: return (void *) new GM_FVECT();
        case GMTYPE_DOUBLE: return (void *) new GM_DVECT();
        case GMTYPE_NODE: return (void *) new GM_NVECT();
        case GMTYPE_EDGE: return (void *) new GM_EVECT();
        case GMTYPE_END: 
        default: assert(false); return NULL; // Control should never reach this case.
    }
}
void gmutil_deleteVectorType(void* vector, VALUE_TYPE vt) 
{
    switch(vt) {
        case GMTYPE_BOOL: delete ((GM_BVECT*) vector); break;
        case GMTYPE_INT: delete ((GM_IVECT*) vector); break;
        case GMTYPE_LONG: delete ((GM_LVECT*) vector); break;
        case GMTYPE_FLOAT: delete ((GM_FVECT*) vector); break;
        case GMTYPE_DOUBLE: delete ((GM_DVECT*) vector); break;
        case GMTYPE_NODE: delete ((GM_NVECT*) vector); break;
        case GMTYPE_EDGE: delete ((GM_EVECT*) vector); break;
        case GMTYPE_END: 
        default: assert(false); break;// Control should never reach this case.
    }
    return;
}

void gmutil_loadValueIntoVector(void *vector, std::string val, VALUE_TYPE vt) 
{
    switch(vt) {
        case GMTYPE_BOOL: 
        {
            bool b = (val == "true");
            GM_BVECT* V = (GM_BVECT*) vector;
            V->push_back(b);
            break;
        }
            
        case GMTYPE_INT:
        {
            int i = atoi(val.c_str());
            GM_IVECT* V = (GM_IVECT*) vector;
            V->push_back(i);
            break;
        }

        case GMTYPE_LONG: 
        {
            long l = atol(val.c_str());
            GM_LVECT* V = (GM_LVECT*) vector;
            V->push_back(l);
            break;
        }

        case GMTYPE_FLOAT: 
        {
            float f = strtof(val.c_str(), NULL);
            GM_FVECT* V = (GM_FVECT*) vector;
            V->push_back(f);
            break;
        }

        case GMTYPE_DOUBLE: 
        {
            double d = strtod(val.c_str(), NULL);
            GM_DVECT* V = (GM_DVECT*) vector;
            V->push_back(d);
            break;
        }

        case GMTYPE_END: 
        default:
            assert(false); return; // Control should never reach this case.
    }
}
void gmutil_loadDummyValueIntoVector(void *vector, VALUE_TYPE vt) 
{
    switch(vt) {
        case GMTYPE_BOOL: { GM_BVECT* V = (GM_BVECT*) vector; V->push_back(false); break; }
        case GMTYPE_INT:  { GM_IVECT* V = (GM_IVECT*) vector; V->push_back(0); break; }
        case GMTYPE_LONG: { GM_LVECT* V = (GM_LVECT*) vector; V->push_back(0); break; }
        case GMTYPE_FLOAT: { GM_FVECT* V = (GM_FVECT*) vector; V->push_back(0); break; }
        case GMTYPE_DOUBLE: { GM_DVECT* V = (GM_DVECT*) vector; V->push_back(0); break; }
        case GMTYPE_END: 
        default:
            assert(false); return; // Control should never reach this case.
    }
}

static void gmutil_copyVectorIntoArray(GM_BVECT& SRC, bool* dest) {
    #pragma omp parallel for
    for(size_t i = 0; i < SRC.size(); i++) dest[i] = SRC[i];
}
static void gmutil_copyVectorIntoArray(GM_IVECT& SRC, int* dest) {
    #pragma omp parallel for
    for(size_t i = 0; i < SRC.size(); i++) dest[i] = SRC[i];
}
static void gmutil_copyVectorIntoArray(GM_LVECT& SRC, long* dest) {
    #pragma omp parallel for
    for(size_t i = 0; i < SRC.size(); i++) dest[i] = SRC[i];
}
static void gmutil_copyVectorIntoArray(GM_FVECT& SRC, float* dest) {
    #pragma omp parallel for
    for(size_t i = 0; i < SRC.size(); i++) dest[i] = SRC[i];
}
static void gmutil_copyVectorIntoArray(GM_DVECT& SRC, double* dest) {
    #pragma omp parallel for
    for(size_t i = 0; i < SRC.size(); i++) dest[i] = SRC[i];
}

void gmutil_copyVectorIntoArray(void* vector, void* array, VALUE_TYPE vt)
{
    // should test copy vs array pointer
    switch(vt) {
        case GMTYPE_BOOL: gmutil_copyVectorIntoArray( *((GM_BVECT*)vector), (bool*) array);break;
        case GMTYPE_INT:  gmutil_copyVectorIntoArray( *((GM_IVECT*)vector), (int*) array);break;
        case GMTYPE_LONG:  gmutil_copyVectorIntoArray( *((GM_LVECT*)vector), (long*) array);break;
        case GMTYPE_FLOAT:  gmutil_copyVectorIntoArray( *((GM_FVECT*)vector), (float*) array);break;
        case GMTYPE_DOUBLE:  gmutil_copyVectorIntoArray( *((GM_DVECT*)vector), (double*) array);break;

        case GMTYPE_END: 
        default:
            assert(false); return ; // Control should never reach this case.
    }
}




/*
 * Method to load a value from the given string 'val'
 * and write to a given location in an array based on the given value type.
 */
void loadValueBasedOnType(void *arr, long pos, std::string val, VALUE_TYPE vt) {
    switch(vt) {
        case GMTYPE_BOOL: ((bool *)arr)[pos] = (val == "true"); break;
        case GMTYPE_INT: ((int *)arr)[pos] = atoi(val.c_str()); break;
        case GMTYPE_LONG: ((long *)arr)[pos] = atol(val.c_str()); break;
        case GMTYPE_FLOAT: ((float *)arr)[pos] = strtof(val.c_str(), NULL); break;
        case GMTYPE_DOUBLE: ((double *)arr)[pos] = strtod(val.c_str(), NULL); break;
        case GMTYPE_END: 
        default:
                    assert(false); return; // Control should never reach this case.
    }
}

const char* gmutil_getTypeString(VALUE_TYPE v)
{
    return
                (v == GMTYPE_BOOL) ? "bool" :
                (v == GMTYPE_INT) ? "int" :
                (v == GMTYPE_LONG) ? "long" :
                (v == GMTYPE_FLOAT) ? "float" :
                (v == GMTYPE_DOUBLE) ? "double" :
                (v == GMTYPE_EDGE) ? "edge" :
                (v == GMTYPE_NODE) ? "node" : "??";
}
        //void* scalar_var = create_scalar_variable(S.type);
/*
 * Method to read a value from the given location in an array based on the given value type
 * and store it in the file
 */
void storeValueBasedOnType(void *arr, long pos, std::ofstream& file, VALUE_TYPE vt) {
    switch(vt) {
        case GMTYPE_BOOL: file << std::boolalpha << ((bool *)arr)[pos]; break;
        case GMTYPE_INT: file << ((int *)arr)[pos]; break;
        case GMTYPE_LONG: file << ((long *)arr)[pos]; break;
        case GMTYPE_FLOAT: file << ((float *)arr)[pos]; break;
        case GMTYPE_DOUBLE: file << ((double *)arr)[pos]; break;
        case GMTYPE_NODE: file << ((node_t *)arr)[pos]; break;
        case GMTYPE_EDGE: file << ((edge_t *)arr)[pos]; break;
        case GMTYPE_END: assert (false); return; // Control should never reach this case.
    }
}

/*
 * Method to read a value from the given location in an array based on the given value type
 * and write to a file using GM_Writer
 */
void storeValueBasedOnType(void *arr, long pos, GM_Writer& writer, VALUE_TYPE vt) {
    switch(vt) {
        case GMTYPE_BOOL: writer.write(((bool *)arr)[pos]); break;
        case GMTYPE_INT: writer.write(((int *)arr)[pos]); break;
        case GMTYPE_LONG: writer.write(((long *)arr)[pos]); break;
        case GMTYPE_FLOAT: writer.write(((float *)arr)[pos]); break;
        case GMTYPE_DOUBLE: writer.write(((double *)arr)[pos]); break;
        case GMTYPE_END: 
        default:
                            assert (false); return; // Control should never reach this case.
    }
}


