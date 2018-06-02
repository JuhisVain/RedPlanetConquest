#ifndef RP_DATATYPES__
#define RP_DATATYPES__

#include <stdio.h>

#define WORLD_HEIGHT 50
#define WORLD_WIDTH 100

#define PACK __attribute__((packed))
#define PACKnALIGN __attribute__((packed,aligned(1)))
#define ALIGN __attribute__((aligned(1)))

#define DEBUG_SETUP()				\
  FILE *macro_debugfile

#define DEBUG_X(X)				\
  macro_debugfile = fopen("test.data","a");	\
  fprintf(macro_debugfile, X);			\
  fclose(macro_debugfile)

#define DEBUG_XY(X,Y)				\
  macro_debugfile = fopen ("test.data","a");	\
  fprintf(macro_debugfile,X,Y);			\
  fclose(macro_debugfile)
  

#define NAME_LENGTH 20
#define MAX_TROOPTYPE_AMOUNT 10
#define MAX_ARMYTEMPLATE_AMOUNT 10

typedef unsigned short tile;

#define TROOP_MOVE_CLASS_LENGTH 4
enum troop_move_class {
  INFANTRY = 0,
  WHEEL = 1,
  TRACK = 2,
  HOVER = 3
};

enum troop_weapon_class {
  SMALL,
  HEAVY_SMALL,
  CANNON,
  ARTILLERY
};

enum troop_armor_class {
  NONE,
  LIGHT,
  MEDIUM,
  HEAVY
};

enum troop_ability_class {
  NO_SPECIAL,
  ENGINEER,
  SCOUT
  //add more later
};

typedef struct troop_type {
  char name[NAME_LENGTH];
  //these enums should be packed in a single variable
  enum troop_move_class movement;
  enum troop_weapon_class weapon;
  enum troop_armor_class armor;
  enum troop_ability_class ability; //Should be possible to have more than one
  char vision_range;
} troop_type;

typedef struct army_template {
  char id;
  char name[NAME_LENGTH];
  troop_type troop[MAX_TROOPTYPE_AMOUNT];
  unsigned int default_troop_count[MAX_TROOPTYPE_AMOUNT];
  
} army_template;

struct faction;

typedef struct army {
  int army_template_id;
  struct faction *owner;
  unsigned short x, y;
  unsigned int troop[MAX_TROOPTYPE_AMOUNT]; //This is the actual amount of troops
  unsigned char movement_left;
  struct army *next;
} army;

typedef struct city {
  unsigned short x,y;
  unsigned int population;
  char name[NAME_LENGTH];
  struct city *next;
} city;

typedef struct faction {
  unsigned char id;
  unsigned int food;
  unsigned int ammo;
  unsigned int straglers;
  char name[NAME_LENGTH];
  army_template army_templates[MAX_ARMYTEMPLATE_AMOUNT];
  army *army_list;
  city *city_list;
  
  //struct faction *next;
} faction;

typedef struct
{
  int air_pressure;
  int temperature;
  tile *worldmap[WORLD_HEIGHT];
  faction *faction_list;
  unsigned char faction_count;
} world;

enum direction {
  N,
  NE,
  E,
  SE,
  S,
  SW,
  W,
  NW
};

/* Moved here from rp_gui_nc.h */
typedef struct map_cursor {
  int x,y;
} map_cursor;


#endif
