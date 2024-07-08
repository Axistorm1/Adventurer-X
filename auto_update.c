#define INSTALLED_VERSION 1.14

#include <stdio.h>
#include <stdlib.h>
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

            printf("Extracted string: %s\n", version);

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
  if (current < uptodate) {
    printf("Game version isn't most recent version \n");
  } else if (current >= uptodate) {
    printf("Game version is most recent version \n");
  }
  
  printf("%.2f & %.2f \n", current, uptodate);
    
  return 1;
}

float get_uptodate_version() {
    setup_download();
    float UPTODATE_VERSION = read_file();
    return UPTODATE_VERSION;
}

void check_up_to_date(){
  float UPTODATE_VERSION;
  UPTODATE_VERSION = get_uptodate_version();
  is_up_to_date(INSTALLED_VERSION, UPTODATE_VERSION);
}
