#include "adventurer.c"
#include "world_generator.c"
#include <ctype.h>
#include <conio.h>
#include "action_log.c"

#define GAME_NAME "Adventurer X"
#define GAME_AUTHOR "Axistorm"
#define ONLINE_MODE 1

#if ONLINE_MODE == 1 
#include "auto_update.c"
#endif

#if ONLINE_MODE == 0
#include <windows.h>
void check_up_to_date();
#endif

char display_char(int value);
void print_chunk_radius(int**** world, int pos_x, int pos_y, int radius_x, int radius_y);
void update_terminal(int**** world, struct adventurer adv, int pos_x, int pos_y);
void print_chunk_at_position(int**** world, int pos_x, int pos_y);
int get_value_at_position(int**** world, int pos_x, int pos_y);
void change_value_at_position(int**** world, int pos_x, int pos_y, int value);
int move_direction(int**** world, int* pos_x, int* pos_y, int x, int y, struct adventurer* adv);
int mine(int**** world, int pos_x, int pos_y, struct adventurer* adv);
int place(int**** world, int pos_x, int pos_y, struct adventurer* adv);
int eat_food(struct adventurer* adv);
int options_menu();
int is_action_valid(char* action);
char* ask_action();
void action(int**** world, int* pos_x, int* pos_y, struct adventurer* adv, char* default_choice);
int play_action_log(int**** world, int* pos_x, int* pos_y, struct adventurer* adv);

/* World options, modifiable at start-up */ 
int world_size_x = 16;
int world_size_y = 16;
char world_name[32];

/* Game related stuff */
int op_log_actions = 1;

/* Options */
int confirm_action = 0;
int radius_x = 16;
int radius_y = 32;
int food_per_eat = 1;
int display_coordinates = 1;
int display_stats = 1;
int display_inventory = 1;
int log_lines_amount = 5;

/* Custom keybinds slots */

char mine_keybind[32] = {"No keybind set"};
char place_keybind[32] = {"No keybind set"};
char right_keybind[32] = {"No keybind set"};
char left_keybind[32] = {"No keybind set"};
char up_keybind[32] = {"No keybind set"};
char down_keybind[32] = {"No keybind set"};
char eat_keybind[32] = {"No keybind set"};
char inventory_keybind[32] = {"No keybind set"};
char statistics_keybind[32] = {"No keybind set"};
char help_keybind[32] = {"No keybind set"};
char quit_keybind[32] = {"No keybind set"};
char options_keybind[32] = {"No keybind set"};
char logs_keybind[32] = {"No keybind set"};

/* Textures */
char texture_0 = '~';
char texture_1 = '#';
char texture_2 = '@';
char texture_3 = '%';
char texture_6 = 6;
char texture_default = 'D';


int main(void) {

  /* Check if game is updated */
  if (ONLINE_MODE == 1) {
    check_up_to_date();
  }
  
  /* Seeding pseudo-random number generator */
  srand(time(NULL));

  printf("\n");
  printf("Welcome to %s! Ready to start a new adventure? \n"
	 "Press any key to start... ", GAME_NAME);
  _getch();

  printf("\n");

  int load_or_create;
  
  printf("1 Load save file \n"
	 "2 Create new world \n");

  scanf(" %d", &load_or_create);

  while(getchar() != '\n');

  /* Load or create the world, create the adventurer, execute world action_log */

  int**** world;
  
  if (load_or_create == 1) {
    char filename[128];
    char confirmation = '\0';
    
    printf("What save file would you like to load? \n");
    scanf(" %s", filename);

    while(getchar() != '\n');

    load_world_size(&world_size_x, &world_size_y, filename);
    
    strcpy(world_name, filename);

    while (confirmation != 'Y' && confirmation != 'y' && confirmation != 'N' && confirmation != 'n') {
      printf("Do you want to load %s (height = %d, width = %d)? [Y/N] \n", filename, world_size_x, world_size_y);
      scanf(" %c", &confirmation);

      while (getchar() != '\n');
    }
    
    if (confirmation == 'Y' || confirmation == 'y') {
      world = create_world_from_file(world_size_x, world_size_y, filename);
    } else {
      ExitProcess(0);
    }
  }
  
  if (load_or_create == 2) {

    printf("Choose world name = ");
    scanf(" %s", world_name);

    while(getchar() != '\n');
    
    printf("Choose world height = ");
    scanf(" %d", &world_size_x);

    while(getchar() != '\n');
  
    printf("Choose world width = ");
    scanf(" %d", &world_size_y);

    while(getchar() != '\n');
  
    world = create_world(world_size_x, world_size_y, world_name);
    
    save_world_size(world_size_x, world_size_y, world_name);
  }
  
  if (load_or_create != 1 && load_or_create != 2) {
    ExitProcess(0);
  }
  
  struct adventurer adv1;
  game_setup(&adv1);

  /* Start position is the center of the world */
  int start_position_x = chunk_size_x * world_size_x / 2;
  int start_position_y = chunk_size_y * world_size_y / 2; 

  int current_position_x = start_position_x;
  int current_position_y = start_position_y;
  
  update_terminal(world, adv1, current_position_x, current_position_y);

  if (load_or_create == 1) {
    play_action_log(world, &current_position_x, &current_position_y, &adv1);
  }

  while(1){
    char* default_action = {"nothing"};
    action(world, &current_position_x, &current_position_y, &adv1, default_action);
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
  /* Return 0 when no block to mine, 1 when mined and 2 when can't mine, 4 when hunger too high */
  
  int tile_value = get_value_at_position(world, pos_x, pos_y);

  if (adv->hunger < 100) {
  
    if (tile_value == 1) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.rock += 1;
      adv->hunger += 1.00;

      return 1;
    } else if (tile_value == 2) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.food += 1;
      adv->hunger += 1.00;
      
      return 1;
    } else if (tile_value == 3) {

      change_value_at_position(world, pos_x, pos_y, 0);
      adv->inv_adv.wood += 1;
      adv->hunger += 1.00;

      return 1;
    } else if (tile_value == 0) {
      return 0;
    } else {
    
    return 2;
    }
  }

  return 4;
}

