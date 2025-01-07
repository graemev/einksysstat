#ifndef _ACTIONS_H_
#define _ACTIONS_H_

/*
 * Actions allow you to use gadgets
 */

/*
 * WHO	WHEN	WHAT
 * GPV	29jun24	Inital version
 */


#include "gadgets.h"
#include "DEV_Config.h"


enum verb
  {
    do_noverb = 0,
    do_none,
    do_label,
    do_loop,
    do_init_module,
    do_init_display,
    do_init_image,
    do_release_image,
    do_release_module,
    do_render,
    do_clear,
    do_text,
    do_hostname,
    do_timestamp,
    do_uptime,
    do_meter,
    do_df,
    do_age,
    do_file,
    do_sleep,
  };


/*
 * args_X are used to hold agrs fro ga_X methods:
 *
 * 1: The args done't need to be in the same order, so large unaligned fiels (char[]) ar put at the end (compile may have done this in anycase, but it may be cautious)
 * 2: The display is 200 pixels wide, the smalles font (fron8) is 5 pixels wide , so 40 chars at most can fit across the display
 * 3: Pathlengths can be quite large (4096 in kernel) . We choose an arbitrary limit much smaller
 *
 */

#define MAX_DISPLAY  41
#define MAX_PATHNAME 65


struct args_init
{
  enum Eink_colour colour;
  UWORD            rotate;
};

struct args_text
{
  UWORD            xstart;
  UWORD            ystart;
  int              fsize;
  enum Eink_colour colour;
  char             text[MAX_DISPLAY];   // Note, last agg (not same as method)
};

struct args_hostname
{
  UWORD            xstart;
  UWORD		   ystart;
  int		   fsize;
};
struct args_timestamp
{
  UWORD            xstart;
  UWORD		   ystart;
  int		   fsize;  
};
struct args_uptime
{
  UWORD            xstart;
  UWORD            ystart;
  int              fsize;
};
struct args_meter
{
  UWORD            xstart;
  UWORD            ystart;
  int              fsize;
  int              value;
  enum Eink_colour colour;
};
struct args_df
{
  UWORD            ystart;
  int		   fsize;
  enum df_units    units;
  int		   cutoff;
  char		   label[MAX_DISPLAY];
  char		   device[MAX_PATHNAME];

};
struct args_age
{
  UWORD		   xstart;
  UWORD		   ystart;
  int		   fsize;
  enum age_units   units;
  int              cutoff;
  char		   label[MAX_DISPLAY];
  char		   filename[MAX_PATHNAME];

};
struct args_file
{
  UWORD		   xstart;
  UWORD		   ystart;
  int		   fsize;
  int		   lines;
  char		   filename[MAX_PATHNAME];
};

struct args_sleep
{
  int		   seconds;
};



struct action
{
  enum   verb     verb;
  struct action * next;
  struct action * prev;

  union
  {
    struct args_init       init_image;
    struct args_text       text;
    struct args_hostname   hostname;
    struct args_timestamp  timestamp;
    struct args_uptime     uptime;
    struct args_meter      meter;
    struct args_df         df;
    struct args_age        age;
    struct args_file       file;
    struct args_sleep      sleep;
  } args;
  
};

static struct action no_action = {do_noverb, NULL, NULL}; // used only in error handling etc, must not be enchain()ed


// Fight the horrible C99 syntax.

// example ==>    {do_hostname , .args.hostname.xstart=0,                .args.hostname.ystart=0, .args.hostname.fsize=12},

#define A(ACTION,ASSIGN)	.args.ACTION.ASSIGN

#define A5(ACTION,ASSIGN1,ASSIGN2,ASSIGN3,ASSIGN4,ASSIGN5) do_##ACTION, A(ACTION,ASSIGN1), A(ACTION,ASSIGN2), A(ACTION,ASSIGN3), A(ACTION,ASSIGN4), A(ACTION,ASSIGN5)
#define A4(ACTION,ASSIGN1,ASSIGN2,ASSIGN3,ASSIGN4        ) do_##ACTION, A(ACTION,ASSIGN1), A(ACTION,ASSIGN2), A(ACTION,ASSIGN3), A(ACTION,ASSIGN4)
#define A3(ACTION,ASSIGN1,ASSIGN2,ASSIGN3                ) do_##ACTION, A(ACTION,ASSIGN1), A(ACTION,ASSIGN2), A(ACTION,ASSIGN3)
#define A2(ACTION,ASSIGN1,ASSIGN2                        ) do_##ACTION, A(ACTION,ASSIGN1), A(ACTION,ASSIGN2)
#define A1(ACTION,ASSIGN1                                ) do_##ACTION, A(ACTION,ASSIGN1)


// Note order of args matches ga_xx() ie the actual call (not how it's stored ..since it's done by name anyhow)


