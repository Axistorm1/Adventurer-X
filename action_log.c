#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACTION_MAX_LENGTH 32
#define MAX_STRINGS 10000

int write_char(char* executed_action, char* filename) {
  char* dir = "saves";
  char* extension = ".logs";
  size_t dir_len = strlen(dir);
  size_t filename_len = strlen(filename);
  size_t extension_len = strlen(extension);

  char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return 1;
    }

  strcpy(filename_extended, dir);
  strcat(filename_extended, "/");
  strcat(filename_extended, filename);
  strcat(filename_extended, extension);

  FILE *file = fopen(filename_extended, "a");
  if (file == NULL) {
    printf("File couldn't be opened \n");
    exit(1);
  }
  
  size_t len = strlen(executed_action);
  fwrite(executed_action, 1, len, file);

  fclose(file);
  
  return 1;
}

void store_last_n_lines(int n, char lines[][ACTION_MAX_LENGTH], int *num_lines_stored, char* filename) {
  char* dir = "saves";
  char* extension = ".logs";
  size_t dir_len = strlen(dir);
  size_t filename_len = strlen(filename);
  size_t extension_len = strlen(extension);

  char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        exit(1);
    }

  strcpy(filename_extended, dir);
  strcat(filename_extended, "/");
  strcat(filename_extended, filename);
  strcat(filename_extended, extension);

  FILE *file = fopen(filename_extended, "a");
  if (file == NULL) {
    printf("File couldn't be opened \n");
    exit(1);
  }

  if (file == NULL) {
    printf("File couldn't be opened \n");
  }

  char buffer[ACTION_MAX_LENGTH];
  int count = 0;

  for (int i = 0; i < n; i++) {
    lines[i][0] = '\0';
  }
  
  printf("Reading action logs... \n");
  
  while (fgets(buffer, sizeof(buffer), file)) {
    strcpy(lines[count % n], buffer);
    count++;
  }

  int start = count < n ? 0 : count % n;
  int num_lines_to_store = count < n ? count : n;

  *num_lines_stored = num_lines_to_store;
  
  for (int i = 0; i < num_lines_to_store; i++) {
    strcpy(lines[i], lines[(start + i) % n]);
  }

  fclose(file);
  
}

int load_all_actions(char strings[MAX_STRINGS][ACTION_MAX_LENGTH], char* filename){

  char* dir = "saves";
  char* extension = ".logs";
  size_t dir_len = strlen(dir);
  size_t filename_len = strlen(filename);
  size_t extension_len = strlen(extension);

  char* filename_extended = malloc(dir_len + 1 + filename_len + extension_len + 1);
    if (filename_extended == NULL) {
        perror("Memory allocation for filename failed");
        return 1;
    }

  strcpy(filename_extended, dir);
  strcat(filename_extended, "/");
  strcat(filename_extended, filename);
  strcat(filename_extended, extension);

  FILE *file = fopen(filename_extended, "r");
  if (file == NULL) {
    printf("File couldn't be opened \n");
    exit(1);
  }

    char buffer[ACTION_MAX_LENGTH];
    int count = 0;

    // Read each line from the file
    while (fgets(buffer, ACTION_MAX_LENGTH, file) != NULL) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Copy the string from buffer to the 2D array
        strncpy(strings[count], buffer, ACTION_MAX_LENGTH - 1);
        strings[count][ACTION_MAX_LENGTH - 1] = '\0';  // Ensure null-termination
        count++;

        // Check if we've reached the maximum number of strings
        if (count >= MAX_STRINGS) {
            break;
        }
    }

    fclose(file);
    return count;
}

void print_n_lines(int n, char lines[][ACTION_MAX_LENGTH]) {
    for (int i = 0; i < n; i++) {
      printf("-> %s", lines[i]);
    }
  }
