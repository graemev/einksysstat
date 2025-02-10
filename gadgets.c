/* -*- Mode: c; tab-width: 4;c-basic-offset: 2; c-default-style: "gnu" -*- */
/*****************************************************************************
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
* | File      	:   gadgets.c
* | Author      :   Graeme Vetterlein
* | Function    :   1.54inch B e-paper PoC display NAS stats on e-Ink panel (collection of gadgets)
* | Info        :
*----------------
* |	This version:   V0.1
* | Date        :   2024-06-20
* | Info        :
*/

#include "eink_sysstat.h"
#include "device.h"  
#include <GUI_Paint.h>

#include "time.h"
#include <libproc2/misc.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>

#include "gadgets.h"
#include "vcore.h"



// ---------------- internal ----------------

/* I suspect there may be, indirect, assumptions that this will be 1024 */
#define STRING_SIZE 1024


static enum Eink_colour  gadget_colour = is_black_on_grey;
static UBYTE            *red_image;
static UBYTE            *black_image;

static struct gc  // Gadget colours
{
  UWORD black_fg;
  UWORD black_bg;
  UWORD red_fg;
  UWORD red_bg;
} gc;

static void init_module()   // Twiddles with GPIO Pins to make display active
{
  Debug("Initalise Module\n");
  if(module_turn_on()!=0)
    {
      fprintf(stderr, "Failed to initalise display\n");
      exit(1);
    }
}

static void init_display()   // Twiddles with GPIO Pins to make display active
{
  Debug("Initalise 1IN54B_V2\n");
  eink_init();
}

static void sleep_display()   // Twiddles with GPIO Pins to make display inactive (deep sleep)
{
  Debug("Sleep 1IN54B_V2\n");
  eink_deepsleep();
}

/*
 * init_images(UBYTE **black, UBYTE **red) -- NB malloc(3)s space for red & black arrays (freed in ga_image_release())
 * UBYTE **black
 * UBYTE **red
 *
 */

static void init_images(UBYTE **black, UBYTE **red)
{
  struct timespec start={0,0}, finish={0,0}; 

  clock_gettime(CLOCK_REALTIME,&start);

  /* create the RED and BLACK images */
  
  UWORD image_size = ((EINK_WIDTH % 8 == 0)? (EINK_WIDTH / 8 ): (EINK_WIDTH / 8 + 1)) * EINK_HEIGHT;

  if((*black = (UBYTE *)malloc(image_size)) == NULL)
    {
      fprintf(stderr, "Failed to malloc(3) black memory...\r\n");
      exit(2);
    }

  if((*red = (UBYTE *)malloc(image_size)) == NULL)
    {
      fprintf(stderr, "Failed to malloc(3) red memory...\r\n");
      exit(2);
    }

  /* Fill in default values, height, width, rotation etc */
  
  Debug("NewImage:black_image and red_image\r\n");
  Paint_NewImage(*black, EINK_WIDTH, EINK_HEIGHT, 90, WHITE);
  Paint_NewImage(*red  , EINK_WIDTH, EINK_HEIGHT, 90, WHITE);

  clock_gettime(CLOCK_REALTIME,&finish);

  Debug("%ld Seconds to setup blank screen\n",finish.tv_sec-start.tv_sec);
}


/* Quote, "any method with more than 6 args, means you've missed one" :-) ... sadly we need each of these */

/*
 * This is ultimately targeted as a "verb" in the script file. It's not now, becasue I don't know 
 * HOWTO use multiple displays (different pins) or different displays (may need different logic)
 */

extern void ga_define(int    display,
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
		      int    no_sclk)
{
  struct display_settings * s;

  s = get_display(display);

  strncpy(s->part_name,  part_name,  MAX_DISPLAY_NAME);
  strncpy(s->local_name, local_name, MAX_DISPLAY_NAME);
    
  s->no_columns     = no_columns;
  s->no_rows        = no_rows;

  s->eink_rst_pin   = no_rst;
  s->eink_dc_pin    = no_dc;
  s->eink_cs_pin    = no_cs;
  s->eink_busy_pin  = no_busy;
  s->eink_pwr_pin   = no_pwr;
  s->eink_mosi_pin  = no_mosi;
  s->eink_sclk_pin  = no_sclk;
  
  return;
}

