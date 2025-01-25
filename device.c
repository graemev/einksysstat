/* -*- Mode: c; tab-width: 4;c-basic-offset: 4; c-default-style: "gnu" -*- */
/*
 * device.c
 *
 * Code relating to the "device" ie the eink "PAPER" itself (as opposed the PCB module which drives it)
 *
 * I can find NO documentation on eg the "magic codes" used to drive the eInk paper, just the uncommented examples herein
 *
 *
 *
 *
 * This stripped down to JUST the -llgpio implementation for the Raspberry PI (4 & 5)
 *
 * The vast majority of this code comes from EPD_1in54b_V2.c in the waveshare
 * project @ https://github.com/waveshareteam/e-Paper
 * 
 * Modded to follow normal C convections (variables lowercase, MACROs and constants upper case, types capitalised)
 * 
 * 
 * Their PD statement reproduced below
 *
 */



/*****************************************************************************
* | File      	:   EPD_1in54b_V2.h
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
#include <fcntl.h>
#include <lgpio.h>
#include <stdlib.h>

#include "device.h"
#include "Debug.h"

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/



static void eink_reset(void)
{
    gpio_write(EINK_RST_PIN, 1);
    gpio_delay(100);
    gpio_write(EINK_RST_PIN, 0);
    gpio_delay(2);
    gpio_write(EINK_RST_PIN, 1);
    gpio_delay(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void eink_sendcommand(UBYTE Reg)
{
    gpio_write(EINK_DC_PIN, 0);
    gpio_write(EINK_CS_PIN, 0);
    spi_writebyte(Reg);
    gpio_write(EINK_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void eink_senddata(UBYTE Data)
{
    gpio_write(EINK_DC_PIN, 1);
    gpio_write(EINK_CS_PIN, 0);
    spi_writebyte(Data);
    gpio_write(EINK_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:

GPV 13Jan24 As described by rohoog ( https://github.com/rohoog ) this readbusy
            locks up a CPU as 100% busy. He implemented various schemes to avoid
            this; however his work predated -llgpio (define USE_LGPIO_LIB) so he
            has no solution for this. This code is cutdown to ONLY use -llgpio,
            so we need to reimplement.  This is mostly a "monkey-see-monkey-do"
            implementation (as I have little understanding of GPIO) but starts
	    with man lgpio(3) then search for lgNotifyOpen(void), it has an example.

******************************************************************************/
static void eink_readbusy(void) // Wait for the EINK_BUSY_PIN to goto 0 (without just spinning)
{

#if DEBUG
#include <sys/time.h>

	struct timeval  start;
	struct timeval  end;

	gettimeofday(&start, NULL);
#endif

#undef JUSTSPIN

#ifdef JUSTSPIN
  
    Debug("e-Paper busy (JUSTSPIN)\r\n");
    while(1) {
        if(gpio_read(EINK_BUSY_PIN) == 0)
            break;
    }
    gpio_delay(200);
    Debug("e-Paper busy release(JUSTSPIN)\r\n");
#else

    Debug("e-Paper poll for not busy\r\n");


    /* Impossible to find any documentation on how to use lgNotifyXXXX .
     *
     * My best guess is a lgGpioReport_t (struct 16 bytes) can be read from the
     * FIFO/Pipe if there is no data avaible, it will block. One problem is
     * while notifcations are active there could be many such events. We might
     * read one relating to a much earlier time.  There is a suspend/resume
     * logic available but this feels prone to race conditions. The approach
     * we'll adopt here is we will check if BUSY is realy 0, if so we're
     * done. Otherwise we proably want to sleep. We can read the FIFO and
     * (hopefully block) BUT there could be some old messages queued. So we'll
     * check is BUSY is 0 , if not then we rise & repeat.
     *
     * Really all we want is "sleep until something changes" (AKA poll(2)) we
     * can work out if it's what we wanted when we wake up.
     */

    int trys=0;

    pthread_mutex_lock(&eink_mutex);	
	
    while(1)
		{
			if(gpio_read(EINK_BUSY_PIN) == 0)
				break;
 
			struct timespec ts;

			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 30;							 // After 30 seconds give up (we will check pin again)

			pthread_cond_timedwait(&eink_cond, &eink_mutex, &ts);  // go to sleep until BUSY changes (or timelimit)
			++trys;
		}

    pthread_mutex_unlock(&eink_mutex);
	
    gpio_delay(200);
    Debug("e-Paper no longer busy (after %d trys)\r\n", trys);

#if DEBUG
	{
		float duration;
		gettimeofday(&end, NULL);

		duration = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;

		Debug("eink_readybusy: paused for %f Seconds\n",
			  duration/1000000.0);
	}
#endif
#endif // JUSTSPIN
	
}


