/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "dannyManager.h"


//DEFINES
#define EOL "\n"
#define STARTING "\nStarting Danny...\n\n"
#define DISCONNECTING "\nDisconnecting Danny...\n\n"


//GLOBAL
int finish = 0;

//FUNCTIONS
void ksighandler(){
    print(DISCONNECTING);
    finish=1;
    signal(SIGINT, ksighandler);
}

int main(int argc, char const *argv[]){
    Data data;

    signal(SIGINT, ksighandler);
        
    print(STARTING);
    processConfig(&data, argv[argc -1]);

    do{
        scanDirectory(&data); 
        sleep(data.time);
    } while (!finish);

    freeConfig(&data);

    return 0;
}
