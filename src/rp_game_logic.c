#include "rp_datatypes.h"
#include "rp_game_map.h"
#include "rp_statline_msg.h"
#include <stdlib.h>

extern world *world_p;

static int movecost[TROOP_MOVE_CLASS_LENGTH][3]; //3=flat,rough,veryrough

typedef struct {
  int x;
  int y;
  int counter;
} pqcoord;

void rp_setup_movecost(void)
{
  movecost[INFANTRY][0] = 0;
  movecost[INFANTRY][1] = 0;
  movecost[INFANTRY][2] = 1;

  movecost[WHEEL][0] = 0;
  movecost[WHEEL][1] = 3;
  movecost[WHEEL][2] = 4;

  movecost[TRACK][0] = 1;
  movecost[TRACK][1] = 1;
  movecost[TRACK][2] = 3;

  movecost[HOVER][0] = 2;
  movecost[HOVER][1] = 2;
  movecost[HOVER][2] = 2;

  rp_new_sl_msg(0,"Movement costs set up.");
  
}

/* returned.y may be outside map */
map_cursor rp_tilecoord_at_dir(int source_x, int source_y, enum direction dir)
{
  int y_mod = source_y;
  int x_mod = source_x;
  
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

  /* Wrapping */
  if (x_mod < 0) {
    x_mod += WORLD_WIDTH;
  } else if (x_mod >= WORLD_WIDTH) {
    x_mod -= WORLD_WIDTH;
  }

  map_cursor ret;
  ret.x = x_mod;
  ret.y = y_mod;
  
  return ret;
}

int rp_step_movecost(army *mover, enum direction dir)
{
  int cost;

  /* Diagonal movement costs 3 units */
  if (dir == N || dir == E || dir == S || dir == W) {
    cost = 2;
  } else {
    cost = 3;
  }

  map_cursor dest_c = rp_tilecoord_at_dir(mover->x,mover->y,dir);;
  int highest_cost = 0;
  for (int i=0; i<MAX_TROOPTYPE_AMOUNT; i++) {

    //dest_c = rp_tilecoord_at_dir(mover->x,mover->y,dir);
    
    int resource = rp_get_resource( &(world_p->worldmap[dest_c.y][dest_c.x]) );
    
    /* Consider everything that is not 'flat' or 'rough' as 'very rough' */
    if (resource > 2) { //Todo: rivers
      resource = 2;
    }

    int temp_cost = movecost[
      mover->owner->army_templates[mover->army_template_id].troop[i].movement
    ][resource];

    if (temp_cost > highest_cost) {
      highest_cost = temp_cost;
    }
    
  }

  cost += highest_cost;

  /* If moving uphill */
  if (rp_get_height( &(world_p->worldmap[dest_c.y][dest_c.x]))
      > rp_get_height(&(world_p->worldmap[mover->y][mover->x])) ) {
    cost++;
  }

  return cost;
}

void rp_pathfind(army *mover, int dest_x, int dest_y)
{

  /* TODO:  even later
  int sx = mover->x, sy = mover->y;
  int dx = dest_x, dy = dest_y;

  int arr_width, arr_height;

  // Handle wrapping 
  if ( WORLD_WIDTH - dest_x + sx < dest_x - sx ) {
    arr_width = WORLD_WIDTH - dest_x + sx +1;
  } else {
    arr_width = dest_x -sx +1;
  }

  // A bit of slack in case we should move away from destination 
  #define SLACK 4
  arr_width += SLACK;
  arr_height = abs(dest_x-sx)+1+SLACK;

  int cost[arr_width][arr_height];

  for (int i=0; i<arr_width; i++) {
    for (int j=0; j<arr_height; j++) {
      cost[i][j] = 9999;
    }
  }
  */
  
}

//Returns 1 if successfully moved:
int rp_step_army(army *mover, enum direction dir)
{

  tile *position = &(world_p->worldmap[mover->y][mover->x]);

  map_cursor dest_coord = rp_tilecoord_at_dir(mover->x,mover->y,dir);
  /* dest_coord.y might be outside map */
  if (dest_coord.y < 0 || dest_coord.y >= WORLD_HEIGHT) {

    rp_new_sl_msg(0,"Unable to move outside map!");
    
    return 0;
  }
  
  tile *destination = &(world_p->worldmap[dest_coord.y][dest_coord.x]);

  /* Check if destination has foreign army: */
  if (/*rp_get_owner(destination) != rp_get_owner(position) &&*/
      //there can not be a city and and army on the same tile!!!
      //...at least for now
      rp_get_armycity(destination)) {
    //todo: combat

    //temporary:
    rp_new_sl_msg(0,"Combat not implemented yet!");
    
    return 0; //return 1 if combat won and moved to tile
  }

  int cost_of_move = rp_step_movecost(mover,dir);

  if (mover->movement_left >= cost_of_move) {
    mover->movement_left -= cost_of_move;
  } else {
    rp_new_sl_msg(0,"No more movement points left!");
    return 0;
  }

  mover->x = dest_coord.x;
  mover->y = dest_coord.y;
  rp_set_armycity(destination,1);
  rp_set_owner(destination,rp_get_owner(position));
  rp_set_armycity(position,0);

  char data[64];
  sprintf(data,"Army moved to (%d,%d) using %d mps: %d left."
	  ,dest_coord.x,dest_coord.y,cost_of_move,mover->movement_left);
  rp_new_sl_msg(0,data);
  
  return 1;

}

