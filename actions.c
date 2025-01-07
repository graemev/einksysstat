/*
 * Actions allow you to use gadgets
 */

/*
 * WHO	WHEN	WHAT
 * GPV	29jun24	Inital version
 */

/*
 * do_action- carry out the action described in the "action structure
 * struct action * action - The action we want to execute
 *
 *
 *
 *
 * 
 */

#include "actions.h"
#include "dump_utils.h"



void do_action(struct action * action)
{

  switch (action->verb)
    {
    case(do_none):
      // should never happen
      break;
	 
    case(do_label):
      // should never happen
      break;
	 
    case(do_loop):
      // should never happen
      break;
	 
    case(do_init_image):
      ga_init_image(action->args.init_image.colour, action->args.init_image.rotate);
      break;
	 
    case(do_init_module):
      ga_init_module();
      break;
      
    case(do_init_display):  // never released
      ga_init_display();
      break;
      
    case(do_release_module):
      ga_release_module();
      break;

    case(do_release_image):
      ga_release_image();
      break;
	 
    case(do_render):
      ga_render();
      break;
	 
    case(do_clear):
      ga_clear();
      break;
	 
    case(do_text):
      ga_text(action->args.text.xstart, action->args.text.ystart, action->args.text.fsize, action->args.text.text, action->args.text.colour);
      break;
	 
    case(do_hostname):
      ga_hostname(action->args.hostname.xstart, action->args.hostname.ystart, action->args.hostname.fsize);
      break;
	 
    case(do_timestamp):
      ga_timestamp(action->args.timestamp.xstart, action->args.timestamp.ystart, action->args.timestamp.fsize);
      break;
	 
    case(do_uptime):
      ga_uptime(action->args.uptime.xstart, action->args.uptime.ystart, action->args.uptime.fsize);
      break;
	 
    case(do_meter):
      ga_meter(action->args.meter.xstart, action->args.meter.ystart, action->args.meter.fsize, action->args.meter.value, action->args.meter.colour);
      break;
	 
    case(do_df):
      ga_df(action->args.df.ystart, action->args.df.fsize, action->args.df.device, action->args.df.label, action->args.df.units, action->args.df.cutoff);
      break;
	 
    case(do_age):
      ga_age(action->args.age.xstart, action->args.age.ystart, action->args.age.fsize, action->args.age.filename, action->args.age.label, action->args.age.units, action->args.age.cutoff);
      break;
	 
    case(do_file):
      ga_file(action->args.file.xstart, action->args.file.ystart, action->args.file.fsize, action->args.file.filename, action->args.file.lines);
      break;

    case(do_sleep):
      ga_sleep(action->args.sleep.seconds);
      
    }
}

/*
 * char * str_action(struct action * action) -- return a string of the action (verb)
 */
char * str_action(struct action * action)
{
  char *p;

  switch (action->verb)
    {
    case(do_none):
      p="none";
      break;
	 
    case(do_label):
      p="label";
      break;
	 
    case(do_loop):
      p="loop";
      break;
	 
    case(do_init_image):
      p="init_image";
      break;

    case(do_init_module):
      p="init_module";
      break;
      
    case(do_init_display):
      p="init_display";
      break;


      
    case(do_release_module):
      p="init_module";
      break;

    case(do_release_image):
      p="release";
      break;
      
    case(do_render):
      p="render";
      break;
	 
    case(do_clear):
      p="clear";
      break;
	 
    case(do_text):
      p="text";
      break;
	 
    case(do_hostname):
      p="hostname";
      break;
	 
    case(do_timestamp):
      p="timestamp";
      break;
	 
    case(do_uptime):
      p="uptime";
      break;
	 
    case(do_meter):
      p="meter";
      break;
	 
    case(do_df):
      p="df";
      break;
	 
    case(do_age):
      p="age";
      break;
	 
    case(do_file):
      p="file";
      break;
      
    case(do_sleep):
      p="sleep";
      break;

    default:
      p="????";
      break;
    }
  return p;
}


/* ====== INIT ====== */

struct action * new_action_init(enum Eink_colour colour,
				UWORD            rotate
				)
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action init:");
      exit(1);
    }

  p->verb = do_init_image;
  
  p->args.init_image.colour=colour;
  p->args.init_image.rotate=rotate;

  return p;
  
}

/* ====== TEXT ====== */


