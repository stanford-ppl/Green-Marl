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
        case GMTYPE_END: assert(false); return NULL; // Control should never reach this case.
    }
    return NULL;
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
        case GMTYPE_END: assert(false); return; // Control should never reach this case.
    }
}

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
        case GMTYPE_END: assert (false); return; // Control should never reach this case.
    }
}