/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void eink_init(void)
{
    eink_reset();
    
    eink_readbusy();   
    eink_sendcommand(0x12);  //SWRESET
    eink_readbusy();   

    eink_sendcommand(0x01); //Driver output control      
    eink_senddata(0xC7);
    eink_senddata(0x00);
    eink_senddata(0x01);

    eink_sendcommand(0x11); //data entry mode       
    eink_senddata(0x01);

    eink_sendcommand(0x44); //set Ram-X address start/end position   
    eink_senddata(0x00);
    eink_senddata(0x18);    //0x18-->(24+1)*8=200   ( why obfuscate? , if it's 24 , say 24 in decimal)

    eink_sendcommand(0x45); //set Ram-Y address start/end position          
    eink_senddata(0xC7);    //0xC7-->(199+1)=200    ( why obfuscate? , just pass 199 (AKA 0xC7) decimal)
    eink_senddata(0x00);
    eink_senddata(0x00);
    eink_senddata(0x00); 

    eink_sendcommand(0x3C); //BorderWavefrom
    eink_senddata(0x05);

    eink_sendcommand(0x18); //Read built-in temperature sensor
    eink_senddata(0x80);

    eink_sendcommand(0x4E);   // set RAM x address count to 0;
    eink_senddata(0x00);
    eink_sendcommand(0x4F);   // set RAM y address count to 0X199;    
    eink_senddata(0xC7);
    eink_senddata(0x00);



    eink_readbusy();
}

/*
 * Free up stuff we allocated during eink_init()
 */

void eink_close(void)
{



}


/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void eink_clear(void)
{
    
    unsigned int i;
    Debug("eink_clear(void) ..clear the screen\n");
    eink_sendcommand(0x24);   //write RAM for black(0)/white (1)
    for(i=0;i<5000;i++)       // (FYI 200*200/8 = 5000, they could have written that, the compiler will fold it)
    {               
        eink_senddata(0xff);
    }
    eink_sendcommand(0x26);   //write RAM for black(0)/white (1)
    for(i=0;i<5000;i++)
    {               
        eink_senddata(0x00);
    }
    eink_sendcommand(0x22); //Display Update Control
    eink_senddata(0xF7);   
    eink_sendcommand(0x20);  //Activate Display Update Sequence
    eink_readbusy();   
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void eink_display(const UBYTE *blackimage, const UBYTE *redimage)
{
    UWORD Width, Height;
    unsigned int i;	

    Width = (EINK_WIDTH % 8 == 0)? (EINK_WIDTH / 8 ): (EINK_WIDTH / 8 + 1);
    Height = EINK_HEIGHT;

    eink_sendcommand(0x24);   //write RAM for black(0)/white (1)
    for(i=0;i<Width*Height;i++)
    {               
        eink_senddata(blackimage[i]);
    }

    eink_sendcommand(0x26);   //write RAM for black(0)/white (1)
    for(i=0;i<Width*Height;i++)
    {               
        eink_senddata(~redimage[i]);
    }
    
    eink_sendcommand(0x22); //Display Update Control
    eink_senddata(0xF7);   

    eink_sendcommand(0x20);  //Activate Display Update Sequence
    eink_readbusy();   
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void eink_deepsleep(void)
{
    eink_sendcommand(0x10); //enter deep sleep
    eink_senddata(0x01); 
    gpio_delay(100);
}
