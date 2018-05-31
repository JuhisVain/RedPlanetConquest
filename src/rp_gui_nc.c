#define _XOPEN_SOURCE_EXTENDED

#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "rp_game_map.h"
#include "rp_gui_nc.h"
#include "rp_game.h"
#include "rp_game_logic.h"

#include "rp_gui_nc_data.h"

#define PANELSIZE 20


void rp_update_panel(tile*);
void rp_update_statusline();
void rp_term_resize();

WINDOW *create_map_window(void);
WINDOW *create_panel_window(void);
WINDOW *create_statusline_window(void);

wchar_t res_sym(unsigned int);

WINDOW *map;
WINDOW *panel;

WINDOW *statusline;
stat_msg *newest_msg = NULL;
//static unsigned int statusmode = 0;//Let's say 0 is unexpanded

/*
static unsigned int mapmode = 0;
#define MM_VIEWCONTROL 0
#define MM_ARMYSELECTED 1
#define MM_CITYSELECTED 2
*/

//static world *world_p;
extern world *world_p;

//left top corner of map:
static map_cursor mc;
//cursor to point at specific tiles
static map_cursor umc;
static map_cursor s_umc; //secondary umc

//Colors for heights

#define C_H0 9
#define C_H1 10
#define C_H2 11
#define C_H3 12
#define C_H4 13
#define C_H5 14
#define C_H6 15
#define C_H7 16
#define C_H8 17
#define C_H9 18
#define C_H10 19
#define C_H11 20
#define C_H12 21
#define C_H13 22
#define C_H14 23
#define C_H15 24

//Colors for resources
#define C_RESOURCE 25
#define C_HR 26
#define C_HVR 27
#define C_ALUMINIUM 28
#define C_TITANIUM 29
#define C_CARBON 30
#define C_CAVES 31
#define C_WATERICE 32

#define C_FACTION_1 128


//other colors:
#define CP_UMC 1024 //Background colorpair for user map cursor

void rp_init_gui(void)
{

  //mapcursor for drawing desireable view of map
  mc.x = 0; mc.y = 0;

  //ncurses init:
  initscr();
  start_color();
  //cbreak();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  refresh();
  nonl();
  curs_set(0); //Cursor visibilty

  //init nc colors:
  //height levels:
  const int low_red=831, low_gre=643, low_blu=439;
  //const int med_red=639, med_gre=184, med_blu=0;
  const int med_red=635, med_gre=220, med_blu=0;
  const int hig_red=180, hig_gre=110, hig_blu=90;

  
  for (int i = 0; i < 8; i++) {
    init_color((C_H0)+i, low_red+((med_red-low_red)/8)*i,
	               low_gre+((med_gre-low_gre)/8)*i,
	               low_blu+((med_blu-low_blu)/8)*i);
  }
  for (int i = 0; i < 8; i++) {
    init_color((C_H8)+i, med_red+((hig_red-med_red)/8)*i,
	               med_gre+((hig_gre-med_gre)/8)*i,
	               med_blu+((hig_blu-med_blu)/8)*i);
  }

  //Adittional roughness "height" colors:
  init_color(C_RESOURCE,hig_red+((hig_red-med_red)/8),
	          hig_gre+((hig_gre-med_gre)/8),
	          hig_blu+((hig_blu-med_blu)/8));
  init_color(C_HR,hig_red+((hig_red-med_red)/8)*2,
	          hig_gre+((hig_gre-med_gre)/8)*2,
	          hig_blu+((hig_blu-med_blu)/8)*2);
  init_color(C_HVR,hig_red+((hig_red-med_red)/8)*3,//Possibly not needed
	          hig_gre+((hig_gre-med_gre)/8)*3,
	          hig_blu+((hig_blu-med_blu)/8)*3);

  //resources:
  init_color(C_ALUMINIUM,537,537,600);
  init_color(C_TITANIUM, 537,600,565);
  init_color(C_CARBON  , 122, 94,  0);
  init_color(C_CAVES   , 239,212,176);
  init_color(C_WATERICE, 906,847,867);

  tile pair = 0;
  
  for (int i=0; i<16; i++) {
    rp_set_height(&pair, i);
    for (int j=0; j<16; j++) {
      rp_set_resource(&pair,j);
      //+1 in the following: color pair 0 not allowed
      if (j > 2){ //if NOT smooth, rough or veryrough
	init_pair(1+rp_get_hrid(&pair),C_RESOURCE+j,C_H0+i);
	//init_pair(1+pair,C_RESOURCE+j,C_H0+i);
      } else { 
	init_pair(1+rp_get_hrid(&pair),C_H0+i+2,C_H0+i);
	//init_pair(1+pair,C_H0+i+2,C_H0+i);
      }
    }
  }
  
  //Faction colors:
  init_color(C_FACTION_1,600,60,600);
  //debug:
  for (int i=1; i<32; i++) {  
    init_color(C_FACTION_1+i,500,400,0);
  }

  init_color(C_FACTION_1+1,800,0,800);
  init_color(C_FACTION_1+2,0,800,0);
  init_color(C_FACTION_1+3,0,750,750);

  pair = 0;
  for (int i=0; i<16; i++) {
    rp_set_height(&pair, i);
    rp_set_armycity(&pair,1);
      for (int j=0; j<32; j++) {
	rp_set_owner(&pair,j);
	init_pair(1+ rp_get_hoid(&pair),C_FACTION_1+j,C_H0+i);
      }
    
  }


  //Other colors & pairs:
  init_pair(CP_UMC,COLOR_WHITE,COLOR_BLUE);

  map = create_map_window();
  panel = create_panel_window();
  statusline = create_statusline_window();
  

  wrefresh(map);
  wrefresh(statusline);
  wrefresh(panel);

  getch();

}



