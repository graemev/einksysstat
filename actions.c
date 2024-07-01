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
