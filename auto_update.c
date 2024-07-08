#define INSTALLED_VERSION 1.16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fetch_file.c"

float get_installed_version() {
  return INSTALLED_VERSION;
}

float read_file() { 
    FILE *file = fopen("up_to_date.txt", "r");

    if (file == NULL) {
        printf("Error opening file\n");
        return 0.0f;
    }

    char buffer[256];
    char version[5] = {0};  // Initialize to zero to avoid uninitialized data
    float UPTODATE_VERSION = 0.0f;

    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Check that we have enough characters in the buffer
        if (strlen(buffer) >= 30) {
            // Extract the version string (assumes the version starts at index 26)
            snprintf(version, sizeof(version), "%.4s", &buffer[26]);  // Copy 4 characters from buffer[26]

            // Remove any newline character if present
            size_t len = strlen(version);
            if (len > 0 && version[len - 1] == '\n') {
                version[len - 1] = '\0';
            }

            // Convert the version string to float
            UPTODATE_VERSION = strtof(version, NULL);
        } else {
            printf("Buffer does not have enough data\n");
        }
    } else {
        printf("Error reading file\n");
    }

    fclose(file);
    return UPTODATE_VERSION;
}


int is_up_to_date(float current, float uptodate) {

  printf("current = %.2f \t updated =  %.2f \n", current, uptodate);
  
  if (current < uptodate) {
    printf("Game version isn't most recent version \n");
    return 0;
  } else if (current >= uptodate) {
    printf("Game version is most recent version \n");
    return 1;
  }

  return 0;
}

float get_uptodate_version() {
    setup_download();
    float UPTODATE_VERSION = read_file();
    return UPTODATE_VERSION;
}

int download_updated_files(){

  const char* url_1 = "https://raw.githubusercontent.com/AxistormDuBled/Adventurer-X/main/auto_update.c";
  const char* file_1 = "auto_update_1.c";

  downloadFile(url_1, file_1);
  printf("Downloaded successfully");

}

void check_up_to_date(){
  float UPTODATE_VERSION;
  char answer;
  
  UPTODATE_VERSION = get_uptodate_version();
  system("del up_to_date.txt");

  if (is_up_to_date(INSTALLED_VERSION, UPTODATE_VERSION) == 0) {
    
    printf("Would you like to update the game?");
    scanf(" %c", &answer);

    while(getchar() != '\n');

    printf("%c \n", answer);
      
    if (answer == 'y') {
      download_updated_files();
      printf("Updated files");
    } else {
      printf("Didn't update");
    }
  }
}