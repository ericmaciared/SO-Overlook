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
#define TESTING "Testing...\n"


//GLOBAL
int finish = 0;


//FUNCTIONS
void ksighandler(int signum){
    switch (signum)
    {
    case SIGINT:
        print(DISCONNECTING);
        break;

    default:
        break;
    }
    signal(signum, ksighandler);
}


int main(int argc, char const *argv[]){
    Data data;
    char buffer[64];
    //int sockfd = -1;

    print(STARTING);

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Process configuration file
    processConfig(&data, argv[argc -1]);

    while (!finish){
        //check for new files
        sprintf(buffer, "/$%s:\n", data.station);
        print(buffer);
        print(TESTING);
        //sleep data time
        sleep(data.time);
        
    }
    
    

    

    return 0;
}
