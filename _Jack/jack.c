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

    //temporal solution
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]){
    Config config;
    int sockfd;
    int sockfdClient;

    print(STARTING);

    //Reprogram signals
    signal(SIGINT, ksighandler);
    
    //Check correct arguments
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }
    
    //Fill config file
    if(!processConfig(&config, argv[argc -1])) exit(EXIT_FAILURE);

    //Create socket
    sockfd = initServer(&config);
    
    if (sockfd < 0) exit(EXIT_FAILURE);
    
    sockfdClient = acceptConnection(sockfd);
    if (sockfdClient < 0) exit(EXIT_FAILURE);


    //Free remaining dynamic memory

    return 0;
}