enum Eink_colour ga_set_private_colour(int display, enum Eink_colour colour, struct gc *lc)
{
  switch (colour)
    {
    default:
	case(is_no_colour):
    case(is_black_on_grey):   // Black Letters on a grey backgound (ie Normal)
      lc->black_bg = 0xFF;
      lc->black_fg = 0x00;
      lc->red_bg   = 0xFF;
      lc->red_fg   = 0xFF;
      break;
      
    case(is_red_on_grey):     // Red Letters on a grey backgound (ie Alert)
      lc->black_bg = 0xFF;
      lc->black_fg = 0xFF;
      lc->red_bg   = 0xFF;
      lc->red_fg   = 0x00;
      break;
      
    case(is_black_on_red):    // INVERTED Color ... a red background
      lc->black_bg = 0xFF;
      lc->black_fg = 0x00;
      lc->red_bg   = 0x00;
      lc->red_fg   = 0xff;
      break;
      
    case(is_red_on_black):    // INVERTED Color ... a black background
      lc->black_bg = 0x00;
      lc->black_fg = 0xFF;
      lc->red_bg   = 0xFF;
      lc->red_fg   = 0x00;
      break;
      
    }
  return colour;
}

// -------------- exported -------------------

enum Eink_colour ga_set_colour(int display, enum Eink_colour colour)
{
  gadget_colour = colour;

  ga_set_private_colour(display, colour, &gc);

  return gadget_colour;
}


/*
 * ga_init_module(int display) - Logically initally setup module (eg enable the SPI pins)
 */

void ga_init_module(int display)
{
  Debug("ga_init_module start\n");

  init_module(); // Twiddle with GPIO pins to setup the module
  
  Debug("ga_init_module end\n");
  
  return;
}


/*
 * ga_init_display() - Logically initally setup display [1IN54B_V2] (eg enable the SPI pins) Only ever done once
 */

void ga_init_display(int display)
{
  Debug("ga_init_display start\n");

  init_display(); // Twiddle with GPIO pins to setup the display
  
  Debug("ga_init_display end\n");
  
  return;
}

/*
 * ga_image_init(enum Eink_colour colour, UWORD rotate)
 *   enum Eink_colour colour	- default colour used on internal data structures (e.g is_black_on_grey)
 *   UWORD rotate		- e.g. 90 only 0, 90, 180 or 270 degrees allowed
 */


void ga_init_image(int display, enum Eink_colour colour, UWORD rotate)
{
  struct timespec start={0,0}, finish={0,0}; 
  
  clock_gettime(CLOCK_REALTIME,&start);

  Debug("ga_image_init start\n");
  init_images(&black_image, &red_image);
  ga_set_colour(display, colour);
  Paint_SetRotate(rotate);

  Debug("ga_Drawing------------------------\n");
  Paint_SelectImage(black_image);
  Paint_Clear(0X00);
  Paint_SelectImage(red_image);
  Paint_Clear(0x00);
  Debug("ga_image_init done 0X00\n");
  Paint_SelectImage(black_image);
  Paint_Clear(0XFF);
  Paint_SelectImage(red_image);
  Paint_Clear(0xFF);
  Debug("ga_init_image done 0XFF\n");

  clock_gettime(CLOCK_REALTIME,&finish);

  Debug("%ld Seconds to do ga_init_image\n",finish.tv_sec-start.tv_sec);
  
  return;
}

/*
 * ga_image_release() - NB it free(3)s memory allocated in init_images(UBYTE **black, UBYTE **red)
 */

void ga_release_image(int display)
{
  free(black_image);
  free(red_image);
  black_image = NULL;
  red_image   = NULL;

  return;
}


void ga_release_module(int display)
{
  struct timespec start={0,0}, finish={0,0};
  clock_gettime(CLOCK_REALTIME,&start);
  gpio_delay(2000);//important, at least 2s
  // close 5V
  Debug("close 5V, Module enters 0 power consumption ...\r\n");
  module_turn_off();
  clock_gettime(CLOCK_REALTIME,&finish);
  
  Debug("%ld Seconds to ga_display_release\n",finish.tv_sec-start.tv_sec);

  eink_close();
  
  return;
}


void ga_render(int display)  // Take the images abd display on the eInk
{
  eink_display(black_image, red_image);
}

