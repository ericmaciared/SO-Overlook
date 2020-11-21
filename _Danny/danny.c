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
    finish=1;
}

int main(int argc, char const *argv[]){
    Data data;
        
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
    

    //Scan directory and sleep
    do{
        scanDirectory(&data); 
        sleep(data.time);
    } while (!finish);

    print(DISCONNECT_DANNY);
    freeConfig(&data);

    return 0;
}
