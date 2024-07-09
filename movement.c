#include <stdlib.h>
#include <stdio.h>
#include "adventurer.c"
#include "world_generator.c"
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "auto_update.c"

#define GAME_NAME "Adventurer X"

char display_char(int value);
void print_chunk_radius(int**** world, int pos_x, int pos_y, int radius_x, int radius_y);
void update_terminal(int**** world, struct adventurer adv, int pos_x, int pos_y);
void print_chunk_at_position(int**** world, int pos_x, int pos_y);
int get_value_at_position(int**** world, int pos_x, int pos_y);
void change_value_at_position(int**** world, int pos_x, int pos_y, int value);
int move_direction(int**** world, int* pos_x, int* pos_y, int x, int y, struct adventurer* adv);
int mine(int**** world, int pos_x, int pos_y, struct adventurer* adv);
int eat_food(struct adventurer* adv);
int options_menu();
int is_action_valid(char action);
char ask_action();
void action(int**** world, int* pos_x, int* pos_y, struct adventurer* adv);


/* World options, modifiable at start-up */ 
int world_size_x = 16;
int world_size_y = 16;

/* Options */
int confirm_action = 0;
int radius_x = 16;
int radius_y = 32;
int food_per_eat = 1;
int display_coordinates = 1;
int display_stats = 1;
int display_inventory = 1;

/* Textures */
char texture_0 = '~';
char texture_1 = '#';
char texture_2 = '@';
char texture_3 = '%';
char texture_6 = 6;
char texture_default = 'D';


int main(void) {

  /* Check if game is updated */
  check_up_to_date();
  
  /* Seeding pseudo-random number generator */
  srand(time(NULL));

  printf("\n");
  printf("Welcome to %s! Ready to start a new adventure? \n"
	 "Press any key to start... ", GAME_NAME);
  _getch();

  printf("\n");
  
  printf("Choose world height = ");
  scanf(" %d", &world_size_x);

  while(getchar() != '\n');
  
  printf("Choose world width = ");
  scanf(" %d", &world_size_y);

  while(getchar() != '\n');
  
  int**** world = create_world(world_size_x, world_size_y);
  
  struct adventurer adv1;
  game_setup(&adv1);

  /* Start position is the center of the world */
  int start_position_x = chunk_size_x * world_size_x / 2;
  int start_position_y = chunk_size_y * world_size_y / 2; 

  int current_position_x = start_position_x;
  int current_position_y = start_position_y;

  update_terminal(world, adv1, current_position_x, current_position_y);

  while(1){
    action(world, &current_position_x, &current_position_y, &adv1);
    }

  return 1; 
}

char display_char(int value) {
  switch (value) {

  case 0:
    return texture_0;
  case 1:
    return texture_1;
  case 2:
    return texture_2;
  case 3:
    return texture_3;
  case 6:
    return texture_6;
  default:
    return texture_default;
    
  }
}

void print_chunk_radius(int**** world, int pos_x, int pos_y, int radius_x, int radius_y) {

  int start_pos_x = pos_x - radius_x / 2;
  int start_pos_y = pos_y - radius_y /2;
  
  int chunk_x;
  int chunk_y;

  int relative_pos_x;
  int relative_pos_y;

  // This can't possibly be efficient for high radius values
  for(int x = 0; x < radius_x; x++){
    for(int y = 0; y < radius_y; y++){
      chunk_x = (start_pos_x + x) / 8;
      chunk_y = (start_pos_y + y) / 8;

      if(chunk_x >= world_size_x || chunk_x < 0 || chunk_y >= world_size_y || chunk_y < 0){
	printf(" ");
	continue;
      }
      
      relative_pos_x = (start_pos_x + x) % 8;
      relative_pos_y = (start_pos_y + y) % 8;

      if(relative_pos_x >= chunk_size_x || relative_pos_x < 0 || relative_pos_y >= chunk_size_y || relative_pos_y < 0){
	printf(" ");
	continue;
      }

      int tile_value = world[chunk_x][chunk_y][relative_pos_x][relative_pos_y];
      char display_c  = display_char(tile_value);
      printf("%c", display_c);
    }  
    printf("\n");
    
  }
}