void ga_clear(int display)  // Just clear the screen (not full setup) then clear out teh red & black images
{
  eink_clear();

  Debug("Writing 0X00\n");
  Paint_SelectImage(black_image);
  Paint_Clear(0X00);
  Paint_SelectImage(red_image);
  Paint_Clear(0x00);

  Debug("Writing 0XFF\n");
  Paint_SelectImage(black_image);
  Paint_Clear(0XFF);
  Paint_SelectImage(red_image);
  Paint_Clear(0xFF);
}


sFONT *get_font(int fsize)
{
  sFONT *font;
  
  switch (fsize)
    {
    case(8):
      font=&Font8;
      break;
    case(12):
      font=&Font12;
      break;
    case(16):
      font=&Font16;
      break;
    case(20):
      font=&Font20;
      break;
    case(24):
    default:
      font=&Font24;
      break;
    }
  return font;
}

static int font_width(int fsize)  // Not the obvious value you might guess
{
  sFONT *font;
  int   width;

  font  = get_font(fsize);
  width = font->Width;

  return width;
}

static int font_height(int fsize)  // Not the obvious value you might guess
{
  sFONT *font;
  int   height;

  font   = get_font(fsize);
  height = font->Height;

  return height;
}

/* ======================================== The Gadets proper ==================================*/

/*
 * ga_hostname(int display, UWORD Xstart, UWORD Ystart, sFONT* Font) -- Display Hostname at specified point , in specific font
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 *
 * Fontsize determines how many characters fit on a line. For example Font8 can have 40 char on the 200 pixels display
 * while fontsize 24 can only fit 11 on that same display.
 *
 * We could simply hardcode the maximums for each from for the 1.54in(200 pixel) display but, instead as an exercise
 * in HOWTO get the information in a display specific fashion we calulated it from the values defines for THIS display
 * (right now that will always be 0, the the side will always be 200 columns but this code could be enhanced.
 *
 *  Fontname  char width	maximum character on 1.54in (200 pixel) display
 *
 *  Font8	5		40
 *  Font12	7		28
 *  Font16	11		18
 *  Font20	14		14
 *  Font24	17		11
 *
 */

static  int fwidth(int fsize)
{
  int width;
  switch(fsize)
    {
    case 8:
      width=5;
      break;
    case 12:
      width=7;
      break;
    case 16:
      width=11;
      break;
    case 20:
      width=14;
      break;
    default:
    case 24:
      width=17;
      break;
    }
  return width;
}


int ga_text(int display, UWORD xstart, UWORD ystart, int fsize, char text[], enum Eink_colour colour)
{
  int				rc=0;
  sFONT			      * font;
  struct gc			lc;
  struct display_settings     * p;
  int				max_chars;

#define HARD_LIMIT 256
  char				buffer[HARD_LIMIT];

  
  
  font = get_font(fsize);


  p = get_display(display);
  
  max_chars = p->no_columns/fwidth(fsize);

  if (max_chars >= HARD_LIMIT)
    max_chars = HARD_LIMIT-1;


  strncpy(buffer, text, max_chars);	// usually copies the '\0' from test
  buffer[max_chars]='\0';
  
  
  (void)ga_set_private_colour(display, colour, &lc);  // Private colours (eg based on value)
  
  Debug("Drawing------------------------\n");

  Paint_SelectImage(black_image);
  Paint_DrawString_EN(xstart, ystart, buffer, font, lc.black_fg, lc.black_bg);

  Paint_SelectImage(red_image);
  Paint_DrawString_EN(xstart, ystart, buffer, font, lc.red_fg, lc.red_bg);
  
  return rc;
}


/*
 * ga_hostname(UWORD Xstart, UWORD Ystart, sFONT* Font) -- Display Hostname at specified point , in specific font
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 */
int ga_hostname(int display, UWORD xstart, UWORD ystart, int fsize)
{
  int rc=0;

  sFONT *font;
  char   hostname[64];
  
  font = get_font(fsize);
  
  if (gethostname(hostname, sizeof(hostname)) == -1)
    {
      perror("gethostname:");
      exit(1);
    }
  hostname[sizeof(hostname)-1]='\0'; // Cope with undefined behaviour

  Debug("Drawing------------------------\n");

  Paint_SelectImage(black_image);
  Paint_DrawString_EN(xstart, ystart, hostname, font, gc.black_fg, gc.black_bg);

  Paint_SelectImage(red_image);
  Paint_DrawString_EN(xstart, ystart, hostname, font, gc.red_fg, gc.red_bg);
  
  return rc;
}

