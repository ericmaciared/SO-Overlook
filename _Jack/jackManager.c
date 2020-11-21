/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "jackManager.h"

//FUNCTIONS
int processConfig(Config* config, const char* file){
    int fd = -1;
    char* buffer;
    
    fd = open(file, O_RDONLY);

    if (fd <= 0) {
        perror(ERROR_FILE);
        return 0;
    }
    else {
        config->ip = readUntil(fd, '\n');
        buffer = readUntil(fd, '\n');
        config->port = atoi(buffer);
    }
    return 1;
}



