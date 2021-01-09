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
    char buffer[128];
    char type = 0;
    struct pollfd pfd;

    char* imageName = NULL;
    char imageLocation[128];
    int imageSize = 0;
    char* md5sum = NULL;
    int framesToProcess = -1;

    int imagefd = -1;

    imageName = (char*) malloc(31 * sizeof(char));
    md5sum = (char *) malloc(33 * sizeof(char));

    pfd.fd = client->sockfd;
    pfd.events = POLLIN;

    while (!terminate){
        //Wait for new information on socket
        while (!terminate){
            if (poll(&pfd, 1, 0) >= 0){
                if (pfd.revents & POLLIN){
                    type = readFromDanny(client, buffer);
                    break;
                }
            }
        }
        
        //if new image prepare for new image
        if (type == 'I'){
            imageSize = dataToImageData(buffer, imageName, md5sum);
            framesToProcess = imageSize;

            //Create image if not existent at barry directory
            sprintf(imageLocation, "./Barry/%s", imageName);
            imagefd = open(imageLocation, O_WRONLY | O_CREAT, 0777);
            close(imagefd);
        }
        
        //TODO: If image info fill image info
        if (type == 'F'){
            //Open image in append mode
            print(imageLocation);
            print(EOL);
            if((imagefd = open(imageLocation, O_WRONLY | O_APPEND)) < 0){
                print("Can't open image.\n");
            }

            if (framesToProcess < 99) write(imagefd, buffer, framesToProcess);
            else write(imagefd, buffer, 99);
            
            close(imagefd);
            
            //sprintf(bufferout, "Bytes remaining: -%d-\n", framesToProcess);
            //write(1, bufferout, strlen(bufferout));
            
            framesToProcess-=99;

            //If finished transmission send reply and post image in Barry
            if (framesToProcess <= 0){
                //print("Receiving image data and checking data integrity\n");
                
                //Check md5sum
                sprintf(buffer, "./Barry/%s", imageName);
                if(checkMD5SUM(md5sum, buffer) == 0){
                    //Send Reply
                    print(imageName);
                    print(EOL);

                    replyToDanny(client, 'S');
                }
                else {
                    //Send Reply
                    replyToDanny(client, 'R');

                    //Delete file
                    remove(imageLocation);
                }
                
                //Reset variables

                framesToProcess = -1;
                imageSize = 0;
                bzero(imageLocation, 0);
                bzero(imageName, 0);
                bzero(md5sum, 0);
            }
        }
        
        //If disconnection from danny 
        if (type == 'Q') break;
    }

    free(imageName);
    free(md5sum);

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

    print("All threads returned\n");

    return 0;
}
