/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "dannyManager.h"

//FUNCTIONS
/*char* fill(int fd, char delimiter){
    char ptr;
    char* target = NULL;
    int count = 0;

    target = (char*) malloc(sizeof(char));
    count++;
    read(fd, &ptr, 1);
    while(ptr != delimiter){
        target[count-1] = ptr;
        target = realloc(target, (++count)*sizeof(char));
        read(fd, &ptr, 1);
    }
    target[count - 2] = '\0';
    return target;
}*/

void processConfig(Data* data, const char* file){
    char buffer[128];
    char ptr;
    int fd = -1;
    int count = 0;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", file);
      perror(buffer);
      raise(SIGINT);
    }
    else{
        //Fill memory for each section of config file
        data->station = readUntil(fd, '\n');
        data->path = readUntil(fd, '\n');

        //Fill memory for time
        count = 0;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            buffer[count++] = ptr;
            read(fd, &ptr, 1);
        }
        buffer[count] = '\0';
        data->time = atoi(buffer);

        data->jackIP = readUntil(fd, '\n');
        data->jackPort = readUntil(fd, '\n');
        data->wendyIP = readUntil(fd, '\n');
        data->wendyPort = readUntil(fd, '\n');

        //Close file
        close(fd);
    }
}

void freeConfig(Data* data){
    free(data->station);
    free(data->path);
    free(data->jackIP);
    free(data->jackPort);
    free(data->wendyIP);
    free(data->wendyPort);

    data->station = NULL;
    data->path = NULL;
    data->jackIP = NULL;
    data->jackPort = NULL;
    data->wendyIP = NULL;
    data->wendyPort = NULL;
}

int checkExtension(char* filename){
    int code = NOFILE;
    char end[4];

    if (strlen(filename) < 3) return code;

    end[0] = filename[strlen(filename) - 3];
    end[1] = filename[strlen(filename) - 2];
    end[2] = filename[strlen(filename) - 1];
    end[3] = '\0';

    if (!strcmp(end, "txt")) code = TXT;
    if (!strcmp(end, "jpg")) code = JPG;

    return code;
}

void showFile(char* filename){
    StationData data;
    char buffer[32];
    int fd = -1;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", filename);
      perror(buffer);
      return;
    }
    else {
        data.dateString = readUntil(fd,'\n');
        print(data.dateString);
        print(EOL);
        data.hourString = readUntil(fd,'\n');
        print(data.hourString);
        print(EOL);
        data.temperatureString = readUntil(fd,'\n');
        print(data.temperatureString);
        print(EOL);
        data.humidityString = readUntil(fd,'\n');
        print(data.humidityString);
        print(EOL);
        data.pressureString = readUntil(fd,'\n');
        print(data.pressureString);
        print(EOL);
        data.precipitationString = readUntil(fd,'\n');
        print(data.precipitationString);
        print(EOL);
        data.temperature = atof(data.temperatureString);
        data.humidity = atoi(data.humidityString);
        data.pressure = atof(data.pressureString);
        data.precipitation = atof(data.precipitationString);

        free(data.dateString);
        free(data.hourString);
        free(data.temperatureString);
        free(data.humidityString);
        free(data.pressureString);
        free(data.precipitationString);
    }
    close(fd);
}

void scanDirectory(Data* data){
    DIR* d;
    char buffer[32];
    struct dirent* dir = NULL;
    char** files = NULL;
    int num_files = 0;

    files = (char**) malloc(sizeof(char*));

    //check for new files
    sprintf(buffer, "$%s:\n", data->station);
    print(buffer);
    print(TESTING);

    sprintf(buffer, ".%s", data->path);
    d = opendir(buffer);
    if (d){
        while ((dir = readdir(d)) != NULL){
            files[num_files++] = (char*) malloc((strlen(dir->d_name) + 1) * sizeof(char));
            strcpy(files[num_files - 1], dir->d_name);
            files = realloc(files, (num_files + 1) * sizeof(char*));
        } 
        closedir(d);
        
        print("Here!\n");

        //Show directory scan file results
        if (num_files <= 2) print(NO_FILES);
        else {
            //We ignore the . and .. file accesses
            sprintf(buffer, FILES_FOUND, num_files - 2);
            print(buffer);
            for (int i = 2; i < num_files; i++){
                print(files[i]);
                print(EOL);
            }

            //Process files
            for (int i = 2; i < num_files; i++){
                switch (checkExtension(files[i])){
                    case TXT:
                        print(EOL);
                        print(files[i]);
                        print(EOL);

                        sprintf(buffer, ".%s/%s", data->path, files[i]);             
                        showFile(buffer);

                        //remove(buffer);
                        
                        break;

                    case JPG:
                        //send file to server

                        break;

                    default:
                        break;
                }
            }
        }

        //Free remaining dynamic memory
        for (int i = 0; i < num_files; i++)
        {
            free(files[i]);
        }
        free(files);
    }

    print(EOL);
}
