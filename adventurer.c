#include <stdio.h>
#include <string.h>

int hunger_numerical = 0;

struct weapon {

  char* name;
  float attack;
  float gold_value;
  int durability;
  
};

struct inventory {

  struct weapon weapon;
  int gold;
  int rock;
  int wood;
  int food;
  
};

struct adventurer {

  char* name;
  float health_points;
  float attack_damage;
  float hunger;
  struct inventory inv_adv;
  
};

struct goblin_ennemy {

  float health_points;
  float attack_damage;
  
};

char* choose_username(void);
void game_setup(struct adventurer* adv);
void print_stats_adv(struct adventurer* adv);
void print_inventory(struct inventory* inv);

char* choose_username(void){

  static char name[17];
  char answer;

  while(1){
  
    printf("Enter an username (<= 16 characters) \n");
    scanf(" %16s", name);

    while (getchar() != '\n');
  
    printf("You are %s, right? Y/N \n", name);
    scanf("%c", &answer);
    while (getchar() != '\n');
  
    if (answer == 'Y' || answer == 'y') {
      printf("Alright %s, good luck! \n", name);
      break;
    } else if (answer == 'N' || answer == 'n') {
      printf("Let's try again \n");
    } else {
      printf("Invalid response. Either 'Y' or 'N'. \n");
    }
  }
  
  return name;
}

void game_setup(struct adventurer* adv) {
  
  char* username = choose_username();

  struct weapon Basic_sword = {"Basic sword", 3.0, 1.0, 64};

  /* Create character based on starting values */
  adv->name = username;
  adv->health_points = 10.0;
  adv->attack_damage = 0.0;
  adv->hunger = 0.00;

  /* Fill inventory of starting values */
  adv->inv_adv.weapon = Basic_sword;
  adv->inv_adv.gold = 5;
  adv->inv_adv.rock = 0;
  adv->inv_adv.wood = 0;
  adv->inv_adv.food = 0;

  printf("\n");
  print_stats_adv(adv);
  print_inventory(&adv->inv_adv);
  
}

void print_stats_adv(struct adventurer* adv){

  char* username = adv->name;
  float health = adv->health_points;
  float damage = adv->attack_damage;
  float hunger = adv->hunger;
  
  char hunger_percent_text[6] = "-----";
  int hunger_over_20 = (int)hunger / 20;

  for (int iterator = 0; iterator < 5; iterator++){
    if (hunger_over_20 > iterator) {
      hunger_percent_text[iterator] = '/';
    } 
  }

  hunger_percent_text[5] = '\0';
  
  if (hunger_numerical == 1) {
    printf("Name = %s \tHealth = %.1f    \tDamage = %.1f \tHunger = %.0f \n", username, health, damage, hunger);
  } else {
    printf("Name = %s \tHealth = %.1f    \tDamage = %.1f \tHunger = %s \n", username, health, damage, hunger_percent_text);
  }

}

void print_inventory(struct inventory* inv) {

  struct weapon weapon = inv->weapon;
  char* weapon_name = weapon.name;
  int gold = inv->gold;
  int rock = inv->rock;
  int wood = inv->wood;
  int food = inv->food;

  printf("Gold = %d \tWeapon = %s \tRock = %d \tWood = %d  \tFood = %d\n", gold, weapon_name, rock, wood, food);
  
}