int place(int**** world, int pos_x, int pos_y, struct adventurer* adv){
  /* Return 0 when no block, 1 when placed, 2 when can't place this block, 3 can't place here, 4 hunger too high */
  if (adv->hunger < 100) {
    
    if (get_value_at_position(world, pos_x, pos_y) == 0) {

      int block_to_place = 0;
    
      printf("Which block do you want to place? \n");
      scanf(" %d", &block_to_place);

      while (getchar() != '\n');
    
      if (block_to_place == 1) {
	
	if (adv->inv_adv.rock > 0) {

	  change_value_at_position(world, pos_x, pos_y, 1);
	  return 1;
	}
	return 0;
	
      } else if (block_to_place == 3) {
	
	if (adv->inv_adv.wood > 0) {
	  
	  change_value_at_position(world, pos_x, pos_y, 3);
	  return 1;
	} return 0;
      } else {
	return 2;
      }
    }
    return 3;
  }
  return 4; 
}

int eat_food(struct adventurer* adv) {
  /* Return 0 when no food, 1 when ate successfully, 2 when hunger empty */
  if (adv->hunger > 0.0){
    if (adv->inv_adv.food >= (1 * food_per_eat)) {
    adv->inv_adv.food -= (1 * food_per_eat);
    adv->hunger -= (10 * food_per_eat);
    if (adv->hunger < 0.0) adv->hunger = 0;
    return 1;
    } return 0;
  } 
  return 2;
}

int graphics_option_menu() {
  int sub_option_id;
  
  printf("Graphisms: \n"
	 "1 Vertical view radius = %d \n"
	 "2 Horizontal view radius = %d \n"
	 ,radius_x, radius_y);

  printf("Choose an option to modify \n");
  scanf(" %d", &sub_option_id);

  while (getchar() != '\n');

  if (sub_option_id >= 1 && sub_option_id <=2) {
    int new_value;

    printf("New value = ");
    scanf(" %d", &new_value);

    if (sub_option_id == 1) radius_x = new_value;
    if (sub_option_id == 2) radius_y = new_value;
    
  }
  
  return 1;
}

int textures_option_menu() {
  int sub_option_id;
  
  printf("Textures: \n"
	 "0 Empty -> %c \n"
	 "1 Rock -> %c \n"
	 "2 Chest -> %c \n"
	 "3 Tree -> %c \n"
	 "6 Adventurer -> %c \n",
	 texture_0, texture_1, texture_2, texture_3, texture_6);

  printf("Choose a texture to modify \n");
  scanf(" %d", &sub_option_id);

  while (getchar() != '\n');

  if ((sub_option_id >= 0 && sub_option_id <= 6) && (sub_option_id != 4 && sub_option_id != 5)){
    char new_char;

    printf("New character = ");
    scanf(" %c", &new_char);
    
    if (sub_option_id == 0) texture_0 = new_char;
    if (sub_option_id == 1) texture_1 = new_char;
    if (sub_option_id == 2) texture_2 = new_char;
    if (sub_option_id == 3) texture_3 = new_char;
    if (sub_option_id == 6) texture_6 = new_char;
  }
  
  return 1;
}

