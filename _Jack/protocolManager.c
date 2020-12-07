/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "protocolManager.h"

//PRIVATE FUNCTIONS
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
        
    if (strcmp(newFrame.source, DANNY) != 0) return -1;

    if (newFrame.type != type) return -1;
    
    return 1;
}

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

//PUBLIC FUNCTIONS
int protocolConnection(int sockfd, int sockfdclient){
    //Frame frame;
    char buffer[115];

    //Connection Request
    read(sockfdclient, buffer, 115);
    
    if (checkFrame(buffer, 'C') > 0){
        Frame frame;
        char* buff;

        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, OK);

        buff = frameToString(frame);
        write(sockfdclient, buff, strlen(buff));
        free(buff);

        return 0;  
    }
    else{

        Frame frame;
        char* buff;

        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, ERROR);

        buff = frameToString(frame);
        write(sockfdclient, buff, strlen(buff));
        free(buff);
        return -1;
    }

    printf("%d\n", sockfd);
}
