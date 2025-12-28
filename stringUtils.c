/* 
 * stringUtils.h
 *
 * holds several useful string utilities
 */

#include <string.h>


// counts the number of occurences of the specified character in the string
int count_occurences(char* string, char character) {
    int count = 0;
    for (size_t i=0; i<strlen(string); i++) {
        if (string[i] == character) {
            count++;
        }
    }
    return count;
}


/* returns a new string pointer that starts 1 chracter after the specified
 * character. Note that this function assumes that the specified character is
 * in the string.
 */
char* move_to_char(char* string, char character) {
    while (string[0] != character) {
        string++;
    }
    string++;
    return string;
}


/* returns the index of the string containing the specified character. This
 * function assumes the character is present somewhere in the string
 */
int index_of(char* string, char character) {
    int i = 0;
    while (string[i] != character) {
        i++;
    }
    return i;
}


/* partitions the given string so that it is split at the specified character.
 * this functiton returns a pointer to the second half of the partition
 */
char* split_string(char* string, char character) {
    while (string[0] != character) {
        string++;
    }
    string[0] = '\0';
    string++;
    return string;
}