void rp_expand_statusline(void)
{

  int  viewheight;
  //int viewwidth;
  //getmaxyx(stdscr,viewheight, viewwidth);
  viewheight = getmaxy(stdscr);

  //int mapheight;
  int mapwidth;
  //getmaxyx(map, mapheight, mapwidth);
  mapwidth = getmaxx(map);
  
  if (!UM_STATLINE_MAXIMIZED) {
    int newheight = viewheight * 0.8;
    //statusmode = 1;
    UM_MAX_STATLINE;
    wresize(map, viewheight-newheight,mapwidth);//resize mapview
    mvwin(statusline,viewheight-newheight,0);
    wresize(statusline,newheight,mapwidth);
  } else if (UM_STATLINE_MAXIMIZED) {
    //statusmode = 0;
    UM_MIN_STATLINE;
    wresize(map, viewheight-1,mapwidth);
    wresize(statusline,1,mapwidth);
    mvwin(statusline,viewheight-1,0); //Move to bottom of screen
  }
  
}

map_cursor *rp_setup_usermapcursor(void)
{
  int viewheight, viewwidth;
  getmaxyx(map,viewheight, viewwidth);
  umc.x = viewwidth/2;
  umc.y = viewheight/2;
  return &umc;
}

map_cursor *rp_get_mapcursor(void)
{
  return &mc;
}

tile *rp_umc_tile(const map_cursor *const inmc)
{
  int mapheight, mapwidth;
  getmaxyx(map,mapheight,mapwidth);

  tile *tile_under_cursor =
    &(world_p->worldmap[inmc->y][inmc->x]);

  cchar_t cchar_under_cursor;

  int xonmap = inmc->x-mc.x;
  
  if (xonmap < 0) {
    xonmap +=WORLD_WIDTH;
  }

  int mody = inmc->y-mc.y;

  //debig text:
  char data[20];
  sprintf(data,"xom:%d,my:%d  ",xonmap,mody);
  mvwaddstr(panel,3,3,data);

  if (UM_ARMY_IS_SELECTED && (inmc != &s_umc) ) {
    tile_under_cursor = rp_umc_tile(&s_umc);
  }

  //if cursor is outside map view:
  if (xonmap >= mapwidth || mody < 0 || mody >= mapheight) {
    //Do nothing
  } else {

    //Extract wide char from screen:
    mvwin_wch(map, mody, xonmap,&cchar_under_cursor);
    
    //cchar_under_cursor.attr = A_BLINK;//attr holds effects. ps. A_BOLD is bad
    cchar_under_cursor.ext_color = CP_UMC;//ext_color holds colorpair id number
    wadd_wch(map,&cchar_under_cursor);
    //repeat umc graphics if view is larger than world width:
    if (xonmap+WORLD_WIDTH <= mapwidth) {
      mvwadd_wch(map, mody,xonmap+WORLD_WIDTH,&cchar_under_cursor);
    }
    
  }

  return tile_under_cursor;
}

