#include "gm_util.h"

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

