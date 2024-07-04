/*
 * Code relating to the "module" (a PCB) used to hold the eInk device.
 *
 * This stripped down to JUST the -llgpio implementation for the Raspberry PI (4 & 5)
 *
 * The vast majority of this code comes from DEV_Config.c in the waveshare
 * project @ https://github.com/waveshareteam/e-Paper
 * 
 * Modded to follow normal C convections (variables lowercase, MACROs and constants upper case, types capitalised)
 * 
 * 
 * Their PD statement reproduce below
 *
 *
 */

/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
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



#include "module.h"

/* ============================ static =================================*/



static int detect_os(void)
{
  FILE *fp;
  char  issue_str[64];
  char  systems[][9] = {"Raspbian", "Debian", "NixOS"};
  int   detected = 0;

  fp = fopen("/etc/issue", "r");
  if (fp == NULL)
    {
      Debug("Unable to open /etc/issue");
      return -1;
    }
  if (fread(issue_str, 1, sizeof(issue_str), fp) <= 0)
    {
      Debug("Unable to read from /etc/issue");
      return -1;
    }
  
  issue_str[sizeof(issue_str)-1] = '\0';
  fclose(fp);

  Debug("Current environment: ");

  for(int i=0; i<3; i++)
    {
      if (strstr(issue_str, systems[i]) != NULL) {
	Debug("%s\n", systems[i]);
	detected = 1;
      }
    }
  
  if (!detected)
    {
      fprintf(stderr,"not recognized\n");
      fprintf(stderr,"Built for Raspberry Pi, but unable to detect environment Linux variant.\n");

      return -1;
    }

  return 0;
}

/* ============================ exposed =================================*/

static int GPIO_Handle;
static int SPI_Handle;

/**
 * GPIO -- NB accessed in device specific code
**/
int EINK_RST_PIN;
int EINK_DC_PIN;
int EINK_CS_PIN;
int EINK_BUSY_PIN;
int EINK_PWR_PIN;
int EINK_MOSI_PIN;
int EINK_SCLK_PIN;




/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE module_turn_on(void)
{
  Debug("/***********************************/ \n");

  if(detect_os() < 0)
    return 1;
  
  Debug("Init with RPI defined\n");
  Debug("Init with USE_LGPIO_LIB defined\n");
  
  char buffer[NUM_MAXBUF];
  FILE *fp;

  fp = popen("cat /proc/cpuinfo | grep 'Raspberry Pi 5'", "r");
  if (fp == NULL)
    {
      fprintf(stderr, "It is not possible to determine the model of the Raspberry PI\n");
      return -1;
    }

  if(fgets(buffer, sizeof(buffer), fp) != NULL)  // If it is a Raspberry PI5
    {
      GPIO_Handle = lgGpiochipOpen(4);
      if (GPIO_Handle < 0)
        {
	  Debug( "gpiochip4 Export Failed\n");
	  return -1;
        }
    }
  else					   // Not a PI5 (probably assumes PI4 here)
    {
      GPIO_Handle = lgGpiochipOpen(0);
      if (GPIO_Handle < 0)
        {
	  Debug( "gpiochip0 Export Failed\n");
	  return -1;
        }
    }
  
  SPI_Handle = lgSpiOpen(0, 0, 10000000, 0);
  gpio_init();

   Debug("/***********************************/ \n");
   
  return 0;
}



void gpio_init(void)
{
  Debug("gpio_init\n");

  EINK_RST_PIN     = 17;
  EINK_DC_PIN      = 25;
  EINK_CS_PIN      = 8;
  EINK_PWR_PIN     = 18;
  EINK_BUSY_PIN    = 24;
  EINK_MOSI_PIN    = 10;
  EINK_SCLK_PIN    = 11;

  gpio_mode(EINK_BUSY_PIN, 0);      // BUSY= Input
  gpio_mode(EINK_RST_PIN, 1);       // RST = Output
  gpio_mode(EINK_DC_PIN, 1);        // DC  = Output
  gpio_mode(EINK_CS_PIN, 1);        // CS  = Output
  gpio_mode(EINK_PWR_PIN, 1);       // PWR = Output

  // gpio_mode(EINK_MOSI_PIN, 0);
  // gpio_mode(EINK_SCLK_PIN, 1);

  gpio_write(EINK_CS_PIN, 1);    // write 1 to CS
  gpio_write(EINK_PWR_PIN, 1);   // write 1 to PWR (turn on power?)
}


/**
 * GPIO Mode
**/
void gpio_mode(UWORD Pin, UWORD Mode)   // Mode0 == Input , Mode1 == Output
{
  if(Mode == 0 || Mode == LG_SET_INPUT)
    {
      lgGpioClaimInput(GPIO_Handle,LFLAGS,Pin);
      Debug("IN Pin = %d\n",Pin);
    }
  else
    {
      lgGpioClaimOutput(GPIO_Handle, LFLAGS, Pin, LG_LOW);
      Debug("OUT Pin = %d\n",Pin);
    }
}


/**
 * GPIO read and write
**/
void gpio_write(UWORD Pin, UBYTE Value)  // Write value to Pin
{
  lgGpioWrite(GPIO_Handle, Pin, Value);
}

UBYTE gpio_read(UWORD Pin)
{
  UBYTE Read_value = 0;
  Read_value = lgGpioRead(GPIO_Handle,Pin);
  return Read_value;
}


/**
 * delay x ms  (only does whole seconds so TBD)
**/
void gpio_delay(UDOUBLE xms)
{
  lguSleep(xms/1000.0);
}


/******************************************************************************
  TBD ... seems to not be defined for LGPIO
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void module_turn_off(void)
{
#if USE_LGPIO_LIB 
  // gpio_write(EINK_CS_PIN, 0);
  // gpio_write(EINK_PWR_PIN, 0);
  // gpio_write(EINK_DC_PIN, 0);
  // gpio_write(EINK_RST_PIN, 0);
  // lgSpiClose(SPI_Handle);
  // lgGpiochipClose(GPIO_Handle);
#elif USE_DEV_LIB
  DEV_HARDWARE_SPI_end();
  gpio_write(EINK_CS_PIN, 0);
  gpio_write(EINK_PWR_PIN, 0);
  gpio_write(EINK_DC_PIN, 0);
  gpio_write(EINK_RST_PIN, 0);
  GPIOD_Unexport(EINK_PWR_PIN);
  GPIOD_Unexport(EINK_DC_PIN);
  GPIOD_Unexport(EINK_RST_PIN);
  GPIOD_Unexport(EINK_BUSY_PIN);
  GPIOD_Unexport_GPIO();
#endif
}


/**
 * SPI
**/
void spi_writebyte(uint8_t Value)
{
  lgSpiWrite(SPI_Handle,(char*)&Value, 1);
}
