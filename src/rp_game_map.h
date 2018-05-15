
#ifndef RP_GAME_MAP_H__
#define RP_GAME_MAP_H__

#include "rp_datatypes.h"
world *create_world(void);
void free_world(world*);

unsigned short rp_get_hrid(const tile *);
unsigned short rp_get_hoid(const tile *);
unsigned short rp_get_resource(const tile *);
unsigned short rp_get_height(const tile *);
unsigned short rp_get_armycity(const tile *);
unsigned short rp_get_owner(const tile *);
unsigned short rp_get_ownerNEW(const tile);

unsigned short rp_get_ac_owner_height(const tile *);

void rp_set_resource(tile *, unsigned short);
void rp_set_height(tile *, unsigned short);
void rp_set_armycity(tile *, unsigned short);
void rp_set_owner(tile *, unsigned short);


#endif
