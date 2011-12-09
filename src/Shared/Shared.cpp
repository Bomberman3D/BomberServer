/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <iostream> // Because of standart (std) functions
#include <string.h>
#include "Shared/Shared.h"

/*
 mstrcmp

 Compares two strings and returns value depending on similarity

 Input:
  pattern   pattern string which is used as major string
  input     string in which pattern presence is checked

 Return values:
  -1        pattern and input doesn't have any similarity (apple and orange)
            also used as default value
  0         pattern and input are the same                (apple and apple)
  1         pattern is present at input beginning         (apple and apple is green)
  2         pattern is present at input ending            (apple and i like apple)
  3         pattern is present somewhere in input         (apple and i like apple from grandpa)
*/
int mstrcmp(const char* pattern, const char* input)
{
    // For cases when both pointers points at the same piece of memory
    if (pattern == input)
        return 0;

    // Init with default value
    int similarity = -1;
    // Store parameters length
    unsigned int pattern_len = strlen(pattern);
    unsigned int input_len   = strlen(input);

    for (unsigned int i = 0; i < input_len; i++)
    {
        if (pattern[0] == input[i])
        {
            similarity = 0;
            // We can use 1 because character at position 0 was checked before
            for (unsigned int j = 1; j < pattern_len; j++)
            {
                if (pattern[j] != input[i+j])
                {
                    similarity = -1;
                    break;
                }
            }
            if (similarity != -1)
            {
                // Same length => same strings
                if (pattern_len == input_len)
                    similarity = 0;
                // If we are at first input character
                else if (i == 0)
                    similarity = 1;
                // If we are at the end of string (one char after last non-similar character)
                else if (i == input_len-pattern_len)
                    similarity = 2;
                // Any other case => somewhere in input
                else
                    similarity = 3;

                return similarity;
            }
        }
    }
    return similarity;
}
