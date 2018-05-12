#ifndef RP_DATATYPES__
#define RP_DATATYPES__

#define WORLD_HEIGHT 50
#define WORLD_WIDTH 100

#define PACK __attribute__((packed))
#define PACKnALIGN __attribute__((packed,aligned(1)))
#define ALIGN __attribute__((aligned(1)))

typedef unsigned short tile;


typedef struct army {
  unsigned short x, y;
  //unsigned short movement_remain; Shuold be determined by troop
  unsigned int troop[10];
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


#endif
