#include "rp_datatypes.h"
#include "rp_game_map.h"

extern world *world_p;

static struct {
  int flat;
} movecost;

typedef struct {
  int x;
  int y;
  int counter;
} pqcoord;

void rp_pathfind(army *mover, int dest_x, int dest_y)
{
  //Todo: later
}

//Returns 1 if successfully moved:
int rp_step_army(army *mover, enum direction dir)
{
  int y_mod = mover->y;
  int x_mod = mover->x;
  
  switch (dir) {
  case NW:
    y_mod--;
    x_mod--;
    break;
  case N:
    y_mod--;
    break;
  case NE:
    y_mod--;
    x_mod++;
    break;
  case E:
    x_mod++;
    break;
  case SE:
    y_mod++;
    x_mod++;
    break;
  case S:
    y_mod++;
    break;
  case SW:
    y_mod++;
    x_mod--;
    break;
  case W:
    x_mod--;
    break;
  }

  if (y_mod < 0 || y_mod >= WORLD_HEIGHT) {
    return 0;
  } else if (x_mod < 0) {
    x_mod += WORLD_WIDTH;
  } else if (x_mod >= WORLD_WIDTH) {
    x_mod -= WORLD_WIDTH;
  }

  tile *position = &(world_p->worldmap[mover->y][mover->x]);
  tile *destination = &(world_p->worldmap[y_mod][x_mod]);

  //Check if destination has foreign army:
  if (/*rp_get_owner(destination) != rp_get_owner(position) &&*/
      //there can not be a city and and army on the same tile!!!
      //...at least for now
      rp_get_armycity(destination)) {
    //todo: combat
    return 0; //return 1 if combat won and moved to tile
  }

  //Ignoring movement costs now for testing

  mover->x = x_mod;
  mover->y = y_mod;
  rp_set_armycity(destination,1);
  rp_set_owner(destination,rp_get_owner(position));
  rp_set_armycity(position,0);
  return 1;

}
