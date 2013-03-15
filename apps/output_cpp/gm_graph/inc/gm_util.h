#ifndef GM_UTIL_H_
#define GM_UTIL_H_

#include <stdint.h>
#include <string>
#include <fstream>
#include "gm_internal.h"
#include "gm_file_handling.h"

/*
 * A class to tokenize a string.
 * The delimiters could be any of the characters in the delim string.
 * This implementation uses the methods in the C++ string class,
 * specifically, find_first_not_of, find_first_of, and substr.
 */
class GM_Tokenizer {
  public:
    GM_Tokenizer(std::string s, std::string d) : str_(s), delim_(d) { reset(); }
    GM_Tokenizer(const char *s, std::string d) : str_(s), delim_(d) { reset(); }
    GM_Tokenizer(std::string s, const char *d) : str_(s), delim_(d) { reset(); }
    GM_Tokenizer(const char *s, const char *d) : str_(s), delim_(d) { reset(); }

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

/*
 * Methods to manipulate arrays based on different value types.
 */
void *gmutil_getArrayType(VALUE_TYPE vt, int size);
int  gmutil_getSizeOfType(VALUE_TYPE vt);
void loadValueBasedOnType(void *arr, long pos, std::string val, VALUE_TYPE vt);
void storeValueBasedOnType(void *arr, long pos, std::ofstream& file, VALUE_TYPE vt);
void storeValueBasedOnType(void *arr, long pos, GM_Writer& writer, VALUE_TYPE vt);

void* gmutil_createVectorType(VALUE_TYPE vt); 
void  gmutil_deleteVectorType(void* vector, VALUE_TYPE vt) ;
void  gmutil_loadValueIntoVector(void *vector, std::string val, VALUE_TYPE vt); 
void  gmutil_loadDummyValueIntoVector(void *vector, VALUE_TYPE vt); 
const char* gmutil_getTypeString(VALUE_TYPE v);
//void* gmutil_getArrayFromVector(void* vector, VALUE_TYPE vt);
void  gmutil_copyVectorIntoArray(void* vector, void* array, VALUE_TYPE rt, edge_t* indirection=NULL);


#endif /* GM_UTIL_H_ */
