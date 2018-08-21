# Red Planet Conquest

![Alt text](/other/SS.jpg "Screenshot")

#

Conquer Mars, or possibly some other location

Requires NCurses.
On Linux compiles with:
gcc -O2 -Wall -pedantic controller.c rp_game.c rp_game_map.c rp_statline_msg.c main.c rp_game_logic.c rp_gui_nc.c -lncursesw

#

Controls:
- Arrow keys to move view around.
- Shift + arrows move view faster
- Number keys move cursor
- Enter on ground centers view on cursor.
- Enter selects army when your own army (a darker violet 'A') is under cursor.
- Number keys move selected army around until it runs out of moves
- '5' goes into unimplemented multitilemovement mode for selected army. Currently you can only move a secondary cursor around.
- '0' escapes for army movement modes
- '0' exits game if in normal view mode