int toggles_option_menu() {
  int sub_option_id;

  printf("Toggles: \n"
	 "1 Confirm actions = %d \n"
	 "2 Numerical hunger = %d \n"
	 "3 Display coordinates = %d \n"
	 "4 Display statistics = %d \n"
	 "5 Display inventory = %d \n",
	 confirm_action, hunger_numerical,
	 display_coordinates, display_stats,
	 display_inventory);

  printf("Choose an option to modify \n");
  scanf(" %d", &sub_option_id);

  while (getchar() != '\n');

  if (sub_option_id >= 1 && sub_option_id <= 5) {
    int new_value;

    printf("New value = ");
    scanf(" %d", &new_value);

    if (sub_option_id == 1) confirm_action = new_value;
    if (sub_option_id == 2) hunger_numerical = new_value;
    if (sub_option_id == 3) display_coordinates = new_value;
    if (sub_option_id == 4) display_stats = new_value;
    if (sub_option_id == 5) display_inventory = new_value;
  }

  return 1;
}

int custom_values_option_menu() {
  int sub_option_id;

  printf("Custom values: \n"
	 "1 Food eaten per action = %d \n"
	 "2 Log lines to display = %d \n",
	 food_per_eat, log_lines_amount);

  printf("Choose an option to modify \n");
  scanf(" %d", &sub_option_id);

  while (getchar() != '\n');

  if (sub_option_id >= 1 && sub_option_id <= 2) {
    int new_value;

    printf("New value = ");
    scanf(" %d", &new_value);

    if (sub_option_id == 1) food_per_eat = new_value;
    if (sub_option_id == 2) log_lines_amount = new_value;
  }

  return 1;
}

int keybinds_option_menu(){
  int sub_option_id;

  printf("Keybinds: \n"
	 "1 Mine -> %s \n"
	 "2 Place -> %s \n"
	 "3 Right -> %s \n"
	 "4 Left -> %s \n"
	 "5 Down -> %s \n"
	 "6 Up -> %s \n"
	 "7 Eat -> %s \n"
	 "8 Inventory -> %s \n"
	 "9 Statistics -> %s \n"
	 "10 Help -> %s \n"
	 "11 Quit -> %s \n"
	 "12 Options -> %s \n"
	 "13 Logs -> %s \n",
	 mine_keybind, place_keybind,
	 right_keybind, left_keybind,
	 down_keybind, up_keybind,
	 eat_keybind, inventory_keybind, 
	 statistics_keybind, help_keybind,
	 quit_keybind, options_keybind,
	 logs_keybind);

  printf("Choose a keybind to modify \n");
  scanf(" %d", &sub_option_id);

  while (getchar() != '\n');

  if (sub_option_id >= 1 && sub_option_id <= 13) {
    static char new_keybind[32];

    printf("New keybind = ");
    scanf(" %s", new_keybind);

    while (getchar() != '\n');
    
    if (sub_option_id == 1) strcpy(mine_keybind, new_keybind);
    if (sub_option_id == 2) strcpy(place_keybind, new_keybind);
    if (sub_option_id == 3) strcpy(right_keybind, new_keybind);
    if (sub_option_id == 4) strcpy(left_keybind, new_keybind);
    if (sub_option_id == 5) strcpy(down_keybind, new_keybind);
    if (sub_option_id == 6) strcpy(up_keybind, new_keybind);
    if (sub_option_id == 7) strcpy(eat_keybind, new_keybind);
    if (sub_option_id == 8) strcpy(inventory_keybind, new_keybind);
    if (sub_option_id == 9) strcpy(statistics_keybind, new_keybind);
    if (sub_option_id == 10) strcpy(help_keybind, new_keybind);
    if (sub_option_id == 11) strcpy(quit_keybind, new_keybind);
    if (sub_option_id == 12) strcpy(options_keybind, new_keybind);
    if (sub_option_id == 13) strcpy(logs_keybind, new_keybind);
  }

  return 1;
}

int options_menu() {
  int option_id;
  
  printf("Options: \n"
	 "1 Graphisms \n"
	 "2 Textures \n"
	 "3 Toggles \n"
	 "4 Custom values \n"
	 "5 Keybinds \n");
  
  printf("Choose a category \n");
  scanf(" %d", &option_id);

  while(getchar() != '\n');
  
  if (option_id == 1) {
    graphics_option_menu();
    return 1;
  }

  if (option_id == 2) {
    textures_option_menu();
    return 1;
  }

  if (option_id == 3) {
    toggles_option_menu();
    return 1;
  }

  if (option_id == 4) {
    custom_values_option_menu();
    return 1;
  }
  
  if (option_id == 5) {
    keybinds_option_menu();
    return 1;
  }

  return 1;
}

