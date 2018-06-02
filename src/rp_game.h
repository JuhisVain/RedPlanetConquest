#include "rp_datatypes.h"

faction *rp_init_factions(int);
void rp_free_factions(void);
void rp_setup_factions(char *);

city *rp_city_search(faction *,int,int);
army *rp_army_search(faction *,int,int);
faction *rp_faction_search(int);

void rp_new_turn(void);