#define DEF_SET_COLOUR(COLOUR)\
  {do_set_colour, .args.set_colour.colour=COLOUR }

//#define DEF_INIT_DISPLAY()			\
//  {do_init_display }

#define DEF_INIT_IMAGE(COLOUR, ROTATE)\
  {do_init_image, .args.init_image.colour=COLOUR, .args.init_image.rotate=ROTATE}

#define DEF_IMAGE_RELEASE()\
  {do_image_release, }

//#define DEF_DISPLAY_RELEASE()			\
//  {do_display_release, }


#define DEF_RENDER()\
  {do_render, }

#define DEF_CLEAR()\
  {do_clear, }

#define DEF_TEXT(XSTART, YSTART, FSIZE, TEXT, COLOUR)\
  {do_text, .args.text.xstart=XSTART, .args.text.ystart=YSTART, .args.text.fsize=FSIZE, .args.text.text=TEXT, .args.text.colour=COLOUR }

#define DEF_HOSTNAME(XSTART, YSTART, FSIZE)\
  {do_hostname, .args.hostname.xstart=XSTART, .args.hostname.ystart=YSTART, .args.hostname.fsize=FSIZE}

#define DEF_TIMESTAMP(XSTART, YSTART, FSIZE)\
  {do_timestamp, .args.timestamp.xstart=XSTART, .args.timestamp.ystart=YSTART, .args.timestamp.fsize=FSIZE }


#define DEF_UPTIME(XSTART, YSTART, FSIZE)\
  {do_uptime, .args.uptime.xstart=XSTART, .args.uptime.ystart=YSTART,.args.uptime.fsize=FSIZE }

#define DEF_METER(XSTART, YSTART, FSIZE, VALUE, COLOUR)\
  {do_meter, .args.meter.xstart=XSTART , .args.meter.ystart=YSTART , .args.meter.fsize=FSIZE , .args.meter.value=VALUE , .args.meter.colour=COLOUR }

#define DEF_DF(YSTART, FSIZE, DEVICE, LABEL, UNITS, CUTOFF)\
  {do_df , .args.df.ystart=YSTART , .args.df.fsize=FSIZE , .args.df.device=DEVICE , .args.df.label=LABEL , .args.df.units=UNITS , .args.df.cutoff=CUTOFF }


#define DEF_AGE(XSTART, YSTART, FSIZE, FILENAME, LABEL, UNITS, CUTOFF)\
  {do_age , .args.age.xstart=XSTART, .args.age.ystart=YSTART, .args.age.fsize=FSIZE, .args.age.filename=FILENAME, .args.age.label=LABEL, .args.age.units=UNITS, .args.age.cutoff=CUTOFF }

#define DEF_FILE(XSTART, YSTART, FSIZE, FILENAME, LINES)\
  {do_file , .args.file.xstart=XSTART, .args.file.ystart=YSTART, .args.file.fsize=FSIZE, .args.file.filename=FILENAME, .args.file.lines=LINES }

#define DEF_SLEEP(SECONDS)\
  {do_sleep , .args.sleep.seconds=SECONDS }







void   do_action (struct action * action);
char * str_action(struct action * action);



struct action * new_action_init		(enum Eink_colour colour,
					 UWORD            rotate
					 );

struct action * new_action_text		(UWORD            xstart,
					 UWORD            ystart,
					 int              fsize,
					 char             text[],
					 enum Eink_colour colour
					 );

struct action * new_action_hostname	(UWORD            xstart,
					 UWORD            ystart,
					 int              fsize
					 );

struct action * new_action_timestamp	(UWORD           xstart,
					 UWORD            ystart,
					 int              fsize
					 );



struct action * new_action_uptime	(UWORD            xstart,
					 UWORD            ystart,
					 int              fsize
					 );


struct action * new_action_meter	(UWORD            xstart,
					 UWORD            ystart,
					 int              fsize,
					 int              value,
					 enum Eink_colour colour
					 );

struct action * new_action_df		(UWORD            ystart,
					 int	          fsize,
					 char	          device[MAX_PATHNAME],
					 char	          label[MAX_DISPLAY],
					 enum df_units    units,
					 int	          cutoff
					 );

struct action * new_action_age		(UWORD		   xstart,
					 UWORD		   ystart,
					 int	           fsize,
					 char		   filename[MAX_PATHNAME],
					 char		   label[MAX_DISPLAY],
					 enum age_units    units,
					 int               cutoff
					 );



struct action * new_action_file		(UWORD		   xstart,
					 UWORD		   ystart,
					 int		   fsize,
					 char		   filename[MAX_PATHNAME],
					 int		   lines
					 );


struct action * new_action_render	();
struct action * new_action_clear	();
struct action * new_action_loop		();
struct action * new_action_none		();




struct action * new_action_sleep	(int		   seconds);



void delete_action			(struct action * action);

char * str_struct_action			(struct action * action);

#endif
