/*****************************************************************************
* | File      	:   einksysstat.c
* | Author      :   Graeme Vetterlein
* | Function    :   1.54inch B e-paper PoC display NAS stats on e-Ink panel
* | Info        :
*----------------
* | This version:   V0.1
* | Date        :   2025-01-16
* | Info        :


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


/*-------------------------------------------------------------------------
The overall common use of this code would be for an eInk display attached to a
Raspberry Pi. The display would show current stats about the "Pi" a quick glance
at the display would show e.g. how much disk space is free. If the date & time
are displayed (the default) then, for example, if the system had crashed you
could get some idea of how long it had been down.



Intended to be run via e.g. crontab, say once an hour. Update the eInk display
to show "current status" the actual contents of the display are defined via a
config file provided via an argument to this command which defines a sequence of
"gadgets".

There are however other ways to use this. In the git repo of this code are
various other example commands, the most instructive might be:

 - Strawman  - direct calls to ga_* methods to update display
 - Woodenman - static macros define actions, then "obey" these as runtime actions

So 3 different ways to execise the "gadgets":

  1: code the calls in regular C code
  2: Define a series of CPP Macros (similar in format to the config file)
  3: Define a series of verbs (each invokes a single gadgets)

  -------------------------------------------------------------------------*/  
  
#include "eink_sysstat.h"
#include "parser.h"
#include "chain.h"





static void print_version(char *name) 
  {
    fprintf(stderr,"%s: Version: %s\n", name, VERSION);
  }

static void usage(char *name) 
  {
  fprintf(stderr,
	  "Usage %s [-d][-v][-h]<filename> \n"
	  "-d = Debug (may be repeated)\n"
	  "-v = Verbose (may be repeated)\n"
	  "-V = VERSION (just prints version and exits)\n"
	  "-h = help\n"
	  "     Parse the given file an obey it's actions\n"
	  "\n"
	  "     The actions are a series of verbs defined in einksysstat.config(5)\n"
	  "     this command will typically be run via crontab(5)\n"
	  , name);
  }


/* parse_action_file() +ve return value = number of rules, -ve = number of errors */

/*
 * Bison (in the way we are using it here) is very "oldskol" , so not at all reentrant.
 * There are lots of "globals" passed back and forth. Here for example we set yyin
 * to the file pointer we want to parse. Since yyparse() takes fixed args and we need
 * to get back the "chain" we have created, we need to pass that in and out via "globals" too.
 *
 * We will try to isolate such "dirty tricks" to this one method, we don't want anybody else
 * playing with these globals.
 */


extern      FILE          *yyin;

static	    int		   debug=0;
static	    int		   verbose=0;



static int parse_action_file(FILE *fp, struct action **actions)
{
  int rc    = 0;
  int nerrs = 0;
  
  // Possibly enable parser runtime debugging.
  yydebug = !!getenv ("YYDEBUG");
  yyin    = fp;

  // See: configure.ac (Note >1 ...1 is default for normal debug)
#if DEBUG > 1
  yydebug = 1;
#endif

  yyparse (&nerrs);
  
  // Exit on failure if there were errors.

  if (nerrs)
    rc = -nerrs;
  else
    {
    rc       = action_count;
    *actions = action_chain;
    }
  return rc;
}







int main(int argc, char*argv[]) 
  {
  char * filename;

  int    mode;
  int    c;


  struct action *actions;
  int            n_actions;
  FILE		*fp;

  struct action *p;


  while ((c=getopt(argc, argv,"dvhV")) != -1)
    {
      switch(c)
	{
	case 'd' :
	  ++debug;
	  if (debug>1)  // e.g. used -d twice
	    yydebug = 1;
	  break;

	case 'v' :
	  ++verbose;
	  break;

	case 'V' :
	  print_version(argv[0]);
	  exit(0);;
	  break;

	case 'h':
	case '?':
	default:
	  usage(argv[0]);
	  exit(1);
	  break;
	}
    }

  
  if ((argc-optind) != 1)
    {
    usage(argv[0]);
    exit(1);
    }

  filename=argv[optind];


  if ((fp=fopen(filename, "r")) == NULL)
    {
    perror("fopen:");
    fprintf(stderr, "Filename=%s\n", filename);
    exit(2);
    }
  
  if ((n_actions=parse_action_file(fp, &actions)) < 0)
    {
      fprintf (stderr, "Exiting with %d errors\n", -n_actions);
      exit (2);
    }
  else
    {
      Debug("Init display\n");
      ga_init_module();   // Not available as an ACTION (only done once)
      ga_init_display();  // Not available as an ACTION (only done once)

      Debug("Init image arrays (malloc(3)\n");
      ga_init_image(is_black_on_grey,  90);   // NB malloc(3)s memory

      Debug("Starting actions\n");
  
      for (p=actions; p; p=p->next)
  
	{
	  Debug("Doing action: %s\n", str_action(p));
	  do_action(p);
	  Debug("Done action: %s\n", str_action(p));
	}

      Debug("Finished actions\n");


    }

  /* three stages to exit & cleanup:
   *
   * 1: Free up the memory we used to store images
   * 2: put the eInk device into a deepsleep
   * 3: Power off the module connected to GPIO pins (and free up the pins)
   *
   * Right now 3 is not possible (but is required) because the sample code was just comments
   * and the device is not documented.
   */

  
  Debug("Release image arrays (free(3)\n");
  ga_release_image();		      // NB free(3)s memory

  Debug("Release module\n");
  ga_release_module();
  
  return 0;
}