void update_terminal(int**** world, struct adventurer adv, int pos_x, int pos_y) {

  /* Clean the terminal and futurely apply the zoom level */
  system("cls");

  /* Adventurer represented by value 6 */
  int temp_value = get_value_at_position(world, pos_x, pos_y);
  change_value_at_position(world, pos_x, pos_y, 6);
  print_chunk_radius(world, pos_x, pos_y, radius_x, radius_y);
  change_value_at_position(world, pos_x, pos_y, temp_value);

  printf("\n");
  if (display_stats == 1) print_stats_adv(&adv);
  if (display_inventory == 1) print_inventory(&adv.inv_adv);
  printf("\n");
  if (display_coordinates == 1) printf("X = %d \nY = %d \n", pos_x, pos_y);
  printf("\n");

  
}

void print_chunk_at_position(int**** world, int pos_x, int pos_y) {

  int chunk_x = pos_x / chunk_size_x;
  int chunk_y = pos_y / chunk_size_y;

  print_chunk(world[chunk_x][chunk_y]);
  
}

int get_value_at_position(int**** world, int pos_x, int pos_y) {
  
  int chunk_x = pos_x / chunk_size_x;
  int chunk_y = pos_y / chunk_size_y;

  if (chunk_x >= world_size_x || chunk_x < 0 || chunk_y >= world_size_y || chunk_y < 0) {
    return 99; // Should allow the adventurer to try to move after the border but prevent since != 0
  }
  
  int relative_pos_x = pos_x % chunk_size_x;
  int relative_pos_y = pos_y % chunk_size_y;

  if(relative_pos_x >= chunk_size_x || relative_pos_x < 0 || relative_pos_y >= chunk_size_y || relative_pos_y < 0){
    return 99;
  }
  
  int value = world[chunk_x][chunk_y][relative_pos_x][relative_pos_y];

  return value;
}

void change_value_at_position(int**** world, int pos_x, int pos_y, int value) {

  int chunk_x = pos_x / chunk_size_x;
  int chunk_y = pos_y / chunk_size_y;

  int relative_pos_x = pos_x % chunk_size_x;
  int relative_pos_y = pos_y % chunk_size_y;
  
  world[chunk_x][chunk_y][relative_pos_x][relative_pos_y] = value;
  
}

int move_direction(int**** world, int* pos_x, int* pos_y, int x, int y, struct adventurer* adv) {

  int adjacent_tile_value = get_value_at_position(world, *pos_x + x, *pos_y + y);

  if ((adjacent_tile_value == 0 || adjacent_tile_value == 1 ||
       adjacent_tile_value == 2 || adjacent_tile_value == 3)
      && (adv->hunger < 100)){
    
    *pos_x = *pos_x + x;
    *pos_y = *pos_y + y;

    adv->hunger += 0.25;
    
  } else return 0;
  
  return 1;
}

int mine(int**** world, int pos_x, int pos_y, struct adventurer* adv) {

  int tile_value = get_value_at_position(world, pos_x, pos_y);

  if (adv->hunger < 100) {
  
    if (tile_value == 1) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.rock += 1;
      adv->hunger += 1.00;

      return 1;
    }

    if (tile_value == 2) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.food += 1;
      adv->hunger += 1.00;
      
      return 1;
    }

    if (tile_value == 3) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.wood += 1;
      adv->hunger += 1.00;

      return 1;
    }
  }

  return 0;
}

int eat_food(struct adventurer* adv) {

  if (adv->hunger > 0.0 && adv->inv_adv.food >= (1 * food_per_eat)) {
    adv->inv_adv.food -= (1 * food_per_eat);
    adv->hunger -= (25 * food_per_eat);
    if (adv->hunger < 0.0) adv->hunger = 0;
  }
  
  return 0;
}

