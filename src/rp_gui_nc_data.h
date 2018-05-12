

static unsigned int ui_mode = 0;
#define armymask 0x1
#define citymask 0x2
#define statlinemask 0x4

#define UM_S_ARMY ui_mode |= armymask
#define UM_DS_ARMY ui_mode = ~(~ui_mode | armymask)
#define UM_S_CITY ui_mode |= citymask
#define UM_DS_CITY ui_mode = ~(~ui_mode | citymask)
#define UM_STATLINE_MAXIMIZED (ui_mode & statlinemask)
#define UM_MAX_STATLINE ui_mode |= statlinemask
#define UM_MIN_STATLINE ui_mode = ~(~ui_mode | statlinemask)

