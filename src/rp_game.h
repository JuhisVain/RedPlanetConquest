#include "rp_datatypes.h"

faction *rp_init_factions(int);
void rp_free_factions(void);
void rp_setup_factions(char *);

city *rp_setup_city(int, int, char*);

void rp_add_city(faction *owner, unsigned short x_coord, unsigned short y_coord,
		 int population, char cityname[20]);
void rp_add_city_rand(faction *owner, int population, char cityname[20]);

void rp_add_army(faction *, unsigned short, unsigned short, char template_id);


city *rp_city_search(faction *,int,int);
army *rp_army_search(faction *,int,int);
faction *rp_faction_search(int);

void rp_new_turn(void);
