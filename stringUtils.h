/* 
 * stringUtils.h
 *
 * holds several useful string utilities
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>

int count_occurences(char* string, char character);

char* move_to_char(char* string, char character);

int index_of(char* string, char character);

char* split_string(char* string, char character);

#endif
