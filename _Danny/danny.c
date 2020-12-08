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
}

int main(int argc, char const *argv[]){
    Data data;
    Station station;
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
    station.name = data.station;
    station.sockfd = connectToJack(&data, station);
    if(station.sockfd < 0){
        exit(EXIT_FAILURE);
    }

    //Scan directory
    while (!finish){
        scanDirectory(&data, sockfd);
        sleep(data.time);
    }
    
    print(DISCONNECT_DANNY);

    freeConfig(&data);

    return 0;
}
