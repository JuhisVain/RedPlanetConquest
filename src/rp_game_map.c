#include <stdlib.h>
#include <stdio.h>
#include "rp_game_map.h"
#include "rp_game.h"

#include <curses.h>

void gen_map_i(tile**);

#define HRID_MASK 0x3FF
#define RESOURCE_MASK 0x3F
#define HEIGHT_MASK 0x3C0
#define ARMYCITY_MASK 0x8000
#define OWNER_MASK 0x7C00

unsigned short rp_get_hrid(const tile *source)
{
  return *source & HRID_MASK;
}
unsigned short rp_get_hoid(const tile *source)
{
  return *source & (HEIGHT_MASK | OWNER_MASK);
}
unsigned short rp_get_resource(const tile *source)
{
  return *source & RESOURCE_MASK;
}
unsigned short rp_get_height(const tile *source)
{
  return (*source & HEIGHT_MASK)>>6;
}
unsigned short rp_get_armycity(const tile *source)
{
  return (*source & ARMYCITY_MASK)>>15;
}
unsigned short rp_get_owner(const tile *source)
{
  return (*source & OWNER_MASK)>>10;
}
unsigned short rp_get_ownerNEW(const tile source)
{
  return (source & OWNER_MASK)>>10;
}
unsigned short rp_get_ac_owner_height(const tile *source)
{
  return *source & (OWNER_MASK | ARMYCITY_MASK | HEIGHT_MASK);
}

void rp_set_resource(tile *source, unsigned short newres)
{
  newres &= RESOURCE_MASK;
  *source = (*source & (~RESOURCE_MASK)) + newres;
}
void rp_set_height(tile *source, unsigned short newhei)
{
  newhei <<= 6;

  newhei &= HEIGHT_MASK;
  *source = (*source & (~HEIGHT_MASK)) + newhei;
}
void rp_set_armycity(tile *source, unsigned short newac)
{
  newac <<= 15;
  newac &= ARMYCITY_MASK;
  *source = (*source & (~ARMYCITY_MASK)) + newac;
}
void rp_set_owner(tile *source, unsigned short newown)
{
  newown <<= 10;
  newown &= OWNER_MASK;
  *source = (*source & (~OWNER_MASK)) + newown;
}

world *create_world(void)
{
  world *cur_world = malloc(sizeof(world));

  cur_world->temperature = -50;
  cur_world->air_pressure = 1;

  for (int i = 0; i < WORLD_HEIGHT; i++){
    cur_world->worldmap[i] = malloc(sizeof(tile)*WORLD_WIDTH);
  }

  gen_map_i(cur_world->worldmap);
  return cur_world;
}

void gen_map_i(tile **in)
{

  //Get height data from pgm file
  FILE *h_source = fopen("Graphics/mhmap.pgm","r");
  char read;
  //these should be read from the file:
  //int width=WORLD_WIDTH, height=WORLD_HEIGHT, depth=255;

  //Get to the heights part:
  for(int count = 0;fread(&read,1,1,h_source), count < 4;) {
    if (read == 10) {
      count++;
    }
  }
  
  for (int y = 0; y < WORLD_HEIGHT; y++) {
    for (int x = 0; x < WORLD_WIDTH; x++) {

      unsigned short tempres = rand()%10;
      if (tempres < 9) tempres = rand()%3;
      else tempres = 3+rand()%5;

      rp_set_resource(&(in[y][x]), tempres);

      unsigned short temphei = ((unsigned short)read/(unsigned short)16);
      fread(&read,1,1,h_source);

      rp_set_height(&(in[y][x]), temphei);
      
      rp_set_owner(&(in[y][x]),0);

    }
  }

  fclose(h_source);
}

//Keep on rockin' in the
void free_world(world *x)
{
  for (int i = 0; i < WORLD_HEIGHT; i++){
    free(x->worldmap[i]);
  }
  free(x);
}
