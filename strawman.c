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
#include "EPD_1in54b_V2.h"  // Hardware specific routines (e.g. what pins to toggle to move canvas onto actual display)
#include "time.h"











int main(void)
{
    ga_display_init();
    ga_image_init(is_black_on_grey, 90);

    ga_hostname(0,0,12);                    /* line 0 */
    ga_timestamp(75,0,12);

    ga_uptime(0,13,16);                     /* line 13 */

    ga_df(30 ,24,"/lost+found",               "root", df_best,20);          /* line  30 */   // Need at least 75% free
    ga_df(55 ,24,"/boot/firmware/config.txt", "SD",   df_best,20);          /* line  55 */
    ga_df(80 ,24,"/Q/mount/rest3/lost+found", "NV1",  df_best,20);          /* line  80 */
    ga_df(105,24,"/dev/nvme0n1p3/lost+found", "NV2",  df_best,20);          /* line 105 */
    ga_age(0,130, 24,"/boot/firmware/config.txt", "BACKUP1",  age_days,20); /* line 130 */
    ga_age(0,155, 24,"/home/graeme/src",          "BACKUP2",  age_days,180); /* line 155 */
    ga_file(0, 180, 8, "/etc/motm", 0);                                      /* line 180 */
    
#if 0

    ga_set_colour(is_black_on_red);
    ga_hostname(0,80,16);
    ga_meter   (150,75,24,80, is_red_on_black);   /* line 80 */

    
    ga_set_colour(is_red_on_black); /* Line 100 */
    ga_hostname(0,100,16);
    ga_meter(150,100,24,85, is_black_on_red);



    ga_set_colour(is_black_on_grey);  /* line 130 */
    ga_text(0,130, 16, "ABCDEFGHIJKLMNO");
    ga_meter(150,130,16,85, is_black_on_grey);


    ga_set_colour(is_red_on_grey);  /* Line 170 */
    ga_text(0,170, 24, "ABCDEFGHI");
    ga_meter(150,170,24,85, is_black_on_grey);

#endif

    printf("Drawing------------------------\r\n");


    ga_render();
    DEV_Delay_ms(2000);

    fprintf(stderr, "Are we there yet? ...can you see image?\n");

    ga_image_release();
    ga_display_release();
    
    return 0;
}

