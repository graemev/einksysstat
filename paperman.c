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
  struct timespec start={0,0}, finish={0,0};


  fprintf(stderr, "Init 1\n");
  
  if(DEV_Module_Init()!=0)
    {
      fprintf(stderr, "Failed to initalise display\n");
      exit(1);
    }
  fprintf(stderr, "Init 1 Done\n");



  
  fprintf(stderr, "Exit 1\n");
  DEV_Module_Exit();
  fprintf(stderr, "Exit 1 done\n");

  fprintf(stderr, "Init 2\n");
  if(DEV_Module_Init()!=0)
    {
      fprintf(stderr, "Failed to initalise display\n");
      exit(1);
    }
  fprintf(stderr, "Init 2 Done\n");

  fprintf(stderr, "Exit 2\n");
  DEV_Module_Exit();
  fprintf(stderr, "Exit 2 Done\n");
  

  
  return 0;
}

