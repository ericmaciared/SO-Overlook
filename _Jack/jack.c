/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "jackManager.h"

//GLOBAL
int finish = 0;
int terminate = 0;

//FUNCTIONS
static void* handleDanny(void* args){
    Station* client = (Station *) args;
    char buffer[64];
    char type = 0;

    while (!terminate){
        type = readFromDanny(client);

        //printf("Type value = -%c-\n", type);

        if (type == 'Q' || type == 'X') break;
        else replyToDanny(client, type);

        //sleep(1);
    }

    sprintf(buffer, "Closing %s station.\n", client->name);
    print(buffer);

    //Close client socket
    close(client->sockfd);

    return (void *) client;
}

void ksighandler(){
    finish = 1;
    terminate = 1;

    printf("Executing termination\n");
    signal(SIGINT, ksighandler);
}

int main(int argc, char const *argv[]){
    Config config;
    int sockfd;

    Station client;
    pthread_t tid[32];
    int i = 0;

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

    //Create socket
    sockfd = initServer(&config);
    if (sockfd < 0) exit(EXIT_FAILURE);

    //Accept connections and assign threads indefinitely
    while (!finish) {
        if (acceptConnection(sockfd, &client) < 0) break;

        if (pthread_create(&tid[i++], NULL, handleDanny, &client) != 0){
            print(ERROR_THREAD);
            i--;
            close(client.sockfd);
            break;
        }
        //sleep(5);
    }

    print(DISCONNECTING);

    //TODO: Wait for all threads to join
    for (int j = 0; j < i; j++){
        pthread_join(tid[j], NULL);
    }
    
    printf("All threads returned\n");
    //TODO: Free all dynamic data

    return 0;
}
