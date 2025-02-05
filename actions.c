/* -*- Mode: c; tab-width: 4;c-basic-offset: 4; c-default-style: "gnu" -*- */
/*
 * Actions allow you to use gadgets
 */

/*
 * WHO	WHEN	WHAT
 * GPV	29jun24	Inital version
 */


/* This file is part of Einkstat. */

/*     Einkstat is free software: you can redistribute it and/or modify it under
       the terms of the GNU General Public License as published by the Free
       Software Foundation, either version 3 of the License, or (at your option)
       any later version. */

/*     Einkstat is distributed in the hope that it will be useful, but WITHOUT ANY
       WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
       FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
       details. */

/*     You should have received a copy of the GNU General Public License along
       with Einkstat. If not, see <https://www.gnu.org/licenses/>. */

/*     Note included within Einkstat is code covered by a Public Domain (PD)
       licence (which is indicated within those files) these sections
       are subject to that PD licence. If you wish to use those portions
       under "Public Domain" terms it is proably easier to use the original
       found at e.g. https://github.com/waveshareteam/e-Paper */


/*
 * do_action- carry out the action described in the "action structure
 * struct action * action - The action we want to execute
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
      ga_init_image(action->display, action->args.init_image.colour, action->args.init_image.rotate);
      break;
	 
    case(do_init_module):
      ga_init_module(action->display);
      break;
      
    case(do_init_display):  // never released
      ga_init_display(action->display);
      break;
      
    case(do_release_module):
      ga_release_module(action->display);
      break;

    case(do_release_image):
      ga_release_image(action->display);
      break;
	 
    case(do_render):
      ga_render(action->display);
      break;
	 
    case(do_clear):
      ga_clear(action->display);
      break;
	 
    case(do_text):
      ga_text(action->display, action->args.text.xstart, action->args.text.ystart, action->args.text.fsize, action->args.text.text, action->args.text.colour);
      break;
	 
    case(do_hostname):
      ga_hostname(action->display,action->args.hostname.xstart, action->args.hostname.ystart, action->args.hostname.fsize);
      break;
	 
    case(do_timestamp):
      ga_timestamp(action->display,action->args.timestamp.xstart, action->args.timestamp.ystart, action->args.timestamp.fsize);
      break;
	 
    case(do_uptime):
      ga_uptime(action->display,action->args.uptime.xstart, action->args.uptime.ystart, action->args.uptime.fsize);
      break;
	 
    case(do_meter):
      ga_meter(action->display,action->args.meter.xstart, action->args.meter.ystart, action->args.meter.fsize, action->args.meter.value, action->args.meter.colour);
      break;
	 
    case(do_df):
      ga_df(action->display,action->args.df.ystart, action->args.df.fsize, action->args.df.device, action->args.df.label, action->args.df.units, action->args.df.cutoff);
      break;
	 
    case(do_age):
      ga_age(action->display,action->args.age.xstart, action->args.age.ystart, action->args.age.fsize, action->args.age.filename, action->args.age.label, action->args.age.units, action->args.age.cutoff);
      break;
	 
    case(do_file):
      ga_file(action->display,action->args.file.xstart, action->args.file.ystart, action->args.file.fsize, action->args.file.filename, action->args.file.lines);
      break;

    case(do_sleep):
      ga_sleep(action->display,action->args.sleep.seconds);
      break;
      
    case(do_identify):
      ga_identify();
      break;
      
    case(do_linux_temp):
      ga_linux_temp();
      break;

    case(do_vcore_temp):
      ga_vcore_temp();
      break;

    case(do_throttle):
      ga_throttle();
      break;

    case(do_fan):
      ga_fan();
      break;

    case(do_xxxxx):
      ga_xxxxx();
      break;
      
    default:
      fprintf(stderr, "Internal error action(%s) is undefined\n", str_action(action));
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
      
    case(do_identify):
      p="identify";
      break;

    case(do_linux_temp):
      p="temp(linux)";
      break;

    case(do_vcore_temp):
      p="temp(vcore)";
      break;

    case(do_throttle):
      p="throttle";
      break;

    case(do_fan):
      p="fan";
      break;

    case(do_xxxxx):
      p="xxxxx";
      break;

    default:
      p="????";
      break;
    }
  return p;
}


/* ====== INIT ====== */

struct action * new_action_init(int              display,
				enum Eink_colour colour,
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

  p->display = display;
  
  p->args.init_image.colour=colour;
  p->args.init_image.rotate=rotate;

  return p;
  
}

/* ====== TEXT ====== */

struct action * new_action_text(int              display,
				UWORD            xstart,
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
  strncpy(p->args.text.text, text, MAX_TEXT_DISPLAY);

  return p;
}

/* ====== HOSTNAME ====== */

