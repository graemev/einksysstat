/*****************************************************************************
* | File      	:   ironman.c
* | Author      :   Graeme Vetterlein
* | Function    :   1.54inch B e-paper PoC display NAS stats on e-Ink panel
* | Info        :
*----------------
* |	This version:   V0.1
* | Date        :   2025-01-05
* | Info        :


Working up to fully functioning code in stages:

 - Paperman  - Just initalise the display
 - Strawman  - direct calles to ga_* methods to update display
 - Woodenman - static macros define actions, then "obey" these as runtime actions
 - Ironman   - Parse an external files to define actions, , then "obey" these as runtime actions


#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

#include "eink_sysstat.h"
#include "parser.h"
#include "chain.h"

static void usage(char *name) 
  {
  fprintf(stderr,
	  "Usage %s [-d][-v][-h]<filename> \n"
	  "-d = Debug\n"
	  "-v = Verbose\n"
	  "-h = help\n"
	  "     Parse the given file an obey it's actions\n"
	  "\n"
	  "     Maybe we should save parsed contents\n"
	  , name);
  }


/* +ve return value = number of rules, -ve = number of errors */

/*
 * Bison (in the way we are using it here) is very "oldskol" , so not at all reentrant.
 * There are lots of "globals" passed back and forth. Here for example we set yyin
 * to the file pointer we want to parse. Since yyparse() takes fixed args and wee need
 * to get back the "chain" we have created, wee need to pass that in and out via "globals" too.
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

  // See: configure.ac
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


  while ((c=getopt(argc, argv,"drvh23")) != -1)
    {
      switch(c)
	{
	case 'd' :
	  ++debug;
	  break;

	case 'v' :
	  ++verbose;
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
      ga_init_module();  // Not available as an ACTION (only done once)
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

