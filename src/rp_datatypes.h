#ifndef RP_DATATYPES__
#define RP_DATATYPES__

#define WORLD_HEIGHT 50
#define WORLD_WIDTH 100

#define PACK __attribute__((packed))
#define PACKnALIGN __attribute__((packed,aligned(1)))
#define ALIGN __attribute__((aligned(1)))

typedef unsigned short tile;



enum troop_move_class {
  INFANTRY,
  WHEEL,
  TRACK,
  HOVER
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
  char name[10];
  //these enums should be packed in a single variable
  enum troop_move_class movement;
  enum troop_weapon_class weapon;
  enum troop_armor_class armor;
  enum troop_ability_class ability; //Should be possible to have more than one
  char vision_range;
} troop_type;

typedef struct army_template {
  char id;
  char name[10];
  troop_type troop[10];
  unsigned int default_troop_count[10];
  
} army_template;

typedef struct army {
  char army_template_id;
  unsigned short x, y;
  unsigned int troop[10]; //This is the actual amount of troops
  unsigned char movement_left;
  struct army *next;
} army;

typedef struct city {
  unsigned short x,y;
  unsigned int population;
  char name[20];
  struct city *next;
} city;

typedef struct faction {
  unsigned char id;
  unsigned int food;
  unsigned int ammo;
  unsigned int straglers;
  char name[20];
  army_template army_templates[10];
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


#endif