/*
 * ga_timestamp(UWORD Xstart, UWORD Ystart, sFONT* Font) -- Display timestamp at specified point , in specific font
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 */
int ga_timestamp(int display, UWORD xstart, UWORD ystart, int fsize)
{
  int        rc=0;

  sFONT     *font;
  char       stamp[64];

  time_t     now;
  struct tm  now_parts;

  Debug("putting timestamp @ %u, %u\n", xstart, ystart);
  
  font = get_font(fsize);

  now = time(NULL);
  (void)localtime_r(&now, &now_parts);
  (void)strftime(stamp, 64,"%Y/%m/%d-%H:%M", &now_parts); // 16 characters

  Debug("Drawing------------------------\n");

  Paint_SelectImage(black_image);
  Paint_DrawString_EN(xstart, ystart, stamp, font, gc.black_fg, gc.black_bg);

  Paint_SelectImage(red_image);
  Paint_DrawString_EN(xstart, ystart, stamp, font, gc.red_fg, gc.red_bg);
  
  return rc;
}

/*
 * ga_uptime:  display how long the system has been up.
 *
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 */


int ga_uptime(int display, UWORD xstart, UWORD ystart, int fsize)
{
  sFONT     *font;
  time_t     now;
  struct tm  now_parts;

  double  uptime_secs;
  char    buffer[64];
  time_t  uptime;
  // int	  secs;
  int	  mins;
  int	  hrs;
  int	  days;
  int	  x;

  int     rc=0;
  
  Debug("putting uptime @ %u, %u\n", xstart, ystart);

  if ((rc=procps_uptime (&uptime_secs, NULL)) <0)  // Man procps_misc(3)
    {
      rc = 0-rc;    // Swap sign (curious API)
      strcpy(buffer, "unavailable");
    }
  else
    {
      uptime = uptime_secs; // Beats me why they would want to use a floating point?

      // secs = uptime % 60;
      x    = uptime / 60;  // Minutes

      mins = x % 60;
      x    = x / 60;       // Hours

      hrs= x % 24;       
      days=  x / 24;       

      sprintf(buffer, "Uptime%3dd %02dh %02dm", days, hrs, mins);
    }
  
  font = get_font(fsize);

  now = time(NULL);
  (void)localtime_r(&now, &now_parts);

  Debug("Drawing------------------------\n");

  Paint_SelectImage(black_image);
  Paint_DrawString_EN(xstart, ystart, buffer, font, gc.black_fg, gc.black_bg);

  Paint_SelectImage(red_image);
  Paint_DrawString_EN(xstart, ystart, buffer, font, gc.red_fg, gc.red_bg);
  
  return rc;
}

/*
 * ga_meter: Generic gadget that shows a VU style meter in a given colour
 *           used by ga_df
 *
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 * int   value, - A number 0-100 e.g a percentage
 * enum  Eink_colour colour - The meter does not use the normal gadget_colour (so it can be highlighted)
 *
 */

int ga_meter(int display, UWORD xstart, UWORD ystart, int fsize, int value, enum Eink_colour colour)
{
  int rc=0;
  int width=50;
  int fwidth;
  int height=fsize;
  struct gc lc;
  
  /* if value =0 -> 100, and widdth is 50 , tehn every "2" pints in value represents a fill with of 1 */
  fwidth = value/(100/width);

  (void)ga_set_private_colour(display, colour, &lc);  // Private colours (eg based on value)
  
  Paint_SelectImage(black_image);

  Paint_DrawRectangle(xstart, ystart, xstart+width,  ystart+height,     lc.black_bg, 1, DRAW_FILL_FULL);
  Paint_DrawRectangle(xstart, ystart, xstart+fwidth, ystart+height,     lc.black_fg, 1, DRAW_FILL_FULL);
  Paint_DrawRectangle(xstart, ystart, xstart+width, ystart+height,      lc.black_fg, 1, DRAW_FILL_EMPTY);

  Paint_SelectImage(red_image);
  Paint_DrawRectangle(xstart, ystart, xstart+width,  ystart+height,     lc.red_bg, 1, DRAW_FILL_FULL);
  Paint_DrawRectangle(xstart, ystart, xstart+fwidth, ystart+height,    lc.red_fg, 1, DRAW_FILL_FULL);
  Paint_DrawRectangle(xstart, ystart, xstart+width, ystart+height,     lc.red_fg, 1, DRAW_FILL_EMPTY);

  return rc;
}