//Map cursor movement:
void rp_mc_left(map_cursor *cu, int n)
{
  cu->x -= n;
  if (cu->x < 0) {
    cu->x += WORLD_WIDTH;
  }
}

void rp_mc_right(map_cursor *cu,int n)
{
  cu->x += n;
  if (cu->x >= WORLD_WIDTH) {
    cu->x -= WORLD_WIDTH;
  }
}

void rp_mc_up(map_cursor *cu,int n)
{
  if ( cu->y - n >= 0){
    cu->y -= n;
  } else {
    cu->y = 0;
  }
}

void rp_mc_down(map_cursor *cu,int n)
{
  int viewheight;
  //int trash;
  //getmaxyx(map,viewheight,trash);
  viewheight = getmaxy(map);

  if ( cu->y + n + viewheight <= WORLD_HEIGHT ){
    cu->y += n;
  } else {
    cu->y = WORLD_HEIGHT - viewheight;
  }
}

void rp_umc_down(map_cursor *cu, int n)
{
  if ( cu->y + n < WORLD_HEIGHT ){
    cu->y += n;
  }
}
/*
void store_world(world *w_p)
{
  world_p = w_p;
}
*/

void rp_draw_gui()
{
  wmove(map,0,0); //curses cursor to start position

  int viewheight, viewwidth;
  getmaxyx(map, viewheight, viewwidth);
  
  int i;
  
  for (i = 0; i < viewheight; i++) {
    int bakedi = i + mc.y;
    if (bakedi >= WORLD_HEIGHT) break;
    for (int j = 0; j < viewwidth; j++){
      int bakedj = j + mc.x;
      //If our window is large enough, we will need to wrap the map
      while (bakedj >= WORLD_WIDTH) {
	bakedj -= WORLD_WIDTH;
      } //mc.x cannot be negative, so no need to do same for that

      //wattron(map,COLOR_PAIR(rp_get_hrid(&(world_p->worldmap[bakedi][bakedj]))));
      wchar_t symbol = '!';
      unsigned short colorpairid = 1;

      //If army or city iterate through those:
      if (rp_get_armycity(&(world_p->worldmap[bakedi][bakedj]))) {

	faction *fact = rp_faction_search(rp_get_owner(&(world_p->worldmap[bakedi][bakedj])));
	
	army *found_army = rp_army_search(fact,bakedj,bakedi);
	if (found_army != NULL) {
	  symbol = L'A';
	  //break;
	}
	
	city *found_city = rp_city_search(fact,bakedj,bakedi);
	if (found_city != NULL) {
	  symbol = L'X';
	  //break;
	}

      } else {
	symbol = res_sym(rp_get_resource(&(world_p->worldmap[bakedi][bakedj])));
      }
      
      cchar_t ch = {0,{symbol},0};
      if (rp_get_armycity(&(world_p->worldmap[bakedi][bakedj]))) {
	colorpairid = 1 + rp_get_hoid(&(world_p->worldmap[bakedi][bakedj]));
      } else {
	colorpairid = 1 + rp_get_hrid(&(world_p->worldmap[bakedi][bakedj]));
      }
      wattr_set(map,0,colorpairid,NULL);
	//cchar_t ch = {0,{symbol},colorpairid};
      
      cchar_t *ch_p = &ch;

      wadd_wch(map, ch_p);
    }
  }

  char mc_pos[20];
  sprintf(mc_pos, "x: %d, y: %d ",mc.x,mc.y);
  mvwaddstr(panel ,2,2, mc_pos);

  char umc_pos[20];
  sprintf(umc_pos, "x: %d, y: %d ",umc.x,umc.y); 
  mvwaddstr(panel ,5,2, umc_pos);

  //These things should only be done when something changes:
  rp_update_panel(rp_umc_tile(&umc));
  rp_update_statusline();

  
  wrefresh(panel);
  wrefresh(map);
  wrefresh(statusline);

}

  

void rp_update_statusline(void)
{
  int viewheight, viewwidth,begheight, begwidth;
  getbegyx(map, begheight, begwidth);
  getmaxyx(map, viewheight, viewwidth);

  char xxx[100];
  sprintf(xxx, "sl: begy:%d begx: %d, maxy: %d maxx: %d ",begheight,begwidth,viewheight,viewwidth);

  wmove(statusline,0,0);
  wattron(statusline, COLOR_PAIR(CP_UMC));
  //for (int i = 0; i < viewheight; i++) {
  //for (int j = 0; j < viewwidth; j++) {
      waddstr(statusline,xxx);
      //}
      //}
}

