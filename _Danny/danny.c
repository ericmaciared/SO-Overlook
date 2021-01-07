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
    time_t timeout;
    struct pollfd *pfds;

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
    station.jacksockfd = connectToServer(&data, station, 1);
    if(station.jacksockfd < 0){
        freeConfig(&data);
        exit(EXIT_FAILURE);
    }
    print(CONNECTED_JACK);

    //Connect to Wendy
    station.wendysockfd = connectToServer(&data, station, 0);
    if(station.wendysockfd < 0){
        freeConfig(&data);
        disconnectJack(&station);
        close(station.jacksockfd);
        exit(EXIT_FAILURE);
    }
    print(CONNECTED_WENDY);

    //Scan directory
    pfds = (struct pollfd *) malloc(2 * sizeof(struct pollfd));

    pfds[0].fd = station.jacksockfd;
    pfds[0].events = POLLIN | POLLHUP;
    pfds[0].fd = station.wendysockfd;
    pfds[0].events = POLLIN | POLLHUP;
    while (!finish){
        if (scanDirectory(&data, station) < 0) break;

        timeout = time(NULL);
        while (time(NULL) - timeout <= data.time && !finish){
            if (poll(pfds, 2, 0) >= 0){
                if ((pfds[0].revents & POLLIN) || (pfds[0].revents & POLLHUP) || 
                ((pfds[1].revents & POLLIN) || (pfds[1].revents & POLLHUP))){
                    finish = 1;
                    break;
                }
            }
        }
    }
    
    if (finish) {
        disconnectJack(&station);

    }


    freeConfig(&data);
    print(DISCONNECT_DANNY);

    return 0;
}