/*
 * ga_df(int display, UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff)
 *
 * UWORD ystart - Y co-ord  (note no X , it always starts in column 0)
 * int   fsize  - Font size (8,12,16,20,24 Only)
 * enum df_units units -  The text part can be diplayed in Gigabyte, Megabyte , or %apage (best chooses automatically)
 * int cutoff - If less than this %age free space exists (e.g. 20) then the meter is shown in RED
 */

/* Like df(1) -- Says how much space is left in a given filesystem */

/* $ df -h */
/* Filesystem                Size  Used Avail Use% Mounted on */
/* udev                      657M     0  657M   0% /dev */
/* tmpfs                     185M  1.5M  184M   1% /run */
/* /dev/disk/by-label/root5   28G  7.7G   19G  30% / */
/* tmpfs                     924M  220K  924M   1% /dev/shm */
/* tmpfs                     5.0M   16K  5.0M   1% /run/lock */
/* /dev/mmcblk0p1            510M   76M  435M  15% /boot/firmware */
/* tmpfs                     185M   36K  185M   1% /run/user/1000 */
/* tmpfs                     185M   40K  185M   1% /run/user/1501 */
/* /dev/mmcblk0p3            515M   24K  477M   1% /media/graeme/rest1 */
/* /dev/mmcblk0p2             29G  7.3G   20G  28% /media/graeme/root1 */
/* so */
/* ga_df(0, 50, 0, int 24, "/", "root",df_geg)                          --> root 19G [###       ] */
/* ga_df(0, 50, 0, int 24, "/dev/disk/by-label/root5", "root",df_pcent) --> root 70% [###       ] */

/* alwayas sarts in col 0 and always fonr24 */

int ga_df(int display, UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff)
{
  int   rc=0;
  int	pcent_free;
  char  value[32];   // In whatever user specified
  int	bpm;	        // blocks per megabyte

  enum  Eink_colour colour;
  struct statvfs  s;

  _Bool stat_ok = true;

  if ( statvfs (device, &s) == -1)
	{
	  //      perror(device);
	  stat_ok = false;
	  rc = errno;
	  sprintf(value, "%s", label);
	}
  else
	{
	  pcent_free = (100 * s.f_bfree) / s.f_blocks;   // Assumes fsblkcnt_t has enough headroom for 100 times value , pcent is a small number
	  bpm = 1024*1024/s.f_bsize;				 // eg 2048 for 512 byte blocks
	  
	  if (units == df_best)
		{
		  if (s.f_bfree/bpm > 1024*1024)  // More than 1TB , display as a %age
			units = df_pcent;
		  else if (s.f_bfree/bpm > 4096)  // More than 4096MB , display in GB
			units = df_geg;
		  else
			units = df_meg;
		}

      switch(units)
		{
		case (df_meg):
		  sprintf(value, "%-4.4s%uM", label, (unsigned int)(s.f_bfree/bpm));
		  break;
		  
		case (df_geg):
		  sprintf(value, "%-4.4s%uG", label, (unsigned int)(s.f_bfree/bpm/1024));
		  break;
		  
		default:
		case (df_none):
		case (df_pcent):
		  sprintf(value, "%-4.4s%d%%", label, pcent_free);
		  break;
		}
    }     

  if (stat_ok == false || pcent_free < cutoff)
    colour = is_red_on_grey;
  else
    colour = is_black_on_grey;
  
  ga_text (0  , 0, ystart, fsize, value, colour);

  if (stat_ok)
    ga_meter(0, 150, ystart, fsize, 100-pcent_free, colour);
  else
    ga_text(0, 150, ystart, 16, "UNAV", colour);

  return rc;
}