void rp_update_panel(tile *source_tile)
{
  char data[20];
  sprintf(data, "Al:%u Re:%u",
	  rp_get_height(source_tile),
	  rp_get_resource(source_tile));
  mvwaddstr(panel,6,1,data);
  
  if (rp_get_armycity(source_tile)) {

    faction *fact = rp_faction_search(rp_get_owner(source_tile));
    army *found_army;
    city *found_city;
    
    if ( (found_army = rp_army_search(fact,umc.x,umc.y)) ) {

      //TODO: redo once troop types are figured out:
      sprintf(data, "%u  ", found_army->troop[0]);
      mvwaddstr(panel,9,1,data);
      sprintf(data, "%u  ", found_army->troop[1]);
      mvwaddstr(panel,10,1,data);
      sprintf(data, "%u  ", found_army->troop[2]);
      mvwaddstr(panel,11,1,data);
      sprintf(data, "%u  ", found_army->troop[3]);
      mvwaddstr(panel,12,1,data);
      sprintf(data, "%u  ", found_army->troop[4]);
      mvwaddstr(panel,13,1,data);
      sprintf(data, "%u  ", found_army->troop[5]);
      mvwaddstr(panel,14,1,data);
      sprintf(data, "%u  ", found_army->troop[6]);
      mvwaddstr(panel,15,1,data);
      sprintf(data, "%u  ", found_army->troop[7]);
      mvwaddstr(panel,16,1,data);
      sprintf(data, "%u  ", found_army->troop[8]);
      mvwaddstr(panel,17,1,data);
      sprintf(data, "%u  ", found_army->troop[9]);
      mvwaddstr(panel,18,1,data);
      
    } else if ( (found_city = rp_city_search(fact,umc.x,umc.y)) ) {

      sprintf(data, "%s", found_city->name);
      mvwaddstr(panel,9,1,data);
      
    }
    /*
    //END
    city *found_city;

    for (int i = 0; i < world_p->faction_count; i++) {
      faction fact = world_p->faction_list[i];
      found_city = rp_city_search(&fact,umc.x,umc.y);
      if (found_city != NULL) {
	break;
      }
    }

    sprintf(data, "%s", found_city->name);
    mvwaddstr(panel,9,1,data);
    */
  }
  
  sprintf(data, "%u   \n %d    ",*source_tile,*source_tile);
  mvwaddstr(panel,7,1,data);
}

wchar_t res_sym(unsigned int resource)
{

  switch(resource) {
  case 0: return L' ';
  case 1: return L'.';
    //LIKE THIS: case 1: return L'\u2522';
  case 2: return L':';
  case 3: return L'a';
  case 4: return L't';
  case 5: return L'c';
  case 6: return L'o';
  case 7: return L'#';
  default: return L'?';
  }
}
 
void draw_map(void)
{
  
}
/*
map_cursor worldx_to_scrx(int x)
{
  
  return world_p->worldmap[]
}
*/
//These two should be somewhere else:



WINDOW *create_map_window(void)
{
  WINDOW *ret_win;
  //ret_win = newwin(LINES-PANELSIZE, COLS,0,0);
  ret_win = newwin(LINES, COLS-PANELSIZE,0,0);

  box(ret_win,0,0);
  wrefresh(ret_win);

  return ret_win;
}

WINDOW *create_panel_window(void)
{
  WINDOW *ret_win;
  //ret_win = newwin(PANELSIZE, COLS,LINES-PANELSIZE,0);
  ret_win = newwin(LINES, PANELSIZE, 0, COLS-PANELSIZE);

  box(ret_win,0,0);
  wrefresh(ret_win);
  
  return ret_win;
}

WINDOW *create_statusline_window(void)
{
  WINDOW *ret_win;
  ret_win = newwin(1,COLS-PANELSIZE,LINES-1,0);
  box(ret_win,0,0);
  wrefresh(ret_win);
  return ret_win;
}

void rp_end_gui(void)
{
  endwin();
}

