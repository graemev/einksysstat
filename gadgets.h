/*****************************************************************************
* | File      	:	gadgets.h
* | Author      :	Graeme Vetterlein
* | Function    :	e-Paper Display system status
* | Info        :
*----------------
* |	This version:   V0.1
* | Date        :   2024-0620
* | Info        :   
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
    is_black_on_grey,   // Black Letters on a grey backgound (ie Normal)
    is_red_on_grey,     // Red Letters on a grey backgound (ie Alert)
    is_black_on_red,    // INVERTED Color ... a red background
    is_red_on_black,    // INVERTED Color ... a black background
  };

/* how to show remaining free space */
enum df_units
  {
    df_meg,
    df_geg,
    df_best,
    df_pcent,
  };

enum age_units
  {
    age_minutes,
    age_hours,
    age_days,
  };

enum Eink_colour ga_set_colour(enum Eink_colour colour);
enum Eink_colour ga_get_colour();
void		 ga_display_init();
void             ga_image_init      (enum Eink_colour colour, UWORD rotate);
void		 ga_render    ();
void		 ga_image_release   ();
void		 ga_display_release   ();


int		 ga_text      (UWORD xstart, UWORD ystart, int fsize, char text[], enum Eink_colour colour);
int		 ga_hostname  (UWORD Xstart, UWORD Ystart, int fsize);
int		 ga_timestamp (UWORD xstart, UWORD ystart, int fsize);
int		 ga_uptime    (UWORD xstart, UWORD ystart, int fsize);
int		 ga_meter     (UWORD xstart, UWORD ystart, int fsize, int value, enum Eink_colour colour);
int              ga_df        (UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff);
int		 ga_age       (UWORD xstart, UWORD ystart, int fsize, char *filename, char * label, enum age_units units, int cutoff);


#endif


