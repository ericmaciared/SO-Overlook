/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "connectionManager.h"
#include "functions.h"

//FUNCTIONS
char* fill(int fd, char delimiter){
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
}

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
        //Memory for name
        data->station = fill(fd, '\n');

        //Memory for path
        data->path = fill(fd, '\n');

        //Getting time
        count = 0;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            buffer[count++] = ptr;
            read(fd, &ptr, 1);
        }
        buffer[count] = '\0';
        data->time = atoi(buffer);

        //Memory for Jack server IP
        data->jackIP = fill(fd, '\n');

        //Memory for Jack port
        data->jackPort = fill(fd, '\n');

        //Memory for Wendy server IP
        data->wendyIP = fill(fd, '\n');

        //Memory for Wendy port
        data->wendyPort = fill(fd, '\n');

        //Close file
        close(fd);

        /*sprintf(buffer, "Station name is -%s-\n", data->station);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "Path name is -%s-\n", data->path);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "Time is -%d-\n", data->time);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "JackIP is -%s-\n", data->jackIP);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "JackPort is -%s-\n", data->jackPort);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "WendyIP is -%s-\n", data->wendyIP);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "WendyPort is -%s-\n", data->wendyPort);
        write(1, buffer, strlen(buffer));*/

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
    char buffer[128];
    int fd = -1;
    //char ptr;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", filename);
      perror(buffer);
      return;
    }
    else{
      data.dateString = fill(fd,'\n');
      print(data.dateString);
      print(EOL);
      data.hourString = fill(fd,'\n');
      print(data.hourString);
      print(EOL);
      data.temperatureString = fill(fd,'\n');
      print(data.temperatureString);
      print(EOL);
      data.humidityString = fill(fd,'\n');
      print(data.humidityString);
      print(EOL);
      data.pressureString = fill(fd,'\n');
      print(data.pressureString);
      print(EOL);
      data.precipitationString = fill(fd,'\n');
      print(data.precipitationString);
      print(EOL);
      data.temperature = atof(data.temperatureString);
      data.humidity = atoi(data.humidityString);
      data.pressure = atof(data.pressureString);
      data.precipitation = atof(data.precipitationString);

      // Check atoi and atofs, although not optimized
      //printf("Temperature: %f\nHumidity: %d%%\nPressure: %f\nPrecipitation: %f\n", data.temperature, data.humidity, data.pressure, data.precipitation);

      /*
      while(read(fd, &ptr, 1) > 0){
        print(&ptr);
      }
      */

    }
    close(fd);
}

void scanDirectory(Data* data){
    DIR* d;
    char buffer[32];
    struct dirent* dir;
    char** files;
    char* path;
    int num_files = 0;

    files = (char**) malloc(sizeof(char*));

    //check for new files
    sprintf(buffer, "/$%s:\n", data->station);
    print(buffer);
    print(TESTING);

    sprintf(buffer, ".%s", data->path);
    d = opendir(buffer);
     if (d){
        while ((dir = readdir(d)) != NULL){
            files[num_files++] = (char*) malloc(strlen(dir->d_name) * sizeof(char));
            files[num_files - 1] = dir->d_name;
            files = realloc(files, (num_files + 1) * sizeof(char*));
        }
        closedir(d);

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
            for (int i = 2; i < num_files; i++)
            {
                switch (checkExtension(files[i]))
                {
                case TXT:
                    print(EOL);
                    print(files[i]);
                    print(EOL);
                    // Copyting the Directory
                    path = (char*) malloc((strlen(data->path)+1) * sizeof(char));
                    strcpy(path, data->path);
                    //Removint the first '/'
                    memmove(path, path+1, strlen(path));
                    //Adding '/'
                    path[strlen(data->path)-1] = '/';
                    path[strlen(data->path)] = '\0';
                    //Concatenate filename
                    char* fullpath = strcat(path, files[i]);
                    showFile(fullpath);
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
        for (int i = 0; i < num_files -1; i++)
        {
            //free(files[i]);
        }
        free(files);
        print("Files free\n");


    }

    print(EOL);
}