/*
 * ga_age(UWORD xstart, UWORD ystart, int fsize, char *filename, char * label, int cutoff) Show thw age of a file (in red if too old)
 * UWORD xstart,
 * UWORD ystart,
 * int   fsize,
 * char *filename,
 * char *label,
 * enum  age_units
 * int   cutoff)
 */

int ga_age(int display, UWORD xstart, UWORD ystart, int fsize, char *filename, char * label, enum age_units units, int cutoff)
{
  int          rc=0;
  int          nvalue;         // In whatever units specified (numberic)
  struct stat  statbuf;
  time_t       now;
  time_t       age;
  char	       buffer[32];
  int	       max_len=0;  // depends on font size and ystart
  char	       c;
  enum Eink_colour colour;
  
  now = time(NULL);

  if (stat(filename, &statbuf) == -1)
    {
      perror(filename);
      rc=errno;
      age=0;			// AKA cannot acces file (e.g. not there)
    }
  else
    {
      age=now-statbuf.st_mtime;  // Mixes long and time_t (hope that's OK :-) 
    }

  switch(units)
    {
    default:
    case(age_none):
      nvalue = age;
      c=' ';
      break;
    case(age_minutes):
      nvalue = age/60;
      c='M';
      break;
    case(age_hours):
      nvalue = age/(60*60);
      c='H';
      break;
    case(age_days):
      nvalue = age/(60*60*24);
      c='D';
      break;
    }
  
  snprintf(buffer, sizeof(buffer), "%s %d%c", label, nvalue, c);

  if (fsize <8)
    fsize=8;
  
  max_len = (200-xstart) / font_width(fsize);   // The amount we can actuall fit on screen
  
  buffer[max_len] = '\0';

  if (nvalue > cutoff)
    colour = is_red_on_grey;
  else
    colour = is_black_on_grey;
  

  ga_text(display, xstart, ystart, fsize, buffer, colour);

  return rc;
}

/* int ga_file(int display, UWORD xstart, UWORD ystart, int fsize, char *filename, int lines)
 *
 * Display some lines from a file.
 *
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 * char *filename - file where text comes from
 * int   lines  - number of liines to show 0 = maximum possible
 */

int ga_file(int display, UWORD xstart, UWORD ystart, int fsize, char *filename, int lines)
{
  int    xspace;
  int    yspace;
  int    max_cols;
  int    max_lines;
  FILE  *f;
  size_t buffer_size = 256;
  char  *buffer;
  size_t n;
  int	 rc=0;
  int	 i;
  int	 offset;
  int	 char_width;
  int	 char_height;
  char	*p;
  
  
  xspace = 200-xstart;
  yspace = 200-ystart;

  char_width  = font_width(fsize);
  char_height = font_height(fsize);
  
  max_cols = xspace/char_width;
  max_lines = yspace/char_height;

  if (lines !=0 && lines < max_lines)  // caller may limit
    max_lines = lines;

  buffer=malloc(buffer_size);

  if ((f=fopen(filename, "r")) == NULL)
    {
      strncpy(buffer, strerror(errno), MIN(buffer_size, max_cols));
      ga_text (display, xstart, ystart, fsize, buffer, gadget_colour);
	  perror(filename);
	  n  = 0;
	  rc = errno;

    }
  else
    {
      offset=0;
      for (i=0; i<max_lines; ++i)
		{
		  if ((n = getline(&buffer, &buffer_size, f)) == -1)
			break;

		  buffer[n-1]='\0'; // Remove the Newline (it kills Eink library)
	  
		  if (n>max_cols)
			buffer[max_cols]='\0';


		  /* turns out the libary BARFS (segfault) for characters outside range (they could check!) */
		  for (p=buffer; *p; ++p)
			{
			  if (*p < ' ' || *p > '~')
				(*p) = '.';
			}
	  
		  ga_text (display, xstart  , ystart+offset, fsize, buffer, gadget_colour);
		  offset += char_height;
		}
    }

  free(buffer); // needed to be dynamic as getline MAY have reallocated (probably didn't)

  return rc;
}

/*
 * ga_sleep - Just a regular sleep, put thread to sleep for some seconds (used to control screen updates (e.g once an hour)
 * unsigned int seconds - how long to sleep
 *
 * Before going to sleep, it releases the module (which puts it in a low power mode), then reinits the module once awake.
 */