struct action * new_action_hostname(int              display,
				    UWORD            xstart,
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

struct action * new_action_timestamp(int              display,
				     UWORD           xstart,
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

struct action * new_action_uptime(int              display,
				  UWORD            xstart,
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

struct action * new_action_meter(int              display,
				 UWORD            xstart,
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

struct action * new_action_df		(int              display,
					 UWORD            ystart,
					 int	          fsize,
					 char	          device[MAX_PATHNAME],
					 char	          label[],
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
  strncpy(p->args.df.label, label, MAX_TEXT_DISPLAY);
  strncpy(p->args.df.device,device, MAX_PATHNAME);

  return p;
  
}

/* ====== AGE ====== */

struct action * new_action_age		(int              display,
					 UWORD		  xstart,
					 UWORD		  ystart,
					 int	          fsize,
					 char		  filename[MAX_PATHNAME],
					 char		  label[MAX_TEXT_DISPLAY],
					 enum age_units   units,
					 int              cutoff
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
  strncpy(p->args.age.label, label, MAX_TEXT_DISPLAY);
  strncpy(p->args.age.filename, filename, MAX_PATHNAME);

  return p;
}

/* ====== FILE ====== */

struct action * new_action_file		(int              display,
					 UWORD		  xstart,
					 UWORD		  ystart,
					 int		  fsize,
					 char		  filename[MAX_PATHNAME],
					 int		  lines
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

struct action * new_action_sleep(int              display,
				 int		  seconds)

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

struct action * new_action_render(int              display)

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

struct action * new_action_clear(int              display)

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


struct action * new_action_loop(int              display)

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

/* ====== IDENTIFY ====== */


struct action * new_action_identify()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action identify:");
      exit(1);
    }

  p->verb = do_identify;
  

  return p;
}

/* ====== TEMP with a pathname (so comes form normal Linux sys filesystem ====== */


struct action * new_action_linux_temp(int              display,
									  UWORD		       xstart,
									  UWORD		       ystart,
									  int		       fsize,
									  char			   pathname[MAX_PATHNAME],
									  int			   limit
									  )

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action temp(linux):");
      exit(1);
    }

  p->verb = do_linux_temp;
  
  p->args.ltemp.xstart=xstart;
  p->args.ltemp.ystart=ystart;
  p->args.ltemp.fsize =fsize;

  strncpy(p->args.ltemp.pathname, pathname, MAX_PATHNAME);
  p->args.ltemp.limit =limit;

  

  return p;
}
/* ====== TEMP with keyword (some coome form videocore mailbox) ====== */

struct action * new_action_vcore_temp(int              display,
									  UWORD		       xstart,
									  UWORD		       ystart,
									  int		       fsize,
									  enum temp_type   type,
									  int			   limit
									  )

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action temp(vcore):");
      exit(1);
    }

  p->verb = do_vcore_temp;
  
  p->args.vtemp.xstart=xstart;
  p->args.vtemp.ystart=ystart;
  p->args.vtemp.fsize =fsize;

  p->args.vtemp.type  =type;
  p->args.vtemp.limit =limit;

  return p;
}


/* ====== THROTTLE ====== */


struct action * new_action_throttle(int            display,
									UWORD		   xstart,
									UWORD		   ystart,
									int		       fsize
									)

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action throttle:");
      exit(1);
    }

  p->verb = do_throttle;
  
  p->args.throttle.xstart=xstart;
  p->args.throttle.ystart=ystart;
  p->args.throttle.fsize =fsize;

  return p;
}


/* ====== FAN ====== */


struct action * new_action_fan(int              display,
							   UWORD		    xstart,
							   UWORD		    ystart,
							   int		        fsize,
							   char			    pathname[],
							   int			    limit
							   )

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action fan:");
      exit(1);
    }

  p->verb = do_fan;
  
  p->args.fan.xstart=xstart;
  p->args.fan.ystart=ystart;
  p->args.fan.fsize =fsize;

  strncpy(p->args.fan.pathname, pathname, MAX_PATHNAME);
  p->args.fan.limit =limit;

  return p;
}


/* ====== XXXXX ====== */


struct action * new_action_xxxxx()

{
  struct action * p;

  if ((p = malloc(sizeof(struct action))) == NULL)
    {
      perror("Malloc action xxxxx:");
      exit(1);
    }

  p->verb = do_xxxxx;
  

  return p;
}




/* ====== NONE ====== */


struct action * new_action_none(int              display)

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






void delete_action(struct action  * action)
{
  free(action);
}



char * str_struct_action(struct action  * action)
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
	  break;
	  
	case(do_identify):
      snprintf(str,256, "Action(identify):");
	  break;

	case(do_linux_temp):
		snprintf(str,256, "Action(temp[linux]: xstart=%d, ystart=%d, fsize=%d, pathname=%s, limit=%d", 
				 action->args.ltemp.xstart, action->args.ltemp.ystart, action->args.ltemp.fsize, action->args.ltemp.pathname, action->args.ltemp.limit);

		break;
		
	case(do_vcore_temp):
		snprintf(str,256, "Action(temp[vcore]: xstart=%d, ystart=%d, fsize=%d, measurement=%s, limit=%d", 
				 action->args.vtemp.xstart, action->args.vtemp.ystart, action->args.vtemp.fsize, str_temp(action->args.vtemp.type), action->args.vtemp.limit);
		break;


	case(do_throttle):
		snprintf(str,256, "Action(throttle): xstart=%d, ystart=%d, fsize=%d", 
				 action->args.throttle.xstart, action->args.throttle.ystart, action->args.throttle.fsize);

	  break;

	case(do_fan):
		snprintf(str,256, "Action(fan): xstart=%d, ystart=%d, fsize=%d, pathname=%s, limit=%d", 
				 action->args.fan.xstart, action->args.fan.ystart, action->args.fan.fsize, action->args.fan.pathname, action->args.fan.limit);

	  break;


	case(do_xxxxx):
      snprintf(str,256, "Action(xxxxx):");
	  break;

	default:
		snprintf(str,256, "Action(%d):", action->verb);
	  break;


    }

  return str; 
}