struct action * new_action_text(UWORD            xstart,
				UWORD            ystart,
				int              fsize,
				char             text[],   // Note, last agg (not same as method)  [ GPV flawed thinking]
				enum Eink_colour colour
				)
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action text:");
      exit(1);
    }

  p->verb = do_text;
  
  p->args.text.xstart=xstart;
  p->args.text.ystart=ystart;
  p->args.text.fsize =fsize;
  p->args.text.colour=colour;
  strncpy(p->args.text.text, text, MAX_DISPLAY);

  return p;
  
}

/* ====== HOSTNAME ====== */


struct action * new_action_hostname(UWORD            xstart,
				    UWORD            ystart,
				    int              fsize
				    )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action hostname:");
      exit(1);
    }

  p->verb = do_hostname;
  
  p->args.hostname.xstart=xstart;
  p->args.hostname.ystart=ystart;
  p->args.hostname.fsize =fsize;

  return p;
  
}

/* ====== TIMESTAMP ====== */


struct action * new_action_timestamp(UWORD           xstart,
				    UWORD            ystart,
				    int              fsize
				    )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action timestamp:");
      exit(1);
    }

  p->verb = do_timestamp;
  
  p->args.timestamp.xstart=xstart;
  p->args.timestamp.ystart=ystart;
  p->args.timestamp.fsize =fsize;

  return p;
  
}

/* ====== UPTIME ====== */


struct action * new_action_uptime(UWORD            xstart,
				  UWORD            ystart,
				  int              fsize
				  )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action uptime:");
      exit(1);
    }

  p->verb = do_uptime;
  
  p->args.uptime.xstart=xstart;
  p->args.uptime.ystart=ystart;
  p->args.uptime.fsize =fsize;

  return p;
  
}

/* ====== METER ====== */


struct action * new_action_meter(UWORD            xstart,
				 UWORD            ystart,
				 int              fsize,
				 int              value,
				 enum Eink_colour colour
				 )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action meter:");
      exit(1);
    }

  p->verb = do_meter;
  
  p->args.meter.xstart=xstart;
  p->args.meter.ystart=ystart;
  p->args.meter.fsize =fsize;
  p->args.meter.value =value;
  p->args.meter.colour=colour;

  return p;
  
}


/* ====== DF ====== */


struct action * new_action_df		(UWORD            ystart,
					 int	          fsize,
					 char	          device[MAX_PATHNAME],
					 char	          label[MAX_DISPLAY],
					 enum df_units    units,
					 int	          cutoff)



{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action df:");
      exit(1);
    }

  p->verb = do_df;
  
  p->args.df.ystart=ystart;
  p->args.df.fsize =fsize;

  p->args.df.units =units;
  p->args.df.cutoff=cutoff;
  strncpy(p->args.df.label, label, MAX_DISPLAY);
  strncpy(p->args.df.device,device, MAX_PATHNAME);

  return p;
  
}

/* ====== AGE ====== */

struct action * new_action_age		(UWORD		   xstart,
					 UWORD		   ystart,
					 int	           fsize,
					 char		   filename[MAX_PATHNAME],
					 char		   label[MAX_DISPLAY],
					 enum age_units    units,
					 int               cutoff
					 )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action age:");
      exit(1);
    }

  p->verb = do_age;
  
  p->args.age.xstart=xstart;
  p->args.age.ystart=ystart;
  p->args.age.fsize =fsize;

  p->args.age.units =units;
  p->args.age.cutoff=cutoff;
  strncpy(p->args.age.label, label, MAX_DISPLAY);
  strncpy(p->args.age.filename, filename, MAX_PATHNAME);

  return p;
  
}

/* ====== FILE ====== */


struct action * new_action_file		(UWORD		   xstart,
					 UWORD		   ystart,
					 int		   fsize,
					 char		   filename[MAX_PATHNAME],
					 int		   lines
					 )
{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action file:");
      exit(1);
    }

  p->verb = do_file;
  
  p->args.file.xstart=xstart;
  p->args.file.ystart=ystart;
  p->args.file.fsize =fsize;

  p->args.file.lines =lines;
  strncpy(p->args.file.filename, filename, MAX_PATHNAME);

  return p;
  
}


/* ====== SLEEP ====== */


struct action * new_action_sleep(int		   seconds)

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action sleep:");
      exit(1);
    }

  p->verb = do_sleep;
  
  p->args.sleep.seconds=seconds;

  return p;
  
}

