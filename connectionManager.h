/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _CONNECTIONMANAGER_H_
#define _CONNECTIONMANAGER_H_

//LIBRARIES
#include <fcntl.h> 
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


//DEFINES
#define EOL "\n"


//STRUCTS
typedef struct Data{
  char* station;
  char* path;
  int time;
  char* jackIP;
  char* jackPort;
  char* wendyIP;
  char* wendyPort;
}Data;

//FUNCTIONS
void processConfig(Data* data, const char* file);

void fill(char** target, int fd, char delimiter);

void freeConfig(Data* data);


#endif