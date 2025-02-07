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

#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

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

#define	MAX_DEVICES 3


static int GPIO_Handle;
static int SPI_Handle;

pthread_mutex_t eink_mutex;
pthread_cond_t  eink_cond;


static struct display_settings  display_settings[MAX_DEVICES];


struct display_settings * get_display(int i)
{
  struct display_settings *p;
  
  if (i>=MAX_DEVICES)
    p=NULL;
  else
    p=&display_settings[i];
  return (p);
}


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

/*
 * We get call whenever the state of the BUSY pin changes
 * we wake up anybody wait foe the BUSY pin and let them figure out if it's now in a state they need.
 */


/*
 * typedef struct lgGpioAlert_s
 *      {
 *         lgGpioReport_t report;
 *         int nfyHandle;
 *      } lgGpioAlert_t, *lgGpioAlert_p;
 *
 *
 * typedef struct
 *          {
 *             uint64_t timestamp; // alert time in nanoseconds
 *             uint8_t chip;       // gpiochip device number
 *             uint8_t gpio;       // offset into gpio device
 *             uint8_t level;      // 0=low, 1=high, 2=timeout
 *             uint8_t flags;      // none currently defined
 *          } lgGpioReport_t;
 *
 *
 *
 *
 *
 */


void watch_busy_pin(int e, lgGpioAlert_p evt, void *data)
{
  //  Debug("watch_busy_pin triggered\n");

  pthread_mutex_lock(&eink_mutex); // avoid race with sleeper

  int i;

  Debug("Busy PIN alerts e=%d\n", e);
  
  for (i=0; i<e; i++)
    {

      Debug("Alert[%d] Time:%ld chip:%d GPIO_PIN:%d: Level=%d, Flags(zero)=%d\n" ,
	    i,
	    evt[i].report.timestamp,
	    evt[i].report.chip,
	    evt[i].report.gpio,
	    evt[i].report.level,
	    evt[i].report.flags);

    }

  
  pthread_cond_signal(&eink_cond); // Wake em up

  pthread_mutex_unlock(&eink_mutex);
}

  


/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE module_turn_on(void)
{
  struct stat    spistat;
  FILE	        *spif;
  struct passwd *passwd;

  
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
	  Debug( "/dev/gpiochip4 (Pi5) Export Failed (%s) look at permissions\n",lguErrorText(GPIO_Handle) );
	  return -1;
        }
    }
  else					   // Not a PI5 (probably assumes PI4 here)
    {
      GPIO_Handle = lgGpiochipOpen(0);
      if (GPIO_Handle < 0)
        {
	  Debug( "/dev/gpiochip0 (Pi4) Export Failed (%s) look at permissions\n",lguErrorText(GPIO_Handle) );
	  return -1;
        }
    }

  /* Just do a few checks, e.g. permissions, witout all these it's likely it will silently fail to updta the eInink device */

  if (stat("/dev/spidev0.0", &spistat) == -1)
    {
      perror("stat of /dev/spidev0.0 failed, have you enabled SPI?\n");
      // we don't exit (non fatal)
      spistat.st_mode = 0;  // Not a char device
    }

  if ( (spistat.st_mode&S_IFMT) != S_IFCHR)
    {
      fprintf(stderr, "/dev/spidev0.0 does not exist as a character device\n"
	              "Likey this is because SPI has not been enabled\n"
	              "Read https://www.waveshare.com/wiki/1.54inch_e-Paper_Module_(B)_Manual#Working_With_Raspberry_Pi\n"
	              "use 'sudo raspi-config' or edit '/boot/config.txt' to include 'dtparam=spi=on'\n"
	              "This code will continue, but likey NO output will appear. This is in case some time has past\n"
	              "since this code was written (Jan2025) and e.g. the device filename has changed. If that happens FIXME.\n");
    }



  if ((spif=fopen("/dev/spidev0.0", "rw")))
        fclose(spif);
  else
    {
      perror("fopen /dev/spidev0.0 as RW");
      passwd=getpwuid(getuid());
      fprintf(stderr, "You probably lack permissions, did you add the groups spi & gpio to this user (%s)?\n"
	              "Most likley you will see no output on the eInk display now\n"
	              "to fix permissions: sudo usermod %s -a -G spi,gpio\n",
	      passwd->pw_name, passwd->pw_name);
    }

  
  SPI_Handle = lgSpiOpen(0, 0, 10000000, 0);
  gpio_init();


  //  lgGpioSetSamplesFunc(watch_busy_pin, NULL);
  lgGpioSetAlertsFunc(GPIO_Handle, EINK_BUSY_PIN, watch_busy_pin,  NULL);
  lgGpioClaimAlert(GPIO_Handle, 0, LG_BOTH_EDGES, EINK_BUSY_PIN, -1);
  
   Debug("/***********************************/ \n");
   
  return 0;
}



void gpio_init(void)
{
  Debug("gpio_init\n");

  EINK_RST_PIN     = 17;
  EINK_DC_PIN      = 25;
  EINK_CS_PIN      = 8;
  EINK_PWR_PIN     = 18;  // (? Nothing plugged in here)
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
