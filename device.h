/*
 * device.h
 *
 * Code relating to the "device" ie the eink "PAPER" itself (as opposed the PCB module which drives it)
 *
 * This stripped down to JUST the -llgpio implementation for the Raspberry PI (4 & 5)
 *
 * The vast majority of this code comes from EPD_1in54b_V2.h in the waveshare
 * project @ https://github.com/waveshareteam/e-Paper
 * 
 * Modded to follow normal C convections (variables lowercase, MACROs and constants upper case, types capitalised)
 * 
 * 
 * Their PD statement reproduced below
 *
 */





/*****************************************************************************
* | File      	:   device.h
* | Author      :   Waveshare team
* | Function    :   1.54inch e-paper b V2
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-04-02
* | Info        :
* -----------------------------------------------------------------------------
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
#ifndef __DEVICE_H_
#define __DEVICE_H_

#include "module.h"

// Display resolution
#define EINK_WIDTH       200
#define EINK_HEIGHT      200

void eink_init   (void);
void eink_clear  (void);
void eink_display(const UBYTE *blackimage, const UBYTE *redimage);
void eink_deepsleep  (void);

#endif
