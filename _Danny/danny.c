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
int volatile finish = 0;

//FUNCTIONS
void ksighandler(){
    finish = 1;
}

int main(int argc, char const *argv[]){
    Data data;
    Station station;
    time_t timeout;
    struct pollfd pfd;

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
    pfd.fd = station.sockfd;
    pfd.events = POLLIN | POLLHUP;
    while (!finish){
        if (scanDirectory(&data, station.sockfd) < 0) break;

        timeout = time(NULL);
        while (time(NULL) - timeout <= data.time || finish){
            if (poll(&pfd, 1, 0) >= 0){
                if ((pfd.revents & POLLIN) || (pfd.revents & POLLHUP)){
                    finish = 1;
                    break;
                }
            }
        }
    }
    
    if (finish) disconnectJack(&station);

    freeConfig(&data);
    print(DISCONNECT_DANNY);

    return 0;
}
