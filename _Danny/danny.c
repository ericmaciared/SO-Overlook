/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "dannyManager.h"


//DEFINES

//GLOBAL
int finish = 0;

//FUNCTIONS
void ksighandler(){
    finish = 1;

    //temporal
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]){
    Data data;
    int sockfd = -1;

    print(STARTING);

    //Activate signal interruption
    signal(SIGINT, ksighandler);

    //Check correct arguments
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }

    //Process configuration file
    if(!processConfig(&data, argv[argc -1])){
        exit(EXIT_FAILURE);
    }

    //Connect to Jack
    sockfd = connectToJack(&data);
    if(sockfd < 0){
        exit(EXIT_FAILURE);
    }

    //Scan directory
    scanDirectory(&data, sockfd);

    print(DISCONNECT_DANNY);
    freeConfig(&data);

    return 0;
}
