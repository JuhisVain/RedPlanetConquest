#define _XOPEN_SOURCE_EXTENDED

#include "rp_gui_nc.h"
#include "rp_game_map.h"
#include "rp_game.h"
#include "rp_game_logic.h"

#include <signal.h>
#include <stdlib.h>

#include <curses.h>

void sig_handler(int);

world *world_p;

void start_game(void)
{

  //Initialize gui
  rp_init_gui();


  map_cursor *user_map_cursor = rp_setup_usermapcursor();
  map_cursor *map_cursor = rp_get_mapcursor();

  //Initialize game:
  //rp_init_factions(1); //moved to create_world in rp_game_map
  world_p = create_world();
  world_p->faction_list = rp_init_factions(4);
  world_p->faction_count = 4;

  rp_setup_factions("testi");

  rp_setup_movecost();

  rp_new_turn();


  //Main menu

  //Begin game

  wint_t input;
  while ( 1 ) {

    wget_wch(stdscr, &input);

    /*//TESTing statusline messages:
    char data[20];
    sprintf(data,"Got key: %u",input);
    rp_new_sl_msg(0, data);
    */
    
    switch (input) {
    case KEY_LEFT:
      rp_mc_left(map_cursor,1);
      break;
    case KEY_RIGHT:
      rp_mc_right(map_cursor,1);
      break;
    case KEY_UP:
      rp_mc_up(map_cursor,1);
      break;
    case KEY_DOWN:
      rp_mc_down(map_cursor,1);
      break;
    case KEY_SLEFT:
      rp_mc_left(map_cursor,10);
      break;
    case KEY_SRIGHT:
      rp_mc_right(map_cursor,10);
      break;
    case KEY_SR: //shift+up
      rp_mc_up(map_cursor,10);
      break;
    case KEY_SF: //shift+down
      rp_mc_down(map_cursor,10);
      break;
    case L'7':
      rp_mc_left(user_map_cursor,1);
      rp_mc_up(user_map_cursor,1);
      break;
    case L'8':
      rp_mc_up(user_map_cursor,1);
      break;
    case L'9':
      rp_mc_up(user_map_cursor,1);
      rp_mc_right(user_map_cursor,1);
      break;
    case L'4':
      rp_mc_left(user_map_cursor,1);
      break;
    case L'6':
      rp_mc_right(user_map_cursor,1);
      break;
    case L'1':
      rp_mc_left(user_map_cursor,1);
      rp_umc_down(user_map_cursor,1);
      break;
    case L'2':
      rp_umc_down(user_map_cursor,1);
      break;
    case L'3':
      rp_umc_down(user_map_cursor,1);
      rp_mc_right(user_map_cursor,1);
      break;
    case L'm':
      rp_expand_statusline();
      break;
    case 13: //ENTER
      rp_center_map_to_umc();
      rp_select_event();
      break;
    case 127: //<- backspace
      rp_deselect_event();
      break;
    case KEY_RESIZE:
      rp_term_resize();
      break;

    case L'0':
      goto endprogescape;
      break;

    default:
      input_key_debug(input);
      break;
    }
    flushinp();
    
    rp_draw_gui();

  }

 endprogescape:
  rp_end_gui();
  
}


void sig_handler(int signum)
{
  ungetch(KEY_RESIZE);
}
