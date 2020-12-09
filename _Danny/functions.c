/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#include "functions.h"

//FUNCTIONS
void print(char* out){
    write(1, out, strlen(out));
}

char* readUntil(int fd, char delimiter) {
    int i = 0, size;
    char buff = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (1) {
        size = read(fd, &buff, sizeof(char));
        // "Igualada\r\n"
        if (buff != delimiter && size > 0) {
            string[i] = buff;
            string = (char*)realloc(string, sizeof(char) * (++i + 1));
        } else {
            break;
        }
    }
    string[i] = '\0';
    return string;
}