void rp_term_resize()
{

  if (UM_STATLINE_MAXIMIZED) {
    rp_expand_statusline();
  }

  wresize(map,LINES,COLS-PANELSIZE);
  wresize(panel,LINES,PANELSIZE);
  wresize(statusline,1,COLS-PANELSIZE);
  
  mvwin(map, 0,0);
  mvwin(panel ,0,COLS-PANELSIZE);
  mvwin(statusline,LINES,0);

  char tests[40];
  sprintf(tests, "lines-ps:%d,columns: %d :", LINES-PANELSIZE, COLS);
  mvwaddstr(panel, 3,1, tests);
  wrefresh(panel);
  refresh();
}

void rp_select_event(void)
{
  
  if (rp_get_armycity(&(world_p->worldmap[umc.y][umc.x]))) {
    
    army *found_army;
  
    found_army = rp_army_search(
		   rp_faction_search(
		     rp_get_ownerNEW( world_p->worldmap[umc.y][umc.x] )),
		   umc.x, umc.y);

    if (found_army != NULL) {
      //UM_S_ARMY;
      //Army selection stuff here
      rp_army_selected_input(found_army);
      return;
    }
    
    city *found_city;
    if ((found_city = rp_city_search(
			rp_faction_search(
			  rp_get_owner(&(world_p->worldmap[umc.y][umc.x]))),
			umc.x, umc.y) ))
    {
      UM_S_CITY;
      //City selection stuff here
      return;
    }
    
  } else { //Nothing to select here
    //Maybe an error message here
  }
}

void rp_army_selected_input(army *selected_army)
{

  while (1) { //single tile step movement
    wint_t input;
    wget_wch(stdscr, &input);

    switch(input) {
    case L'8':
      rp_step_army(selected_army,N);
      break;
    case L'9':
      rp_step_army(selected_army,NE);
      break;
    case L'6':
      rp_step_army(selected_army,E);
      break;
    case L'3':
      rp_step_army(selected_army,SE);
      break;
    case L'2':
      rp_step_army(selected_army,S);
      break;
    case L'1':
      rp_step_army(selected_army,SW);
      break;
    case L'4':
      rp_step_army(selected_army,W);
      break;
    case L'7':
      rp_step_army(selected_army,NW);
      break;
    case L'5': //Go to multitile movement mode
      rp_multitile_movement(selected_army);
      break;
    case L'0':
      goto exitloop;
      break;
    default:
      break;
    }

    //Make umc follow army:
    umc.x = selected_army->x;
    umc.y = selected_army->y;

    rp_center_map_to_umc();
    
    rp_draw_gui();

  }

 exitloop:
  rp_deselect_event();
  return;
}

//returns 1 if moved:
int rp_multitile_movement(army *selected_army)
{

  UM_S_ARMY;
  
  s_umc.x = umc.x;
  s_umc.y = umc.y;
  
  while (1) { //Multitile movement using secondary user map cursor
    wint_t input;
    wget_wch(stdscr, &input);

    switch(input) {
    case L'8':
      rp_mc_up(&s_umc,1);
      break;
    case L'9':
      rp_mc_up(&s_umc,1);
      rp_mc_right(&s_umc,1);
      break;
    case L'6':
      rp_mc_right(&s_umc,1);
      break;
    case L'3':
      rp_mc_right(&s_umc,1);
      rp_umc_down(&s_umc,1);
      break;
    case L'2':
      rp_umc_down(&s_umc,1);
      break;
    case L'1':
      rp_mc_left(&s_umc,1);
      rp_umc_down(&s_umc,1);
      break;
    case L'4':
      rp_mc_left(&s_umc,1);
      break;
    case L'7':
      rp_mc_up(&s_umc,1);
      rp_mc_left(&s_umc,1);
      break;
      
    case L'0':
      UM_DS_ARMY;
      return 0;
      break;
    default:
      break;
    }

    rp_draw_gui();
    
  }
}

void rp_deselect_event(void)
{
  UM_DS_ARMY;
  UM_DS_CITY;
}

void rp_center_map_to_umc(void)
{
  int mapy, mapx;
  getmaxyx(map, mapy, mapx);
  mc.x = (umc.x - mapx/2);
  while (mc.x < 0) {
    mc.x += WORLD_WIDTH;
  }
  mc.y = (umc.y - mapy/2);
  if (mc.y < 0) {
    mc.y = 0;
  } else if (mc.y > WORLD_HEIGHT - mapy) {
    mc.y = WORLD_HEIGHT - mapy;
  }

}


//debug:
void input_key_debug(int input)
{
  char xxx[20];
  sprintf(xxx, "%d   ",input);
  mvwaddstr(panel,10,2,xxx);
}
