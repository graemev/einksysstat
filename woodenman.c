/*****************************************************************************
* | File      	:   strawman.c
* | Author      :   Graeme Vetterlein
* | Function    :   1.54inch B e-paper PoC display NAS stats on e-Ink panel
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

#include "eink_sysstat.h"


static struct action actions[] =
  {
    //    {do_init_image     , .args.init.colour    =is_black_on_grey, .args.init.rotate    =90},
    // {do_hostname , .args.hostname.xstart=0,                .args.hostname.ystart=0, .args.hostname.fsize=12},
    // {A3(timestamp,xstart=75,ystart=0,fsize=12)},

    
    DEF_HOSTNAME(0,0,12),
    DEF_TIMESTAMP(75,0,12),
    
    DEF_UPTIME(0,13,16),

    
    DEF_DF(30 ,24,"/lost+found",               "root", df_best,20),
    DEF_DF(55 ,24,"/boot/firmware/config.txt", "SD",   df_best,20),           /* line  55 */
    DEF_DF(80 ,24,"/Q/mount/rest3/lost+found", "NV1",  df_best,20),           /* line  80 */
    DEF_DF(105,24,"/dev/nvme0n1p3/lost+found", "NV2",  df_best,20),           /* line 105 */

    
    DEF_AGE(0,130, 24,"/boot/firmware/config.txt", "BACKUP1",  age_days,20),  /* line 130 */
    DEF_TEXT(0,155, 24,"Woodenman", is_black_on_red), /* line 155 */
    DEF_FILE(0, 180, 8, "/etc/motm", 0),                                      /* line 180 */

    DEF_RENDER(),
    DEF_SLEEP(5),

    DEF_CLEAR(),
  };




int main(void)
{
  Debug("Init display\n");
  ga_init_module();  // Not available as an ACTION (only done once)
  ga_init_display();  // Not available as an ACTION (only done once)

  Debug("Init image arrays (malloc(3)\n");
  ga_init_image(is_black_on_grey,  90);   // NB malloc(3)s memory

  
  const int bounds = sizeof(actions)/sizeof(struct action);
  int       i;


  for (;/*ever*/;)
    {

      for (i=0; i<bounds; ++i)
	{
	  Debug("Doing action: %s\n", str_action(&(actions[i])));
	  do_action(&(actions[i]));
	  Debug("Done action: %s\n", str_action(&(actions[i])));
	}
    }


  /* three stages to exit & cleanup:
   *
   * 1: Free up the memory we used to store images
   * 2: put the eInk device into a deepsleep
   * 3: Power off the module connected to GPIO pins (and free up the pins)
   *
   * Right now 3 is not possible (but is required) becasue the sample code was just comments
   * and the device is not documented.
   */

  
  Debug("Release image arrays (free(3)\n");
  ga_release_image();		      // NB free(3)s memory

  Debug("Release module\n");
  ga_release_module();
  
  return 0;
}

