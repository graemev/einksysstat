/*
 * module.h:
 * Code relating to the "module" (a PCB) used to hold the eInk device.
 *
 * This stripped down to JUST the -llgpio implementation for the Raspberry PI (4 & 5)
 *
 * The vast majority of this code comes from DEV_Config.h in the waveshare
 * project @ https://github.com/waveshareteam/e-Paper
 * 
 * Modded to follow normal C convections (variables lowercase, MACROs and constants upper case, types capitalised)
 * 
 * 
 * Their PD statement reproduced below
 *
 */

/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V2.0
* | Date        :   2018-10-30
* | Info        :
* 1.add:
*   UBYTE\UWORD\UDOUBLE
* 2.Change:
*   EPD_RST -> EPD_RST_PIN
*   EPD_DC -> EPD_DC_PIN
*   EPD_CS -> EPD_CS_PIN
*   EPD_BUSY -> EPD_BUSY_PIN
* 3.Remote:
*   EPD_RST_1\EPD_RST_0
*   EPD_DC_1\EPD_DC_0
*   EPD_CS_1\EPD_CS_0
*   EPD_BUSY_1\EPD_BUSY_0
* 3.add:
*   #define DEV_Digital_Write(_pin, _value) bcm2835_GPIOI_write(_pin, _value)
*   #define DEV_Digital_Read(_pin) bcm2835_GPIOI_lev(_pin)
*   #define DEV_SPI_WriteByte(__value) bcm2835_spi_transfer(__value)
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


/* Autoconf */
#include "config.h" 


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "Debug.h"


#include <lgpio.h>
#include <pthread.h>

#define LFLAGS 0
#define NUM_MAXBUF  4


extern int EINK_RST_PIN;
extern int EINK_DC_PIN;
extern int EINK_CS_PIN;
extern int EINK_BUSY_PIN;
extern int EINK_PWR_PIN;
extern int EINK_MOSI_PIN;
extern int EINK_SCLK_PIN;




/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

extern UBYTE module_turn_on(void);
extern void  gpio_init(void);
extern void  gpio_mode(UWORD Pin, UWORD Mode);       // Mode0 == Input , Mode1 == Output
extern void  gpio_write(UWORD Pin, UBYTE Value);  // Write value to Pin
extern UBYTE gpio_read(UWORD Pin);                // Read a value from Pin
extern void  gpio_delay(UDOUBLE xms);
extern void  module_turn_off(void);
extern void  spi_writebyte(uint8_t Value);


extern pthread_mutex_t eink_mutex;
extern pthread_cond_t  eink_cond;

