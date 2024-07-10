#define INSTALLED_VERSION 1.44
char UPDATE_CONTENT[] = {"\t1.29 -> Place blocks \n\t"
"1.30 -> Actions log \n\t"
"1.31 -> Actions as strings \n\t"
"1.32 -> Auto updater downloads numbered version \n\t"
"1.33 -> Eating failures explained \n\t"
"1.35 -> Custom keybinds \n\t"
"1.36 -> Option menu overhaul \n\t"
"1.37 -> Start menu changes \n\t"
"1.38 -> Created offline mode, debug-tool only \n\t"
"1.39 -> Added world save and world load, action log isn't added to the world save yet \n\t"
"1.40 -> Action log plays when file loaded, nice cinematic, custom keybinds might break everything \n\t"
"1.41 -> Custom keybinds translated into action log, won't break anything \n\t"
"1.42 -> Added 'back' in option menus + minor changes \n\t"
"1.43 -> Optimized display function \n\t"
"       Fixed bug when loading inexistent file \n\t"
"       Fixed place in cinematic world + place_block option \n\t"
"1.44 -> Added credits\n\t"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
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

  printf("\n");
  
  printf("Current version = %.2f \t Updated version = %.2f \n \n", current, uptodate);
  printf("What's new ? \n%s \n \n", UPDATE_CONTENT);
  
  if (current < uptodate) {
    printf("Game version doesn't match most recent version \n");
    return 0;
  } else if (current >= uptodate) {
    printf("The game is up to date \n");
    return 1;
  }

  return 0;
}

float get_uptodate_version() {
    setup_download();
    float UPTODATE_VERSION = read_file();
    return UPTODATE_VERSION;
}

int download_updated_files(float uptodate){

  const char* url = "https://raw.githubusercontent.com/AxistormDuBled/Adventurer-X/main/Adventurer-X.exe";
  const char* file = "Adventurer-X";
  const char* extension = ".exe";

  double double_version = uptodate;
  char buffer[10];
  sprintf(buffer, "%.2f", double_version);
  const char* installed_version = buffer;
  
  size_t base_len = strlen(file);
  size_t version_len = strlen(installed_version);
  size_t extension_len = strlen(extension);

  char* filename = (char*)malloc(base_len + version_len + extension_len + 1); // +1 for null terminator
    if (filename == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

  strcpy(filename, file);
  strcpy(filename + base_len, installed_version);
  strcpy(filename + base_len + version_len, extension);

  downloadFile(url, filename);
  printf("Downloaded successfully \n");
  
  return 1;
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
      
    if (answer == 'y' || answer == 'Y') {
      download_updated_files(UPTODATE_VERSION);
      printf("Updated files \n");
      printf("Closing in 5 seconds \n");
      Sleep(5000);
      ExitProcess(0);
    } else {
      printf("Didn't update \n");
    }
  }
}
