#define _XOPEN_SOURCE_EXTENDED

#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rp_game_map.h"
#include "rp_gui_nc.h" /*datatypes included here*/
#include "rp_game.h"
#include "rp_game_logic.h"

#include "rp_statline_msg.h"



#include "rp_gui_nc_data.h"

#define PANELSIZE 20

/*At what line does panel update start writing*/
#define PANELSTARTLINE 8


void rp_update_panel(tile*);
void rp_update_panel_army(army *army_source, tile *source_tile);
void rp_update_panel_city(city *city_source, tile *source_tile);
void rp_update_panel_clear(tile *source_tile);
void rp_update_statusline();
void rp_term_resize();


WINDOW *create_map_window(void);
WINDOW *create_panel_window(void);
WINDOW *create_statusline_window(void);

wchar_t res_sym(unsigned int);

WINDOW *map;
WINDOW *panel;


WINDOW *statusline;
extern stat_msg *newest_msg;
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
    //Testing with colors with random values
    init_color(C_FACTION_1+i,rand()%999,rand()%999,rand()%999);
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
  
  if (!UM_STATLINE_MAXIMIZED()) {
    int newheight = viewheight * 0.8;
    //statusmode = 1;
    UM_MAX_STATLINE();
    wresize(map, viewheight-newheight,mapwidth);//resize mapview
    mvwin(statusline,viewheight-newheight,0);
    wresize(statusline,newheight,mapwidth);
  } else if (UM_STATLINE_MAXIMIZED()) {
    //statusmode = 0;
    UM_MIN_STATLINE();
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
  char data[33];
  sprintf(data,"xom:%d,my:%d  ",xonmap,mody);
  mvwaddstr(panel,3,3,data);

  if (UM_OWN_ARMY_IS_SELECTED() && (inmc != &s_umc) ) {
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
    for (int j = 0; j < viewwidth; j++) {
      int bakedj = j + mc.x;
      //If our window is large enough, we will need to wrap the map
      while (bakedj >= WORLD_WIDTH) {
	bakedj -= WORLD_WIDTH;
      } //mc.x cannot be negative, so no need to do same for that


      wchar_t symbol = '!'; //symbol will remain '!' if armycity cue true, but none found
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

      //debugging
      if (symbol == L'!') {
	char symdata[60];
	tile tt = world_p->worldmap[bakedi][bakedj];
	sprintf(symdata,"armycitycue error:Tile(%d,%d)=%u,own:%d",bakedj,bakedi,tt,
		rp_get_owner(&tt));
	rp_new_sl_msg(0,symdata);
      }
      //debugging out
      
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

  wrefresh(map);
  wrefresh(panel);
  wrefresh(statusline);
}

  

void rp_update_statusline(void)
{

  int top = getbegy(statusline);    //beginning coordinate
  int bottom = getmaxy(statusline); //height
  stat_msg *msg = newest_msg;

  /* DEBUG mark statline window's y-coords at top of panel window */
  char data[20];
  sprintf(data,"t:%d,b:%d",top,bottom);
  mvwaddstr(panel,0,0,data);

  for (int i = bottom-1;
       i >= 0 && msg != NULL;
       i--, msg = msg->older) {
    mvwaddstr(statusline,i,0,msg->message);
    wclrtoeol(statusline); /*Clear window to end of line*/
  }
  
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
    void *found;
    //army *found_army;
    //city *found_city;
    
    // if ( (found_army = rp_army_search(fact,umc.x,umc.y)) ) {
    if ( (found = rp_army_search(fact,umc.x,umc.y)) ) {
      
      rp_update_panel_army(found, source_tile);

      //} else if ( (found_city = rp_city_search(fact,umc.x,umc.y)) ) {
    } else if ( (found = rp_city_search(fact,umc.x,umc.y)) ) {

      rp_update_panel_city(found, source_tile);
      /*
      sprintf(data, "%s", found_city->name);
      mvwaddstr(panel,9,1,data);
      */
    } else {
      //if armycity cue found but no actual army or city found for faction
      // = error
    }

  } else {
    rp_update_panel_clear(source_tile);
  }
  
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

  if (UM_STATLINE_MAXIMIZED()) {
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

  tile *sel_tile = &(world_p->worldmap[umc.y][umc.x]);
  if (rp_get_armycity(sel_tile)) {

    /* Find army */
    army *found_army;
    found_army = rp_army_search(
		   rp_faction_search(
		     rp_get_owner( &(world_p->worldmap[umc.y][umc.x]))),
		   umc.x, umc.y);

    if (found_army != NULL) {
      UM_S_ARMY();
      //Army selection stuff here

      if (found_army->owner->controller == HUMAN) {
	UM_S_OWN_ARMY();
	rp_army_selected_input(found_army);
      } else {
	//display data on panel about foreign army
	rp_update_panel_army(found_army,sel_tile);
      }
      return;
    }

    /* Find city if no army was found */
    city *found_city;
    found_city = rp_city_search(
		   rp_faction_search(
		     rp_get_owner(&(world_p->worldmap[umc.y][umc.x]))),
		   umc.x, umc.y);
    
    if (found_city != NULL) {
      UM_S_CITY();
      rp_update_panel_city(found_city,sel_tile);
      //City selection stuff here
      return;
    }
    
  } else { //Nothing to select here
    //Maybe an error message here
  }
}

void rp_update_panel_city(city *city, tile *source_tile)
{
  char data[20];
  int line = PANELSTARTLINE;

  rp_panel_add(line++,2,"CITY");
  rp_panel_add(line++,1,city->owner->name);

  sprintf(data, "%d , %d",city->x,city->y);
  rp_panel_add(line++,3,data);

  rp_tile_description(source_tile , data);
  rp_panel_add(line++,1,data);
  rp_panel_add(line++,1,city->name);

  sprintf(data, "Poulation: %u",city->population);
  rp_panel_add(line++,1,data);

}

/* Show army data on panel */
void rp_update_panel_army(army *army, tile *source_tile)
{

  char data[20];
  int line = PANELSTARTLINE;

  rp_panel_add(line++,2,"ARMY");
  rp_panel_add(line++,1,army->owner->name);
  
  sprintf(data, "%d , %d",army->x,army->y);
  rp_panel_add(line++,3,data);

  rp_tile_description(source_tile, data);
  rp_panel_add(line++,1,data);

  sprintf(data, "Army: %s",army->owner->army_templates[army->army_template_id].name);
  rp_panel_add(line++,1,data);

  sprintf(data, "Move left: %d",army->movement_left);
  rp_panel_add(line++,2,data);

  for (int i=0; i<MAX_TROOPTYPE_AMOUNT; i++) {

    army_template *at = &(army->owner->army_templates[army->army_template_id]);
    /* 
       <trooptype name in army's armytype>
       <actual mancount of trooptype in army>
       <default mancount of troop in armytype>
    */
    sprintf(data,"%s %d/%d", at->troop[i].name,army->troop[i],at->default_troop_count[i]);
    rp_panel_add(line++,1,data);
  }
  
}

void rp_update_panel_clear(tile *source_tile)
{
  char data[20];
  int line = PANELSTARTLINE;

  rp_panel_add(line++,1,"Countryside");
  
  strcpy(data, (world_p->faction_list[rp_get_owner(source_tile)].name));
  rp_panel_add(line++,1,data);

  sprintf(data, "%d , %d",umc.x,umc.y);
  rp_panel_add(line++,3,data);

  rp_tile_description(source_tile, data);
  rp_panel_add(line++,3,data);
}

void rp_panel_add(int line, int xcoord, char *data)
{
  wmove(panel,line,0);
  wclrtoeol(panel);
  mvwaddstr(panel,line,xcoord,data);
}

void rp_army_selected_input(army *selected_army)
{

  while (1) { //single tile step movement

    rp_update_panel_army(selected_army,
      &(world_p->worldmap[selected_army->y][selected_army->x]));
    
    wrefresh(panel);
    
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

  UM_S_ARMY();
  
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
      UM_DS_ARMY();
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
  UM_DS_ARMY();
  UM_DS_CITY();
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
