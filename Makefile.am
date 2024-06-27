SUBDIRS=Fonts GUI Device
AM_CPPFLAGS=-I Fonts -I GUI -I Device 
LDADD=Fonts/libFonts.a GUI/libGUI.a Device/libDevice.a -llgpio -lm -lproc2
# clean out dead code (in linkers view)
AM_LDFLAGS=-Wl,--gc-sections 

bin_PROGRAMS = clear hello fulldemo showfonts strawman

hello_SOURCES     = hello.c      DEV_Config.c ImageData.c
clear_SOURCES     = clear.c      DEV_Config.c ImageData.c
fulldemo_SOURCES  = fulldemo.c   DEV_Config.c ImageData.c
showfonts_SOURCES = showfonts.c  DEV_Config.c
strawman_SOURCES  = strawman.c   DEV_Config.c gadgets.c


DEBUG = -D DEBUG

# USELIB_RPI = USE_BCM2835_LIB
# USELIB_RPI = USE_WIRINGPI_LIB
USELIB_RPI = USE_LGPIO_LIB
# USELIB_RPI = USE_DEV_LIB

LIB_RPI=-Wl,--gc-sections
LIB_RPI += -llgpio -lm

DEBUG_RPI = -D $(USELIB_RPI) -D RPI

