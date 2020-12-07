/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "protocolManager.h"

//PRIVATE FUNCTIONS
char* frameToString(Frame frame){
    char* final;

    final = (char*) malloc(115 * sizeof(char));

    strcat(final, frame.source);
    memset(&final[strlen(final)], '$', 14 - strlen(frame.source));
    
    final[14] = frame.type;

    strcat(final, frame.data);
    memset(&final[strlen(final)], '$', 100 - strlen(frame.data));

    return final;
}

int checkFrame(char* frame, char type){
    Frame newFrame;

    for (int i = 0; i < 14; i++){
        if (frame[i] == '$'){
            newFrame.source[i] = '\0';
            break;
        }   
        newFrame.source[i] = frame[i];         
    }

    newFrame.type = frame[14];
    for (int i = 15; i < 115; i++){
        newFrame.data[i] = frame[i];
        if (frame[i] == '$'){
            newFrame.data[i] = '\0';
            break;
        }        
    }
        
    if (strcmp(newFrame.source, JACK) != 0) return -1;

    if (newFrame.type != type) return -1;
    
    return 1;
}

//PUBLIC FUNCTIONS
int protocolConnection(int sockfd, char* name){
    
    //Connection Request
    Frame frame;
    strcpy(frame.source, DANNY);
    frame.type = 'C';
    strcpy(frame.data, name);

    char* aux = frameToString(frame);
    write(sockfd, aux, strlen(aux));
    free(aux);

    //Connection Reply
    char buffer[115];

    //Connection Request
    read(sockfd, buffer, 115);

    if (checkFrame(buffer, 'O') > 0) return 0;  
    else return -1;
}
