/*****************************************************************************
* | File      	:   gadgets.c
* | Author      :   Graeme Vetterlein
* | Function    :   1.54inch B e-paper PoC display NAS stats on e-Ink panel (collection of gadgets)
* | Info        :
*----------------
* |	This version:   V0.1
* | Date        :   2024-06-20
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

#include "device.h"  
#include <GUI_Paint.h>

#include "time.h"
#include <libproc2/misc.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>

#include "gadgets.h"

// ---------------- internal ----------------


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


enum Eink_colour ga_set_private_colour(enum Eink_colour colour, struct gc *lc)
{
  switch (colour)
    {
    deefault:
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


enum Eink_colour ga_set_colour(enum Eink_colour colour)
{
  gadget_colour = colour;

  ga_set_private_colour(colour, &gc);

  return gadget_colour;
}


/*
 * ga_init_module() - Logically initally setup module (eg enable the SPI pins)
 */

void ga_init_module()
{
  Debug("ga_init_module start\n");

  init_module(); // Twiddle with GPIO pins to setup the module
  
  Debug("ga_init_module end\n");
  
  return;
}


/*
 * ga_init_display() - Logically initally setup display [1IN54B_V2] (eg enable the SPI pins) Only ever done once
 */

void ga_init_display()
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


void ga_init_image(enum Eink_colour colour, UWORD rotate)
{
  struct timespec start={0,0}, finish={0,0}; 
  
  clock_gettime(CLOCK_REALTIME,&start);

  Debug("ga_image_init start\n");
  init_images(&black_image, &red_image);
  ga_set_colour(colour);
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

void ga_release_image()
{
  free(black_image);
  free(red_image);
  black_image = NULL;
  red_image   = NULL;

  return;
}


void ga_release_module()
{
  struct timespec start={0,0}, finish={0,0};
  clock_gettime(CLOCK_REALTIME,&start);
  gpio_delay(2000);//important, at least 2s
  // close 5V
  Debug("close 5V, Module enters 0 power consumption ...\r\n");
  module_turn_off();
  clock_gettime(CLOCK_REALTIME,&finish);
  
  Debug("%ld Seconds to ga_display_release\n",finish.tv_sec-start.tv_sec);

  return;
}


void ga_render()  // Take the images abd display on the eInk
{
  eink_display(black_image, red_image);
}

void ga_clear()  // Just clear the screen (not full setup) then clear out teh red & black images
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



/* ======================================== The Gadets proper ==================================

/*
 * ga_hostname(UWORD Xstart, UWORD Ystart, sFONT* Font) -- Display Hostname at specified point , in specific font
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 */
int ga_text(UWORD xstart, UWORD ystart, int fsize, char text[], enum Eink_colour colour)
{
  int        rc=0;
  sFONT     *font;
  struct gc  lc;
  
  font = get_font(fsize);

  (void)ga_set_private_colour(colour, &lc);  // Private colours (eg based on value)
  
  Debug("Drawing------------------------\n");

  Paint_SelectImage(black_image);
  Paint_DrawString_EN(xstart, ystart, text, font, lc.black_fg, lc.black_bg);

  Paint_SelectImage(red_image);
  Paint_DrawString_EN(xstart, ystart, text, font, lc.red_fg, lc.red_bg);
  
  return rc;
}


/*
 * ga_hostname(UWORD Xstart, UWORD Ystart, sFONT* Font) -- Display Hostname at specified point , in specific font
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 */
int ga_hostname(UWORD xstart, UWORD ystart, int fsize)
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
int ga_timestamp(UWORD xstart, UWORD ystart, int fsize)
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


int ga_uptime(UWORD xstart, UWORD ystart, int fsize)
{
  sFONT     *font;

  char       stamp[64];

  time_t     now;
  struct tm  now_parts;

  double  uptime_secs;
  char    buffer[64];
  time_t  uptime;
  int	  secs;
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

      secs = uptime % 60;
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


int ga_meter(UWORD xstart, UWORD ystart, int fsize, int value, enum Eink_colour colour)
{
  int rc=0;
  int width=50;
  int fwidth;
  int height=fsize;

  struct gc lc;
  
  /* if value =0 -> 100, and widdth is 50 , tehn every "2" pints in value represents a fill with of 1 */
  fwidth = value/(100/width);



  (void)ga_set_private_colour(colour, &lc);  // Private colours (eg based on value)
  
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
 * ga_df(UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff)
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
/* ga_df(50, 0, int 24, "/", "root",df_geg)                          --> root 19G [###       ] */
/* ga_df(50, 0, int 24, "/dev/disk/by-label/root5", "root",df_pcent) --> root 70% [###       ] */


/* alwayas sarts in col 0 and always fonr24 */

int ga_df(UWORD ystart, int fsize, char *device, char * label, enum df_units units, int cutoff)
{
  int   rc=0;
  int	pcent_free;
  char  value[9];   // In whatever united specified
  int	bpm;	        // blocks per megabyte

  enum Eink_colour colour;
  
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
	  sprintf(value, "%s %dM", label, s.f_bfree/bpm);
	  break;
		  
	case (df_geg):
	  sprintf(value, "%s %dG", label, s.f_bfree/bpm/1024);
	  break;
		  
	case (df_pcent):
	  sprintf(value, "%s %d%%", label, pcent_free);
	  break;
	}
    }     


  if (stat_ok == false || pcent_free < cutoff)
    colour = is_red_on_grey;
  else
    colour = is_black_on_grey;

  
  ga_text (0  , ystart, fsize, value, colour);

  if (stat_ok)
    ga_meter(150, ystart, fsize, 100-pcent_free, colour);
  else
    ga_text(150, ystart, 16, "UNAV", colour);

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
 *
 *
 *
 *
 *
 *
 */

int ga_age(UWORD xstart, UWORD ystart, int fsize, char *filename, char * label, enum age_units units, int cutoff)
{
  int          rc=0;
  int	       pcent_free;
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
  

  ga_text(xstart, ystart, fsize, buffer, colour);
}

/* int ga_file(UWORD xstart, UWORD ystart, int fsize, char *filename, int lines)
 *
 * Display some lines from a file.
 *
 * UWORD xstart - X co-ord
 * UWORD ystart - Y co-ord
 * int   fsize  - Font size (8,12,16,20,24 Only)
 * char *filename - file where text comes from
 * int   lines  - number of liines to show 0 = maximum possible
 */





int ga_file(UWORD xstart, UWORD ystart, int fsize, char *filename, int lines)
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
      ga_text (xstart, ystart, fsize, buffer, gadget_colour);
    }
  else
    {
      offset=0;
      for (i=0; i<max_lines; ++i)
	{
	  if ((n = getline(&buffer, &buffer_size, f)) == -1)
	    {
	      perror(filename);
	      n  = 0;
	      rc = errno;
	    }

	  buffer[n-1]='\0'; // Remove the Newline (it kills Eink library)
	  
	  if (n>max_cols)
	    buffer[max_cols]='\0';


	  /* turns out the libary BARFS (segfault) for characters outside range (they could check!) */
	  for (p=buffer; *p; ++p)
	    {
	      if (*p < ' ' || *p > '~')
		(*p) = '.';
	    }
	  
	  ga_text (xstart  , ystart+offset, fsize, buffer, gadget_colour);
	  offset += char_height;
	}
    }

  free(buffer); // needed to be dynamic as getline MAY have reallocated (probably didn't)
  
}


/*
 * ga_sleep - Just a regular sleep, put thread to sleep for some seconds (used to control screen updates (e.g once an hour)
 * unsigned int seconds - how long to sleep
 *
 * Before going to sleep, it releases the module (which puts it in a low power mode), then reinits the module once awake.
 *
 *
 */

int ga_sleep(unsigned int seconds)
{
  sleep_display();

  Debug("Going to sleep (%d sec)\n", seconds);
  sleep(seconds);
  Debug("Awoke from sleep\n");

  init_display(); 
}


/* /usr/include/libproc2/misc.h */
/* int   procps_uptime (double *uptime_secs, double *idle_secs); */

// https://man7.org/linux/man-pages/man3/procps_misc.3.html

// man statvfs(3)