int options_menu() {

  printf("Options: \n"
	 "1 Confirm actions = %d \n"
	 "2 Vertical view radius = %d \n"
	 "3 Horizontal view radius = %d \n"
	 "4 Modify textures \n"
	 "5 Food eaten per action = %d \n"
	 "6 Numerical hunger = %d \n"
	 "7 Display coordinates = %d \n"
	 "8 Display statistics = %d \n"
	 "9 Display inventory = %d \n",
	 confirm_action, radius_x,
	 radius_y, food_per_eat,
	 hunger_numerical, display_coordinates,
	 display_stats, display_inventory);
  
  printf("To modify an option, write its id \n");

  int option_id;
  int new_value;
  
  scanf(" %d", &option_id);

  while(getchar() != '\n');

  /* Rewrite this as a function that checks for a value in a list */
  if (option_id == 1 || option_id == 2 ||
      option_id == 3 || option_id == 5 ||
      option_id == 6 || option_id == 7 ||
      option_id == 8 || option_id == 9){
  
    printf("New value = ");
    scanf(" %d", &new_value);

    while(getchar() != '\n');
    
    if (option_id == 1) confirm_action = new_value;
    if (option_id == 2) radius_x = new_value;
    if (option_id == 3) radius_y = new_value;
    if (option_id == 5) food_per_eat = new_value;
    if (option_id == 6) hunger_numerical = new_value;
    if (option_id == 7) display_coordinates = new_value;
    if (option_id == 8) display_stats = new_value;
    if (option_id == 9) display_inventory = new_value;

    return 1;
  } else if (option_id == 4) {

    printf("\n");
    printf("0 Empty -> %c \n"
           "1 Rock -> %c \n"
           "2 Chest -> %c \n"
           "3 Tree -> %c \n"
	         "6 User -> %c \n",
           texture_0, texture_1, texture_2, texture_3, texture_6);

    printf("To modify a texture, write its id \n");

    int texture_id;
    char new_char;

    scanf(" %d", &texture_id);

    while(getchar() != '\n');

    if (texture_id == 0 || texture_id == 1 || texture_id == 2 || texture_id == 3 || texture_id == 6) {
      printf("New character = ");
      scanf(" %c", &new_char);

      while(getchar() != '\n');

      if (texture_id == 0) texture_0 = new_char;
      if (texture_id == 1) texture_1 = new_char;
      if (texture_id == 2) texture_2 = new_char;
      if (texture_id == 3) texture_3 = new_char;
      if (texture_id == 6) texture_6 = new_char;
    }
    
  }
  
	 
  return 0;
}

int is_action_valid(char action) {

  char valid_actions[] = {'r', 'u', 'd', 'l', 'm', 'h', 'q', 'o', 'e', 'i', 's'};
  int num_choices = sizeof(valid_actions) / sizeof(valid_actions[0]);

  for (int num = 0; num < num_choices; num++) {
    if (action == valid_actions[num]) {
      return 1;
    }
  }

  return 0;
}

char ask_action() {
    char choice_any, choice;
    printf("What action would you like to do? \n");
    scanf(" %c", &choice_any);

    choice = tolower(choice_any);

    while (getchar() != '\n');

    while(is_action_valid(choice) != 1) {
      printf("Wrong action. List of actions: \n"
	     "'r' -> move right \n"
	     "'l' -> move left \n"
	     "'u' -> move up \n"
	     "'d' -> move down \n"
	     "'m' -> mine block \n"
	     "'e' -> eat food \n"
	     "'i' -> show inventory \n"
	     "'s' -> show statistics \n"
	     "'h' -> help menu \n"
	     "'q' -> quit \n"
	     "'o' -> options \n");
      scanf(" %c", &choice);

      while (getchar() != '\n');
    }

    if(confirm_action == 1) {
      char answer = '\0';
      while(answer != 'Y' && answer != 'y' && answer != 'n' && answer != 'N') {
        printf("Are you sure you want to do the following action? %c \n", choice);
        scanf(" %c", &answer);

	while (getchar() != '\n');
      }

      if (answer == 'n' || answer == 'N') {
        return ask_action();
      } else if (answer == 'y' || answer == 'Y') {
        return choice;
      }
    }

    return choice;
}

void action(int**** world, int* pos_x, int* pos_y, struct adventurer* adv) {

  char choice = ask_action();
  
  switch(choice) {

  case 'r':
    move_direction(world, pos_x, pos_y, 0, 1, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'd':
    move_direction(world, pos_x, pos_y, 1, 0, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'l':
    move_direction(world, pos_x, pos_y, 0, -1, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'u':
    move_direction(world, pos_x, pos_y, -1, 0, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'm':
    mine(world, *pos_x, *pos_y, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'e':
    eat_food(adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  case 'i':
    system("cls");
    print_inventory(&adv->inv_adv);
    break;

  case 's':
    system("cls");
    print_stats_adv(adv);
    break;

  case 'h':
    update_terminal(world, *adv, *pos_x, *pos_y);
    printf("List of actions: \n"
	   "'r' -> move right \n"
	   "'l' -> move left \n"
	   "'u' -> move up \n"
	   "'d' -> move down \n"
	   "'m' -> mine block \n"
	   "'i' -> show inventory \n"
	   "'s' -> show statistics \n"
	   "'h' -> help menu \n"
	   "'q' -> quit \n"
	   "'o' -> options \n"
	   );
    break;

  case 'q':
    printf("Closing game \n");
    ExitProcess(0);

  case 'o':
    system("cls");
    options_menu();
    update_terminal(world, *adv, *pos_x, *pos_y);
    break;

  default:
    break;
  }
  
}
