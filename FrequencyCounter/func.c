#include <string.h>
#include "include/func.h"


char *lowerCase(char *arg) {

        int i;
        i = 0;

        while(i != strlen(arg)) {
                arg[i] = tolower(arg[i]);
                i++;
        }

        return arg;
}

// This function counts the frequency of a given string in the given file.
int freqCount(FILE *source, char *string) {

        int counter;
        counter = 0;
        char buffer[strlen(string) + 1]; // space to hold candidates from the file

        char c;
        while( (c = fgetc(source)) != EOF) { // Get character from file
                if(c == string[0]) { // If starting characters match, begin the processing
                        fseek(source,-1,SEEK_CUR);
                        fgets(buffer,strlen(string)+1,source); // get the candidate string from file
                        if(strcmp(lowerCase(buffer),string) == 0) // If the strings match, increment the counter
                                counter++;
                        fseek(source,-(strlen(string)-1),SEEK_CUR); // Reposition stream position to next character after the matched character

                }
        }


        fseek(source,0,SEEK_SET); // Return stream position indicator to beginning of the file
        return counter;
}

