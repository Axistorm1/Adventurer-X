#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int** create_2d_arrays(int size_x, int size_y);
int fill_chunk(int** chunk);
int add_obstacle(int** chunk, int pos_x, int pos_y);
int** create_chunk(void);
void print_chunk(int** chunk);
int**** create_world(int size_x, int size_y);

int chunk_size_x = 8;
int chunk_size_y = 8;


int**** create_world(int size_x, int size_y){

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

  int obstacle = rand() % 4; /* 0 = No object, 1 = Rock, 2 = Chest, 3 = Tree */
  
  chunk[pos_x][pos_y] = obstacle;

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
