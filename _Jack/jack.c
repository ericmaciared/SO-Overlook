/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "jackManager.h"

//GLOBAL
int finish = 0;

//FUNCTIONS
void ksighandler(){
    //Disconnect connections
    finish = 1;
}

int main(int argc, char const *argv[]){
    Config config;

    print(STARTING);

    //Reprogram signals
    signal(SIGINT, ksighandler);
    
    //Fill config file
    if(!processConfig(&config, argv[argc -1])){
        exit(EXIT_FAILURE);
    }
    print(config.ip);
    print(EOL);
    printf("-%d-\n", config.port);



    //Free remaining dynamic memory

    return 0;
}
