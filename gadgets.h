/*****************************************************************************
* | File	:	gadgets.h
* | Author	:	Graeme Vetterlein
* | Function	:	e-Paper Display system status
* | Info	:
*----------------
* |	This version:	V0.1
* | Date	:   2024-0620
* | Info	:   
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
#ifndef _GADGETS_H_
#define _GADGETS_H_

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// TBD #include "DEV_Config.h"

#include "module.h"

/* Colours on the eInk display works a tad oddly,
   Both WHITE and RED are defined to be "ON" (0xFF)
   While BLACK is defined to be "off (0x00) 

   There are 2 images (red_image and black_image) ...to get BLACK , BOTH must be OFF
						     to get RED , only the red_image (pixel) must be on
						     to get WHITE , only the black_image (pixel) must be on

   To add the confusion, while RED is just "the colour Red", "BLACK" means the backround colour of the display
   which is actually a light grey (so almost white) so "BLACK" means a "light grey" while "WHITE" means diaplsy as black
   I hope that's clear.

   In Eink_colour, I'll use the terms differently:

   
   
    */



enum Eink_colour
  {
    is_no_colour,
    is_black_on_grey,	// Black Letters on a grey backgound (ie Normal)
    is_red_on_grey,	// Red Letters on a grey backgound (ie Alert)
    is_black_on_red,	// INVERTED Color ... a red background
    is_red_on_black,	// INVERTED Color ... a black background
  };

/* how to show remaining free space */
enum df_units
  {
    df_none,
    df_meg,
    df_geg,
    df_best,
    df_pcent,
  };

enum age_units
  {
    age_none,
    age_minutes,
    age_hours,
    age_days,
  };

enum temp_type
  {
    temp_none,
    temp_cpu,
    temp_gpu,
    temp_sdram_c, /* these last 3 can't be measured(yet) so can't be used. */
    temp_sdram_i,
    temp_sdram_p,
  };



enum zzzzz
  {
    yyyyy,
  };

/* Quote, "any method with more than 6 args, means you've missed one" :-) ... sadly we need each of these */

extern void		ga_define(int    display,
				  char * part_name,
				  char * local_name,
				  int    no_columns,
				  int    no_rows,

				  int    no_rst,
				  int    no_dc,
				  int    no_cs,
				  int    no_busy,
				  int    no_pwr,
				  int    no_mosi,
				  int    no_sclk);
				  

extern enum Eink_colour	ga_set_colour(int display, enum Eink_colour colour);

extern void		ga_init_display  (int display);
extern void		ga_init_module   (int display);
extern void		ga_init_image	 (int display, enum Eink_colour colour, UWORD rotate);
extern void		ga_release_image (int display);
extern void		ga_release_module(int display);

extern void		ga_render    (int display);
extern void		ga_clear     (int display);

extern int		ga_text      (int display, UWORD xstart, UWORD ystart, int fsize, char text[], enum Eink_colour colour);
extern int		ga_hostname  (int display, UWORD Xstart, UWORD Ystart, int fsize);
extern int		ga_timestamp (int display, UWORD xstart, UWORD ystart, int fsize);
extern int		ga_uptime    (int display, UWORD xstart, UWORD ystart, int fsize);
extern int		ga_meter     (int display, UWORD xstart, UWORD ystart, int fsize, int value, enum Eink_colour colour);
extern int		ga_df	     (int display,               UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff);
extern int		ga_age	     (int display, UWORD xstart, UWORD ystart, int fsize, char *filename, char * label, enum age_units units, int cutoff);
extern int		ga_file      (int display, UWORD xstart, UWORD ystart, int fsize, char *filename, int lines);

extern int		ga_sleep     (int display, unsigned int seconds);

extern void		ga_identify();

extern int		ga_linux_temp(int display, UWORD xstart, UWORD ystart, int fsize, char            *pathname, int limit);
extern int		ga_vcore_temp(int display, UWORD xstart, UWORD ystart, int fsize, enum temp_type   type,     int limit);
extern int		ga_throttle  (int display, UWORD xstart, UWORD ystart, int fsize);

extern int		ga_fan       (int display, UWORD xstart, UWORD ystart, int fsize, char *pathname, int limit);

extern void		ga_xxxxx();
#endif


