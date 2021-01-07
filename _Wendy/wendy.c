/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "wendyManager.h"

//GLOBAL
int finish = 0;
int terminate = 0;

//FUNCTIONS
void ksighandler(){
    finish = 1;
    terminate = 1;

    signal(SIGINT, ksighandler);
}

static void* handleDanny(void* args){
    Station* client = (Station *) args;
    char buffer[64];
    char type = 0;
    struct pollfd pfd;
    
    pfd.fd = client->sockfd;
    pfd.events = POLLIN;

    while (!terminate){
        //Wait for new information on socket
        while (!terminate){
            if (poll(&pfd, 1, 0) >= 0){
                if (pfd.revents & POLLIN){
                    type = readFromDanny(client);
                    break;
                }
            }
        }
        
        //TODO: if new image prepare for new image
        if (type == 'I'){
            /* code */
        }
        
        //TODO: If image info fill image info
        if (type == 'F'){
            /* code */
        }

        //TODO: If finished transmission send reply and post image in Barry


        
        //If disconnection from danny 
        if (type == 'Q') break;
    }

    sprintf(buffer, "Closing %s station.\n", client->name);
    print(buffer);

    //Close client socket
    close(client->sockfd);

    return (void *) client;
}


int main(int argc, char const *argv[]){
    Config config;
    int sockfd;
    Station clients[32];
    pthread_t tid[32];
    int i = 0;
    struct pollfd pfd;

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Check correct arguments
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }

    //Fill config file
    if(!processConfig(&config, argv[argc -1])) exit(EXIT_FAILURE);
    print(STARTING);

    sockfd = initServer(&config);
    if (sockfd < 0) exit(EXIT_FAILURE); 

    //Accept connections and assign threads indefinitely
    pfd.fd = sockfd;
    pfd.events = POLLIN;

    print(PROMPT);
    print(CONNECTION_WAITING);

    while (!finish) {
        if (poll(&pfd, 1, 0) >= 0) {
            if (pfd.revents & POLLIN){
                if (acceptConnection(sockfd, &clients[i]) < 0 || terminate) break;
                if (pthread_create(&tid[i], NULL, handleDanny, &clients[i]) != 0){
                    print(ERROR_THREAD);
                    close(clients[i].sockfd);
                    i--;
                    break;
                }
                else i++;
            } 
        }
    }

    print(DISCONNECTING);

    //TODO: Wait for all threads to join
    for (int j = 0; j < i; j++){
        pthread_join(tid[j], NULL);
    }
    
    //TODO: Free all dynamic data
    free(config.ip);
    for (int j = 0; j < i; j++){
        free(clients[j].name);
    }

    return 0;
}