int ga_sleep(int display, unsigned int seconds)
{
  sleep_display();

  Debug("Going to sleep (%d sec)\n", seconds);
  sleep(seconds);
  Debug("Awoke from sleep\n");

  init_display(); 

  return 0;
}

/* /usr/include/libproc2/misc.h */

// https://man7.org/linux/man-pages/man3/procps_misc.3.html
// man statvfs(3)

/*
 * ga_identify(): Identify the screens. On each screen (only 1 right now) display 
 * useful stuff about the screen. This would allow a persion watching to work out
 * which screen is which.
 */

void ga_identify()
{
  int display=0;
  struct display_settings *p;
  char buffer[256];
  
  for (display=0; (p=get_display(display)); ++display)
    {
      if (p->no_columns == 0) /* This display not defined */
	continue;

      ga_clear(display);
      
      sprintf(buffer, "Display: %d", display);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 0, 24, buffer, is_red_on_grey);

      sprintf(buffer, "Name: %s", p->local_name);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 25, 16, buffer, is_black_on_grey);

      sprintf(buffer, "Model: %s", p->part_name);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 50, 12, buffer, is_black_on_grey);

      sprintf(buffer, "%3d cols, %3d rows", p->no_columns, p-> no_rows);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 75, 16, buffer, is_black_on_grey);

      sprintf(buffer, "RST =%2d DC  =%2d", p->eink_rst_pin, p->eink_dc_pin);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 100, 16, buffer, is_black_on_grey);

      sprintf(buffer, "CS  =%2d BUSY=%2d", p->eink_cs_pin, p->eink_busy_pin);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 120, 16, buffer, is_black_on_grey);

      sprintf(buffer, "PWR =%2d", p->eink_pwr_pin);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 140, 16, buffer, is_black_on_grey);

      sprintf(buffer, "MOSI=%2d SCLK=%2d", p->eink_mosi_pin, p->eink_sclk_pin);
      fprintf(stderr, "%s\n", buffer);
      ga_text(0, 0, 160, 16, buffer, is_black_on_grey);

      ga_render(display);
    }
}


int ga_linux_temp(int display, UWORD xstart, UWORD ystart, int fsize, char *pathname, int limit)
{
	char	          buffer[32];
	int			      cputemp;
	enum  Eink_colour colour;
	FILE             *fp;
	size_t            len = 0;
	char			 *line = NULL;
	int			      rc = 0;
	float			  f;

	if ((fp=fopen(pathname, "r")) == NULL)
		{
			perror(pathname);
			exit(1);
		}

	if (getline(&line, &len, fp) > 0)
		{
			Debug("CPU TEMP says %s\n", line);
			if (sscanf(line, "%d", &cputemp) != 1)
				fprintf(stderr, "CPUTEMP: Bad data in %s\n", pathname);
		}
	fclose(fp);
	free(line);
		

	f = (cputemp+500)/1000;
	
	if (f > limit)   // So only whole degrees
		colour = is_red_on_grey;
	else
		colour = is_black_on_grey;

	snprintf(buffer, sizeof(buffer), "CPU:%4.1f", f);

	ga_text(display, xstart, ystart, fsize, buffer, colour);
	
	return rc;
}

/*
 * I initially understood this allowed for multiple temperature readings (core, sdram_c sdram_i sdram_p)
 * but it turns out this only for voltages. We will leave the other types in grammar, in case sombody wants to
 * add voltage readings. Here we will igore the temp_type (as we only have the one)
 */

int ga_vcore_temp(int display, UWORD xstart, UWORD ystart, int fsize, enum temp_type   type, int limit)
{
  char *send="measure_temp";
  char	buffer[32];
  char  receive[STRING_SIZE];
  float gputemp;
  enum  Eink_colour colour;
  char  scale;
  int   rc=0;

  if ((rc=mail_vcore(send, receive, STRING_SIZE)) !=0)
	fprintf(stderr, "Non-Zero code (%d) from measure temp request\n", rc);
  
  Debug("get_gputemp result was %s\n", receive);
  
  if (sscanf(receive, " temp=%f'%c", &gputemp, &scale) != 2)    // eg "temp=53.8'C"
    fprintf(stderr, "Bad data in response from MBOX\n");

  if (gputemp > limit)   // So only whole degrees
	colour = is_red_on_grey;
  else
	colour = is_black_on_grey;
  
  snprintf(buffer, sizeof(buffer), "GPU:%4.1f", gputemp);

  ga_text(display, xstart, ystart, fsize, buffer, colour);
  
  Debug("gputemp=%f, scale=%c\n", gputemp, scale);

  return rc;
}

