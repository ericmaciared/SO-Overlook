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

    signal(SIGINT, ksighandler);
        
    print(STARTING);
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }
    
    if(!processConfig(&data, argv[argc -1])){
        exit(EXIT_FAILURE);
    }

    //Connect to Jack
    

    do{
        scanDirectory(&data); 
        sleep(data.time);
    } while (!finish);

    print(DISCONNECT_DANNY);
    freeConfig(&data);

    return 0;
}
