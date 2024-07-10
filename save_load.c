#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>

// Platform-specific includes for mkdir
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#define MAX_BUFFER_SIZE 1024

int chunk_size_x = 8, chunk_size_y = 8;

int save_world_size(int world_size_x, int world_size_y, char* filename){
    char* dir = "saves";
    char* extension = ".size";
    size_t dir_len = strlen(dir);
    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(extension);

    // Allocate memory for the full path
    char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return 1;
    }

    strcpy(filename_extended, dir);
    strcat(filename_extended, "/");
    strcat(filename_extended, filename);
    strcat(filename_extended, extension);

    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        #ifdef _WIN32
        if (_mkdir(dir) != 0) {
        #else
        if (mkdir(dir, 0755) != 0) {
        #endif
            perror("Failed to create directory");
            free(filename_extended);
            return 1;
        }
    }

    FILE *file = fopen(filename_extended, "a");
    if (file == NULL) {
        perror("Failed to open file");
        free(filename_extended);
        return 1;
    }

    fprintf(file, "%d ", world_size_x);
    fprintf(file, "%d ", world_size_y);

    fclose(file);
    free(filename_extended);

    return 0;
}

int load_world_size(int* world_size_x, int* world_size_y, char* filename){
    char* dir = "saves";
    char* extension = ".size";
    size_t dir_len = strlen(dir);
    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(extension);

    char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return 0;
    }

    strcpy(filename_extended, dir);
    strcat(filename_extended, "/");
    strcat(filename_extended, filename);
    strcat(filename_extended, extension);

    FILE* file = fopen(filename_extended, "r");
    if (file == NULL) {
        //perror("Failed to open file");
        free(filename_extended);
        return 4;
    }

    if (fscanf(file, "%d", world_size_x) != 1) {
        fprintf(stderr, "Error reading data");
        fclose(file);
        free(filename_extended);
        return 0;
    }

    if (fscanf(file, "%d", world_size_y) != 1) {
        fprintf(stderr, "Error reading data");
        fclose(file);
        free(filename_extended);
        return 0;
    }

    fclose(file);
    free(filename_extended);
    return 1;
}

int save_chunk(int** chunk, char* filename) {
    // Create the /saves/ directory path and append the filename with ".save"
    char* dir = "saves";
    char* extension = ".save";
    size_t dir_len = strlen(dir);
    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(extension);

    // Allocate memory for the full path
    char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return 1;
    }

    // Build the full path
    strcpy(filename_extended, dir);
    strcat(filename_extended, "/");
    strcat(filename_extended, filename);
    strcat(filename_extended, extension);


    // Create the directory if it doesn't exist
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        #ifdef _WIN32
        if (_mkdir(dir) != 0) {
        #else
        if (mkdir(dir, 0755) != 0) {
        #endif
            perror("Failed to create directory");
            free(filename_extended);
            return 1;
        }
    }

    FILE *file = fopen(filename_extended, "a");
    if (file == NULL) {
        perror("Failed to open file");
        free(filename_extended);
        return 1;
    }

    for (int i = 0; i < chunk_size_x; i++) {
        for (int j = 0; j < chunk_size_y; j++) {
            fprintf(file, "%d ", chunk[i][j]);
        }
    }

    fclose(file);
    free(filename_extended);

    return 0;
}

int** load_chunk_from_position(char* filename, long position) {
    // Allocate memory for the 2D array
    int** chunk = malloc(chunk_size_x * sizeof(int*));
    if (chunk == NULL) {
        perror("Memory allocation for chunk failed");
        return NULL;
    }
    for (int i = 0; i < chunk_size_x; i++) {
        chunk[i] = malloc(chunk_size_y * sizeof(int));
        if (chunk[i] == NULL) {
            perror("Memory allocation for chunk row failed");
            for (int j = 0; j < i; j++) {
                free(chunk[j]);
            }
            free(chunk);
            return NULL;
        }
    }

    // Construct the full path to the file
    char* dir = "saves";
    char* extension = ".save";
    size_t dir_len = strlen(dir);
    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(extension);

    char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return NULL;
    }

    // Build the full path
    strcpy(filename_extended, dir);
    strcat(filename_extended, "/");
    strcat(filename_extended, filename);
    strcat(filename_extended, extension);

    // Open the file for reading
    FILE* file = fopen(filename_extended, "r");
    if (file == NULL) {
        perror("Failed to open file");
        free(filename_extended);
        // Free allocated memory for the chunk before returning
        for (int i = 0; i < chunk_size_x; i++) {
            free(chunk[i]);
        }
        free(chunk);
        return NULL;
    }

    // Move the file position indicator to the specified location
    if (fseek(file, position, SEEK_SET) != 0) {
        perror("Failed to seek to position");
        fclose(file);
        free(filename_extended);
        // Free allocated memory for the chunk before returning
        for (int i = 0; i < chunk_size_x; i++) {
            free(chunk[i]);
        }
        free(chunk);
        return NULL;
    }

    // Read the file contents into the 2D array
    for (int i = 0; i < chunk_size_x; i++) {
        for (int j = 0; j < chunk_size_y; j++) {
            if (fscanf(file, "%d", &chunk[i][j]) != 1) {
                fprintf(stderr, "Error reading data at position [%d][%d]\n", i, j);
                fclose(file);
                free(filename_extended);
                // Free allocated memory for the chunk before returning
                for (int k = 0; k <= i; k++) {
                    free(chunk[k]);
                }
                free(chunk);
                return NULL;
            
            }
        }
    }

    fclose(file);
    free(filename_extended);
    return chunk;
}


char **getFilesWithExtension(const char *path, const char *extension, int *fileCount) {
    struct dirent *entry;
    DIR *dp = opendir(path);
    char **fileNames = NULL;
    int count = 0;
    size_t extLength = strlen(extension);

    if (dp == NULL) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dp))) {
            size_t len = strlen(entry->d_name);
            if (len > extLength && strcmp(entry->d_name + len - extLength, extension) == 0) {
                // Allocate memory for base filename without extension
                char *baseName = malloc((len - extLength + 1) * sizeof(char));
                strncpy(baseName, entry->d_name, len - extLength);
                baseName[len - extLength] = '\0';

                count++;
                fileNames = realloc(fileNames, count * sizeof(char *));
                fileNames[count - 1] = baseName;
            }
    }

    closedir(dp);
    *fileCount = count;
    return fileNames;
}

void freeFileNames(char **fileNames, int fileCount) {
    for (int i = 0; i < fileCount; i++) {
        free(fileNames[i]);
    }
    free(fileNames);
}