/* ====== RENDER ====== */


struct action * new_action_render()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action render:");
      exit(1);
    }

  p->verb = do_render;
  

  return p;
  
}

/* ====== CLEAR ====== */


struct action * new_action_clear()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action clear:");
      exit(1);
    }

  p->verb = do_clear;
  

  return p;
  
}

/* ====== LOOP ====== */


struct action * new_action_loop()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action loop:");
      exit(1);
    }

  p->verb = do_loop;
  

  return p;
  
}




/* ====== NONE ====== */


struct action * new_action_none()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action none:");
      exit(1);
    }

  p->verb = do_none;

  return p;
  
}






void delete_action(struct action * action)
{
  free(action);
}



char * str_struct_action(struct action * action)
{
  static char str[256];  // Messy, but debuging only
  
  switch (action->verb)
    {
    case(do_none):
      snprintf(str,256, "Action(NONE)");
      break;
	 
    case(do_label):
      snprintf(str,256, "Action(label)");
      break;
	 
    case(do_loop):
      snprintf(str,256, "Action(loop)");
      break;
	 
    case(do_init_image):
      snprintf(str,256, "Action(init_image): colour=%s, rotate=%d", str_colour(action->args.init_image.colour), action->args.init_image.rotate);
      break;
	 
    case(do_init_module):
      snprintf(str,256, "Action(init_module)");
      break;
      
    case(do_init_display):  // never released
      snprintf(str,256, "Action(init_display)");
      break;
      
    case(do_release_module):
      snprintf(str,256, "Action(release_module)");
      break;

    case(do_release_image):
      snprintf(str,256, "Action(release_image)");
      break;
	 
    case(do_render):
      snprintf(str,256, "Action(render)");
      break;
	 
    case(do_clear):
      snprintf(str,256, "Action(clear)");
      break;
	 
    case(do_text):
      snprintf(str,256, "Action(text): xstart=%d, ystart=%d, fsize=%d, text=%s, colour=%s", 
	       action->args.text.xstart, action->args.text.ystart, action->args.text.fsize, action->args.text.text, str_colour(action->args.text.colour));

      break;
	 
    case(do_hostname):
      snprintf(str,256, "Action(hostname): xstart=%d, ystart=%d, fsize=%d", 
	       action->args.hostname.xstart, action->args.hostname.ystart, action->args.hostname.fsize);
      break;
	 
    case(do_timestamp):
      snprintf(str,256, "Action(timestamp): xstart=%d, ystart=%d, fsize=%d", 
	       action->args.timestamp.xstart, action->args.timestamp.ystart, action->args.timestamp.fsize);
      break;
	 
    case(do_uptime):
      snprintf(str,256, "Action(uptime): xstart=%d, ystart=%d, fsize=%d", 
	       action->args.uptime.xstart, action->args.uptime.ystart, action->args.uptime.fsize);
      break;
	 
    case(do_meter):
      snprintf(str,256, "Action(meter): xstart=%d, ystart=%d, fsize=%d, value=%d, colour=%s", 
	       action->args.meter.xstart, action->args.meter.ystart, action->args.meter.fsize, action->args.meter.value, str_colour(action->args.meter.colour));
      break;
	 
    case(do_df):
      snprintf(str,256, "Action(df): ystart=%d, fsize=%d, device=%s , label=%s, units=%s, cutoff=%d", 
	       action->args.df.ystart, action->args.df.fsize, action->args.df.device, action->args.df.label, str_df_units(action->args.df.units), action->args.df.cutoff);
      break;
	 
    case(do_age):
      snprintf(str,256, "Action(age): xstart=%d, ystart=%d, fsize=%d, filename=%s, label=%s, units=%s, cutoff=%d", 
	       action->args.age.xstart, action->args.age.ystart, action->args.age.fsize, action->args.age.filename, action->args.age.label, str_age(action->args.age.units), action->args.age.cutoff);
      break;
	 
    case(do_file):
      snprintf(str,256, "Action(file): xstart=%d, ystart=%d, fsize=%d, filename=%s, lines=%d", 
	       action->args.file.xstart, action->args.file.ystart, action->args.file.fsize, action->args.file.filename, action->args.file.lines);
      break;

    case(do_sleep):
      snprintf(str,256, "Action(sleep): seconds=%d", 
	       action->args.sleep.seconds);
      
    }

  return str; 
}
