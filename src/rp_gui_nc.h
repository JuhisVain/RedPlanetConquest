#include "rp_datatypes.h"

void rp_init_gui(void);
void rp_end_gui(void);
void rp_term_resize();
void store_world(world*);
void rp_draw_gui(void);
void rp_mc_left(map_cursor*,int);
void rp_mc_right(map_cursor*,int);
void rp_mc_up(map_cursor*,int);
void rp_mc_down(map_cursor*,int);
void rp_umc_down(map_cursor*,int);
map_cursor *rp_setup_usermapcursor(void);
map_cursor *rp_get_mapcursor(void);

void rp_expand_statusline(void);

void rp_select_event(void);
void rp_deselect_event(void);
void rp_center_map_to_umc(void);

void rp_army_selected_input(army *);
int rp_multitile_movement(army *selected_army);

void rp_new_sl_msg(unsigned int par_flag, char *text);

void input_key_debug(int);
