#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACTION_LOG_FILE "action_logs.txt"
#define ACTION_MAX_LENGTH 12

int write_char(char* executed_action) {

  FILE *file = fopen(ACTION_LOG_FILE, "a");

  if (file == NULL) {
    printf("File couldn't be opened \n");
    exit(1);
  }
  
  size_t len = strlen(executed_action);
  fwrite(executed_action, 1, len, file);

  fclose(file);
  
  return 1;
}

void store_last_n_lines(int n, char lines[][ACTION_MAX_LENGTH], int *num_lines_stored) {

  FILE *file;

  file = fopen(ACTION_LOG_FILE, "r");

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

void print_n_lines(int n, char lines[][ACTION_MAX_LENGTH]) {
    for (int i = 0; i < n; i++) {
      printf("-> %s", lines[i]);
    }
  }

int use_exemple() {

  char a[] = "mine \n";
  
  write_char(a);
  write_char(a);
  write_char(a);

  char lines[5][ACTION_MAX_LENGTH];
  int num_lines_stored;
  
  store_last_n_lines(5, lines, &num_lines_stored);

  int smaller = num_lines_stored < 5 ? num_lines_stored : 5;
  
  print_n_lines(smaller, lines);
  
  return 0;
  
}
