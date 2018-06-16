#include <stdlib.h>
#include <string.h>
#include "rp_datatypes.h"

#include "rp_statline_msg.h"

stat_msg *newest_msg = NULL;

void rp_cull_msgbuf(void);

/* Add new statusline message */
void rp_new_sl_msg(unsigned int par_flag, char *text)
{
  stat_msg *new_msg = malloc(sizeof(stat_msg));
  new_msg->flag = par_flag;
  new_msg->message = malloc(sizeof(char) * strlen(text));
  strcpy(new_msg->message,text);
  
  new_msg->older = newest_msg;
  newest_msg = new_msg;
  
  rp_cull_msgbuf();
  
}

void rp_free_sl_msg(stat_msg **old_msg)
{
  free((*old_msg)->message);
  free((*old_msg));
  *old_msg = NULL;
}


void rp_cull_msgbuf(void)
{
  int i = 0;
  stat_msg **cur_msg = &newest_msg;
  while(*cur_msg != NULL) {
    if (i == MSGBUF_SIZE) {
      rp_free_sl_msg(cur_msg);
    } else {
      cur_msg = &((*cur_msg)->older);
      i++;
    }
  }
}