void lower_string(char* string){
  
  int character = 0;
  while(string[character] != '\0') {
    string[character] = tolower(string[character]);
    character++;
  }
  
}

int is_action_valid(char* action) {
  
  const char *valid_actions[] = {"right", "r", right_keybind, "left", "l", left_keybind,
         "up", "u", up_keybind, "down", "d", down_keybind, "mine", "m", mine_keybind,
				 "place", "p", place_keybind, "eat", "e", eat_keybind, "inventory", "i",
         inventory_keybind, "statistics", "s", statistics_keybind, "help", "h",
         help_keybind, "quit", "q", quit_keybind, "options", "o", options_keybind, 
         "logs", logs_keybind};

  int num_choices = sizeof(valid_actions) / sizeof(valid_actions[0]);

  for (int num = 0; num < num_choices; num++) {
    if (strcmp(action, valid_actions[num]) == 0) {
      return 1;
    } else {

      // printf("Comparison between %s and %s failed \n", action, valid_actions[num]); DEBUG

    }
  }
  
  return 0;
}

char* ask_action() {
  
  static char choice[ACTION_MAX_LENGTH];
  
  printf("What action would you like to do? \n");
  scanf(" %s", choice);
  
  while (getchar() != '\n');
  
  lower_string(choice);

  while (is_action_valid(choice) != 1) {
     printf("Wrong action. List of actions: \n"
	     "'right' -> move right \n"
	     "'left' -> move left \n"
	     "'up' -> move up \n"
	     "'down' -> move down \n"
	     "'mine' -> mine block \n"
	     "'place' -> place block \n"
	     "'eat' -> eat food \n"
	     "'inventory' -> show inventory \n"
	     "'statistics' -> show statistics \n"
	     "'help' -> help menu \n"
	     "'quit' -> quit \n"
	     "'options' -> options \n"
	     "'logs' -> show action log \n");
     scanf(" %s", choice);

     while (getchar() != '\n');

     lower_string(choice);
  }

  if (confirm_action == 1) {
      char answer = '\0';
      while (answer != 'Y' && answer != 'y' && answer != 'n' && answer != 'N') {
        printf("Are you sure you want to do the following action? %s \n", choice);
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

int translate_custom_keybinds(char* choice, int**** world, int* pos_x, int* pos_y, struct adventurer* adv) {
  if (strcmp(choice, right_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "r");
    return 1;
  }
  if (strcmp(choice, left_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "l");
    return 1;
  }
  if (strcmp(choice, down_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "d");
    return 1;
  }
  if (strcmp(choice, up_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "u");
    return 1;
  }
  if (strcmp(choice, mine_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "m");
    return 1;
  }
  if (strcmp(choice, place_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "p");
    return 1;
  }
  if (strcmp(choice, eat_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "e");
    return 1;
  }
  if (strcmp(choice, inventory_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "i");
    return 1;
  }
  if (strcmp(choice, statistics_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "s");
    return 1;
  }
  if (strcmp(choice, help_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "help");
    return 1;
  }
  if (strcmp(choice, quit_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "q");
    return 1;
  }
  if (strcmp(choice, options_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "o");
    return 1;
  }
  if (strcmp(choice, logs_keybind) == 0) {
    action(world, pos_x, pos_y, adv, "logs");
    return 1;
  }

  return 0;
}

void action(int**** world, int* pos_x, int* pos_y, struct adventurer* adv, char* default_choice) {

  char* choice;
  
  if (strcmp(default_choice, "nothing") == 0){
  choice = ask_action();
  } else if (strcmp(default_choice, "q") == 0 || strcmp(default_choice, "quit") == 0 ||
             strcmp(default_choice, "o") == 0 || strcmp(default_choice, "options") == 0 ||
             strcmp(default_choice, "help") == 0 || strcmp(default_choice, "h") == 0 || 
             strcmp(default_choice, "logs") == 0) {
    return;
  } else {
    choice = default_choice;
  }
  
  if (translate_custom_keybinds(choice, world, pos_x, pos_y, adv) == 1) return;

  int mined, placed, ate;
  
  char stored_lines[log_lines_amount][ACTION_MAX_LENGTH];
  int num_lines_stored;
  int smaller;

  char choice_log[ACTION_MAX_LENGTH];
  strcpy(choice_log, choice);
  strcat(choice_log, "\n");
  
  if (op_log_actions == 1) write_char(choice_log, world_name);

  if (strcmp(choice, "r") == 0 || strcmp(choice, "right") == 0) {
    move_direction(world, pos_x, pos_y, 0, 1, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);

  } else if (strcmp(choice, "d") == 0 || strcmp(choice, "down") == 0 || strcmp(choice, down_keybind) == 0) {
    move_direction(world, pos_x, pos_y, 1, 0, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);

  } else if (strcmp(choice, "l") == 0 || strcmp(choice, "left") == 0 || strcmp(choice, left_keybind) == 0) {
    move_direction(world, pos_x, pos_y, 0, -1, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);

  } else if (strcmp(choice, "u") == 0 || strcmp(choice, "up") == 0 || strcmp(choice, up_keybind) == 0) {
    move_direction(world, pos_x, pos_y, -1, 0, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);

  } else if (strcmp(choice, "m") == 0 || strcmp(choice, "mine") == 0 || strcmp(choice, mine_keybind) == 0) {
    mined = mine(world, *pos_x, *pos_y, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    if (mined == 0) printf("There is no block to mine \n");
    if (mined == 1); // Success
    if (mined == 2) printf("You can't mine this block \n");
    if (mined == 4) printf("You're too hungry to do this \n");

  } else if (strcmp(choice, "p") == 0 || strcmp(choice, "place") == 0 || strcmp(choice, place_keybind) == 0) {
    placed = place(world, *pos_x, *pos_y, adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    if (placed == 0) printf("You don't have the block you want to place \n");
    if (placed == 1); // Success
    if (placed == 2) printf("This can't be placed \n");
    if (placed == 3) printf("You can't place this block here \n");
    if (placed == 4) printf("You're too hungry to do this \n");

  } else if (strcmp(choice, "e") == 0 || strcmp(choice, "eat") == 0 || strcmp(choice, eat_keybind) == 0) {
    ate = eat_food(adv);
    update_terminal(world, *adv, *pos_x, *pos_y);
    if (ate == 0) printf("You don't have any food left \n");
    if (ate == 1); // Success
    if (ate == 2) printf("You are not hungry \n");

  } else if (strcmp(choice, "i") == 0 || strcmp(choice, "inventory") == 0 || strcmp(choice, "inv") == 0 || strcmp(choice, inventory_keybind) == 0) {
    system("cls");
    print_inventory(&adv->inv_adv);

  } else if (strcmp(choice, "s") == 0 || strcmp(choice, "statistics") == 0 || strcmp(choice, "stats") == 0 || strcmp(choice, statistics_keybind) == 0) {
    system("cls");
    print_stats_adv(adv);

  } else if (strcmp(choice, "h") == 0) {
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

  } else if (strcmp(choice, "help") == 0 || strcmp(choice, help_keybind) == 0) {
    update_terminal(world, *adv, *pos_x, *pos_y);
    printf("List of actions: \n"
	     "'right' -> move right \n"
	     "'left' -> move left \n"
	     "'up' -> move up \n"
	     "'down' -> move down \n"
	     "'mine' -> mine block \n"
	     "'place' -> place block \n"
	     "'eat' -> eat food \n"
	     "'inventory' -> show inventory \n"
	     "'statistics' -> show statistics \n"
	     "'help' -> help menu \n"
	     "'quit' -> quit \n"
	     "'options' -> options \n"
	     "'logs' -> show action log \n");

  } else if (strcmp(choice, "q" ) == 0 || strcmp(choice, "quit") == 0 || strcmp(choice, quit_keybind) == 0) {
    printf("Closing game\n");
    ExitProcess(0);

  } else if (strcmp(choice, "o") == 0 || strcmp(choice, "options") == 0 || strcmp(choice, options_keybind) == 0) {
    system("cls");
    options_menu();
    update_terminal(world, *adv, *pos_x, *pos_y);

  } else if (strcmp(choice, "logs") == 0 || strcmp(choice, logs_keybind) == 0) {
    store_last_n_lines(log_lines_amount, stored_lines, &num_lines_stored, world_name);
    printf("Last %d actions: \n", log_lines_amount);
    smaller = num_lines_stored < log_lines_amount ? num_lines_stored : log_lines_amount;
    print_n_lines(smaller, stored_lines);
  } else {}
  
}

int play_action_log(int**** world, int* pos_x, int* pos_y, struct adventurer* adv){
  char lines[MAX_STRINGS][ACTION_MAX_LENGTH];
  int actions;

  actions = load_all_actions(lines, world_name);

  op_log_actions = 0;
  
  for(int i = 0; i < actions; i++){
    action(world, pos_x, pos_y, adv, lines[i]);
  }

  op_log_actions = 1;
  
  return 1;
}
