#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rp_datatypes.h"
#include "rp_game_map.h"

city *rp_setup_city(int, int, char*);
void rp_add_city(faction *, unsigned short, unsigned short,int, char[20]);
void rp_add_army(faction *, unsigned short, unsigned short, char template_id);

extern world *world_p;

faction *factions = NULL;
unsigned char faction_count; //moved to world struct, remove

faction *rp_init_factions(int n)
{
  faction_count = n;
  factions = malloc(sizeof(faction) * faction_count);
  return factions;
}
void rp_free_factions(void)
{
  free(factions);
}

void rp_setup_army_templates(faction *fact)
{
  for (int i=0; i<10; i++) {
    fact->army_templates[i].id = i;
    strcpy(fact->army_templates[i].name, "DEF");

    troop_type temp_tt;
    strcpy(temp_tt.name, "DEF_TT");
    temp_tt.movement = INFANTRY;
    temp_tt.weapon = SMALL;
    temp_tt.armor = NONE;
    temp_tt.ability = NO_SPECIAL;
    temp_tt.vision_range = 2;
    
    for (int j=0; j<10; j++) {
      fact->army_templates[i].troop[j] = temp_tt;
      fact->army_templates[i].default_troop_count[j] = 3000;
    }
    
  }
}

void rp_setup_factions(char playername[20])
{
  sprintf(factions[0].name, "%s", playername);
  factions[0].id = 0;
  factions[0].food = 0;
  factions[0].ammo = 0;
  factions[0].straglers = 0;

  //Create a default testing template:
  rp_setup_army_templates(&(factions[0]));
  
  factions[0].army_list = NULL;
  factions[0].city_list = NULL;

  rp_add_city(&(factions[0]),5,10,100,"testi1");
  rp_add_city(&(factions[0]),30,45,100,"testi2");
  rp_add_city(&(factions[0]),56,25,100,"testi3");

  
  rp_add_army(&(factions[0]),22,22, 0);

  
  for (int i = 1; i < faction_count; i++) {
    sprintf(factions[i].name, "AI-fact-%d", i);
    factions[i].id = i;
    factions[i].food = 0;
    factions[i].ammo = 0;
    factions[i].straglers = 0;
    rp_setup_army_templates(&(factions[i]));
    factions[i].army_list = NULL;
    factions[i].city_list = NULL;
  }

  
  //This will segfault if factions dont exist
  rp_add_city(&(factions[1]),15,10,100,"AItesti1");
  rp_add_city(&(factions[2]),16,12,100,"AItesti2");
  rp_add_city(&(factions[3]),17,14,100,"AItesti3");
}

//Same stuff as in rp_add_city:
//TODO: check if tile xy already has something in it
void rp_add_army(faction *fact ,unsigned short in_x, unsigned short in_y, char template_id)
{
  army *csa;

  if (fact->army_list == NULL) {
    fact->army_list = malloc(sizeof(army));
    csa = fact->army_list;
    csa->x = in_x; csa->y = in_y;
    csa->army_template_id = template_id;
    csa->troop[0] = 200;
    
    csa->next = NULL;
  } else {

    csa = fact->army_list;

    while (1) {
      if (csa->next != NULL) {
	csa = csa->next;
      } else {
	csa->next = malloc(sizeof(city));
	csa = csa->next;
	csa->x = in_x; csa->y = in_y;
	csa->army_template_id = template_id;
	csa->troop[0] = 200;
	
	csa->next = NULL;
	break;
      }
    }
  }

  //Modify world map data:
  rp_set_armycity( &(world_p->worldmap[in_y][in_x]) ,1);
  rp_set_owner( &(world_p->worldmap[in_y][in_x]) ,fact->id);
  return;
  
}


void rp_add_city(faction *fact ,unsigned short in_x, unsigned short in_y,
		 int in_pop, char in_name[20])
{
  //TODO:Make this function a recursive thing later OR combine with rp_setup_city
  city *csc; 

  if (fact->city_list == NULL) {
      
    fact->city_list = malloc(sizeof(city));
    csc = fact->city_list;
    csc->x = in_x; csc->y = in_y;
    csc->population = in_pop;
    strcpy(csc->name, in_name);
    csc->next = NULL;
      
  } else {

    csc = fact->city_list;

    while (1) {
      if (csc->next != NULL) {
	csc = csc->next;
      } else {

	csc->next = malloc(sizeof(city));
      
	csc = csc->next;
	csc->x = in_x; csc->y = in_y;
	csc->population = in_pop;
	strcpy(csc->name, in_name);
	csc->next = NULL;
	break;
      
      }
    }
  }
  //Modify world map data:
  rp_set_armycity( &(world_p->worldmap[in_y][in_x]) ,1);
  rp_set_owner( &(world_p->worldmap[in_y][in_x]) ,fact->id);
  return;
  
}

//obsolete:
city *rp_setup_city(int x, int y, char *name)
{
  city *ret = malloc(sizeof(city));
  ret->x = x;
  ret->y = y;
  sprintf(ret->name,"%s",name);
  ret->next = NULL;
  return ret;
}

//Returns pointer to city at (x,y)
city *rp_city_search(faction *fact, int x, int y)
{
  city *ptr = fact->city_list;
  while (ptr != NULL) {
    if (ptr->x == x && ptr->y == y) {
      return ptr;
    } else {
      ptr = ptr->next;
    }
  }
  return NULL;
}

army *rp_army_search(faction *fact,int x,int y)
{
  
  army *ptr = fact->army_list;
  while (ptr != NULL) {
    if (ptr->x == x && ptr->y == y) {
      return ptr;
    } else {
      ptr = ptr->next;
    }
  }
  
  return NULL;
}

faction *rp_faction_search(int fact_id)
{
  for (int i=0; i<faction_count; i++) {
    if (factions[i].id == fact_id) {
      return &(factions[i]);
    }
  }
  
  return NULL;
}
