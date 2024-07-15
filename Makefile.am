SUBDIRS=Fonts GUI Device
AM_CPPFLAGS=-I Fonts -I GUI -I Device 
paperman_LDADD=Fonts/libFonts.a GUI/libGUI.a Device/libDevice.a -llgpio -lm -lproc2
strawman_LDADD=Fonts/libFonts.a GUI/libGUI.a Device/libDevice.a -llgpio -lm -lproc2
woodenman_LDADD=Fonts/libFonts.a GUI/libGUI.a                   -llgpio -lm -lproc2
parse_LDADD=Fonts/libFonts.a GUI/libGUI.a                   -llgpio -lm -lproc2

# clean out dead code (in linkers view)
AM_LDFLAGS=-Wl,--gc-sections 

bin_PROGRAMS = strawman woodenman paperman parse

paperman_SOURCES   = paperman.c   DEV_Config.c old_gadgets.c
strawman_SOURCES   = strawman.c   DEV_Config.c old_gadgets.c
woodenman_SOURCES  = woodenman.c  module.c     device.c gadgets.c actions.c
parse_SOURCES      = parse.c      module.c     device.c gadgets.c actions.c


DEBUG = -D DEBUG

# USELIB_RPI = USE_BCM2835_LIB
# USELIB_RPI = USE_WIRINGPI_LIB
USELIB_RPI = USE_LGPIO_LIB
# USELIB_RPI = USE_DEV_LIB

LIB_RPI=-Wl,--gc-sections
LIB_RPI += -llgpio -lm

DEBUG_RPI = -D $(USELIB_RPI) -D RPI

