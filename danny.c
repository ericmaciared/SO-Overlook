/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "connectionManager.h"


//DEFINES
#define EOL "\n"
#define STARTING "\nStarting Danny...\n\n"
#define DISCONNECTING "\nDisconnecting Danny...\n\n"


//GLOBAL
int finish = 0;


//FUNCTIONS
void ksighandler(int signum){
    switch (signum)
    {
    case SIGINT:
        print(DISCONNECTING);
        finish=1;
	break;

    default:
        break;
    }
    signal(signum, ksighandler);
}

int main(int argc, char const *argv[]){
    Data data;

    print(STARTING);

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Fill danny config file
    processConfig(&data, argv[argc -1]);

    do{
        //scan directory and sleep 
        scanDirectory(&data);
        sleep(data.time);
    } while (!finish);

    //Free remaining dynamic memory from danny config
    freeConfig(&data);

    return 0;
}
