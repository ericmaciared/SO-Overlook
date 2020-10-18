/**
 * Authors: Eric Macià Redono (eric.macia)
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
Data data;


//FUNCTIONS
void ksighandler(int signum){
    switch (signum)
    {
    case SIGINT:
        print(DISCONNECTING);
        //Free memory from data structures
        
        exit(0);
        break;

    default:
        break;
    }
    signal(signum, ksighandler);
}

int main(int argc, char const *argv[]){
    print(STARTING);

    //Reprogram signals
    for (int i = 0; i < 64; i++) signal(i, ksighandler);

    //Process configuration file
    processConfig(&data, argv[argc -1]);



    return 0;
}
