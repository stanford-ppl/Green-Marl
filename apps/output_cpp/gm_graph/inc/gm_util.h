#ifndef GM_UTIL_H_
#define GM_UTIL_H_

#include <string>

/*
 * A class to tokenize a string.
 * The delimiters could be any of the characters in the delim string.
 * This implementation uses the methods in the C++ string class,
 * specifically, find_first_not_of, find_first_of, and substr.
 */
class Tokenizer {
  public:
    Tokenizer(std::string s, std::string d) : str_(s), delim_(d) { reset(); }
    Tokenizer(const char *s, std::string d) : str_(s), delim_(d) { reset(); }
    Tokenizer(std::string s, const char *d) : str_(s), delim_(d) { reset(); }
    Tokenizer(const char *s, const char *d) : str_(s), delim_(d) { reset(); }

    void setString (std::string s) { str_ = s; reset(); }
    void setDelimiter (std::string d) { delim_ = d; reset(); }
    std::string getString () { return str_; }
    std::string getDelimiter () { return delim_; }

    void reset();
    void findNextTokenStart();
    std::string getNextToken();
    bool hasNextToken();
    long countNumberOfTokens();

  private:
    std::string str_;   // the string to be parsed
    std::string delim_; // the string whose characters are all considered delimiters
    size_t current_pos_;// the current position of a moving pointer to the characters in _str.
                        // This pointer moves as and when tokens are parsed
    size_t next_token_start_; // the position in _str where the next token will start.
    bool found_next_token_start_; // flag to indicate if the next token start has been found
    long number_of_tokens_; // the number of tokens in the given string with the given delimiter
};

#endif /* GM_UTIL_H_ */