/*--------------------------------------------------------
              Bit   Meaning
              ────  ────────────────────────────────────
               0    Under-voltage detected
               1    Arm frequency capped
               2    Currently throttled
               3    Soft temperature limit active
              16    Under-voltage has occurred
              17    Arm frequency capping has occurred
              18    Throttling has occurred
              19    Soft temperature limit has occurred
			  --------------------------------------------------------*/

/* could write thse as OTEMP (1>>19) if you find that easier */

#define  VOLT    0x00000001
#define  FREQ    0x00000002
#define  THROT   0x00000004
#define  ATEMP   0x00000008
#define  OVOLT   0x00010000
#define  OFREQ   0x00020000
#define  OTHROT  0x00040000
#define  OTEMP   0x00080000

struct tstates { int mask; char c;};

/*
 * So T/t is "throttled"  Uppercase is NOW, lowercase is history
 *    F/f is frequency (capped)
 *    V/v is Voltage (under)
 *    H/h is heat  lower case = active, uppercase = has occured
 */


static struct tstates states[] = {
  {THROT,  'T'},
  {OTEMP,  'H'},  // Heat
  {VOLT,   'V'},
  {FREQ,   'F'},

  {OTHROT, 't'},
  {ATEMP,  'h'},  // Heat
  {OVOLT,  'v'},
  {OFREQ,  'f'},
};

int  ga_throttle(int display, UWORD xstart, UWORD ystart, int fsize)
{
	char  buffer[32];
	char *send="get_throttled";
	char  receive[STRING_SIZE];
	enum  Eink_colour colour;
	int   throttled;
	int   i;
	char *p;
	
	int  rc=0;

	if ((rc=mail_vcore(send, receive, STRING_SIZE)) !=0)
		fprintf(stderr, "Non-Zero code (%d) from get_throttled request\n", rc);

	Debug("get_throttled result was %s\n", receive);

	if (sscanf(receive, " throttled=%x", &throttled) != 1)    // e.g. throttled=0x0
		fprintf(stderr, "Bad data in response from MBOX\n");

	// for debug throttled=(1<<18)|(1<<17)|(1<<19);
	
	if (throttled & THROT || throttled & OTHROT ) // is now, or has every been ...
		colour = is_red_on_grey;
	else
		colour = is_black_on_grey;

	snprintf(buffer, sizeof(buffer), "TH:%#08X" , throttled);

	if (throttled)
	  {
		/* this bit could be made conditional on "text mode" */
		p=&buffer[3];

		for (i=0; i<(sizeof states/sizeof (struct tstates)); ++i)  // curently 8
		  {
			if (throttled & states[i].mask)
			  *p++ = states[i].c;
		  }
		*p++='\0';
	  }
		  
	ga_text(display, xstart, ystart, fsize, buffer, colour);

	return rc;
}

int		ga_fan       (int display, UWORD xstart, UWORD ystart, int fsize, char *pathname, int limit)
	
{
	char	          buffer[32];
	int			      rpm;
	enum  Eink_colour colour;
	FILE             *fp;
	size_t            len = 0;
	char			 *line = NULL;
	int			      rc = 0;


	if ((fp=fopen(pathname, "r")) == NULL)
		{
			perror(pathname);
			exit(1);
		}

	if (getline(&line, &len, fp) > 0)
		{
			Debug("Fan says %s\n", line);
			if (sscanf(line, "%d", &rpm) != 1)
				fprintf(stderr, "FAN: Bad data in %s\n", pathname);
		}
	fclose(fp);
	free(line);
		
	
	if (rpm > limit)
		colour = is_red_on_grey;
	else
		colour = is_black_on_grey;

	snprintf(buffer, sizeof(buffer), "Fan:%05d", rpm);

	ga_text(display, xstart, ystart, fsize, buffer, colour);
	
	return rc;
}

void ga_xxxxx()
{
}
