#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "save_load.c"

int** create_2d_arrays(int size_x, int size_y);
int fill_chunk(int** chunk);
int add_obstacle(int** chunk, int pos_x, int pos_y);
int** create_chunk(void);
void print_chunk(int** chunk);
char** load_structure_from_array(char* structure_array, int array_length);
int add_structure(int** chunk, char** structure_recipe, int array_length);
int**** create_world(int size_x, int size_y, char* world_name);

char sr1_array[] = {"00010101020103010401050106010701"
		    "10011100120013031403150016001701"
		    "20012100220023002400250026002701"
		    "30013103320033023402350036033701"
		    "40014103420043024402450046034701"
		    "50015100520053005400550056005701"
		    "60016100620063036403650066006701"
		    "70017101720173017401750176017701"};
int sr1_array_length = sizeof(sr1_array) / sizeof(sr1_array[0]);

int**** create_world_from_file(int size_x, int size_y, char* filename){

  int**** world = malloc(size_x * sizeof(int***));

  if (world == NULL){
    printf("Memory allocation failed \n");
    return NULL;
  }

  int count = 0;

  for(int x = 0; x < size_x; x++) {
    world[x] = malloc(size_y * sizeof(int***));
    if(world[x] == NULL) {
      printf("Memory allocation failed \n");
      return NULL;
  }
    for(int y = 0; y < size_y; y++) {
      world[x][y] = load_chunk_from_position(filename, (count * chunk_size_x * chunk_size_y * 2));
      count++;
    }
  }
  return world;

}

int**** create_world(int size_x, int size_y, char* world_name){

  int**** world = malloc(size_x * sizeof(int***));
			
  if (world == NULL){
    printf("Memory allocation failed \n");
    return NULL;
  }

  for(int x = 0; x < size_x; x++) {
    world[x] = malloc(size_y * sizeof(int***));
    if(world[x] == NULL) {
      printf("Memory allocation failed \n");
      return NULL;
  }
    for(int y = 0; y < size_y; y++) {
      world[x][y] = create_chunk();
      save_chunk(world[x][y], world_name);
    }
  }
  return world;
}

void print_chunk(int** chunk){

  for(int x = 0; x < chunk_size_x; x++) {
    for(int y = 0; y < chunk_size_y; y++) {
      printf("%d", chunk[x][y]);
    }
    printf("\n");
  }
  
}

int** create_chunk(void){
  
  int** chunk = create_2d_arrays(chunk_size_x, chunk_size_y);

  if(chunk == NULL) {
    printf("Chunk creation failed \n");
    return NULL;
}

  fill_chunk(chunk);
  
  return chunk;
}

int fill_chunk(int** chunk){

  int amount = rand() % (chunk_size_x * chunk_size_y) / 4;

  /* Only create a structure if result is 64, allows to make sure it only generates in chunks <= to 8 of width and height */
  int structure_amount = rand() % (chunk_size_x * chunk_size_y);

  if (structure_amount == 63) {

    /* Should load structures out of this function that repeats but can't find a way to */
    char** structure_recipe_1 = load_structure_from_array(sr1_array, sr1_array_length);
    
    int sr1_length = sr1_array_length / 4;
    
    add_structure(chunk, structure_recipe_1, sr1_length);
    
    return 1; 
  }
  
  for(int x = 0; x != amount; x++){
    int pos_x = rand() % chunk_size_x;
    int pos_y = rand() % chunk_size_y;
    
    if(chunk[pos_x][pos_y] == 0) {
    add_obstacle(chunk, pos_x, pos_y);
    } else {
      x--;
    }
  }
  return 1;
}

int add_obstacle(int** chunk, int pos_x, int pos_y){

  int obstacle = rand() % 4; /* 0 = No object, 1 = Rock, 2 = Food, 3 = Tree */
  
  chunk[pos_x][pos_y] = obstacle;

  return 1;
}

char** load_structure_from_array(char* structure_array, int array_length){
 
  char** structure_recipe = (char**)malloc(array_length * sizeof(char*));
  
  if (structure_recipe == NULL) {
        printf("Failed to allocate memory \n");
        return NULL;
    }
  
  for(int iterator = 0; iterator < array_length; iterator++){

    int array_position = iterator * 4;

    structure_recipe[iterator] = (char*)malloc(5 * sizeof(char));
  
        if (structure_recipe[iterator] == NULL) {
            printf("Failed to allocate memory \n");
	    
            for (int j = 0; j < iterator; j++) {
                free(structure_recipe[j]);
            }
            free(structure_recipe);
            return NULL;
        }

        // Copy the substring into the allocated memory
        strncpy(structure_recipe[iterator], structure_array + array_position, 4);
        structure_recipe[iterator][4] = '\0'; // Null-terminate the string
    }

  return structure_recipe;
}

int add_structure(int** chunk, char** structure_recipe, int array_length) {
  
  for (int iterator = 0; iterator < array_length; iterator++) {
    
    int pos_x = structure_recipe[iterator][0] - '0';
    int pos_y = structure_recipe[iterator][1] - '0';

    int value_2 = structure_recipe[iterator][2] - '0';
    int value_3 = structure_recipe[iterator][3] - '0';
    
    int texture_id = value_2 * 10 + value_3;

    chunk[pos_x][pos_y] = texture_id;
    
  }

  return 1;
}

int** create_2d_arrays(int size_x, int size_y) {
  /* Return NULL if the board is too big, leading to memory issues*/
  if (size_x > 100 || size_y > 100){
    printf("Maximal board size is 100 \n");
    return NULL;
  }

  /* Allocating memory to the 2-dimensional array used as board.
  First allocating memory for the rows.*/
  int** board = malloc(size_x * sizeof(int*));

  if (board == NULL){
    printf("Memory allocation failed \n");
    return NULL;
  }
  /* Then allocating memory for the columns.*/
  for (int i=0; i < size_x; i++) {
    board[i] = malloc(size_y * sizeof(int));

    if (board[i] == NULL){
      printf("Memory allocation failed \n");

      for (int j = 0; j < i; j++){
        free(board[j]);
      }

      free(board);
      return NULL;
    }
    /* Initialize all the cells to 0.*/
    for (int j = 0; j < size_y; j++){
      board[i][j] = 0;
    }
  }

  return board;
} 
