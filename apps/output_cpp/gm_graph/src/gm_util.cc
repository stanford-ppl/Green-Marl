#include "gm_util.h"
#include <assert.h>

/*
 * This is an implementation of the Tokenizer class.
 * This implementation uses the methods in the C++ string class;
 * specifically, find_first_not_of, find_first_of, and substr.
 */

void Tokenizer::reset() {
    current_pos_ = 0;
    next_token_start_ = 0;
    found_next_token_start_ = false;
    number_of_tokens_ = 0;
}

void Tokenizer::findNextTokenStart() {
    if ( ! found_next_token_start_) {
        next_token_start_ = str_.find_first_not_of(delim_, current_pos_);
        found_next_token_start_ = true;
    }
}

std::string Tokenizer::getNextToken() {
    if ( ! found_next_token_start_) {
        findNextTokenStart();
    }
    size_t next_token_end = str_.find_first_of(delim_, next_token_start_);
    current_pos_ = next_token_end; // setting beyond the next token
    found_next_token_start_ = false;
    return str_.substr(next_token_start_, next_token_end - next_token_start_);
}

bool Tokenizer::hasNextToken() {
    if ( ! found_next_token_start_) {
        findNextTokenStart();
    }
    return next_token_start_ != std::string::npos;
}

long Tokenizer::countNumberOfTokens() {
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
 * Method to write to a given location in an array based on the given value type.
 * The value to be written is extracted from the given string 'val'.
 */
void storeValueBasedOnType(void *arr, long pos, std::string val, VALUE_TYPE vt) {
    switch(vt) {
        case GMTYPE_BOOL: ((bool *)arr)[pos] = (val == "true"); break;
        case GMTYPE_INT: ((int *)arr)[pos] = atoi(val.c_str()); break;
        case GMTYPE_LONG: ((long *)arr)[pos] = atol(val.c_str()); break;
        case GMTYPE_FLOAT: ((float *)arr)[pos] = strtof(val.c_str(), NULL); break;
        case GMTYPE_DOUBLE: ((double *)arr)[pos] = strtod(val.c_str(), NULL); break;
        case GMTYPE_END: assert(false); return; // Control should never reach this case.
    }
}


