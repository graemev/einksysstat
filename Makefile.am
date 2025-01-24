SUBDIRS=Fonts GUI Device
AM_CPPFLAGS=-I Fonts -I GUI -I Device 

einksysstat_LDADD=Fonts/libFonts.a GUI/libGUI.a                  -llgpio -lm -lproc2


# clean out dead code (in linkers view)
AM_LDFLAGS=-Wl,--gc-sections 

bin_PROGRAMS = einksysstat

BUILT_SOURCES=parser.h parser.c lexer.c	\
	einksysstat.1  einksysstat.config.5


einksysstat_SOURCES    = einksysstat.c    DEV_Config.c module.c     device.c gadgets.c actions.c parser.y lexer.l dump_utils.c chain.c



man1_MANS = einksysstat.1
man5_MANS = einksysstat.config.5


BISON = bison
FLEX = flex
#BISONFLAGS= --header --html --grap -Wcounterexamples  -v -d  (HTML fails on Pi4)
BISONFLAGS= --header --grap -Wcounterexamples  -v -d

AM_YFLAGS=-Wno-yacc $(BISONFLAGS)


DEBUG = -D DEBUG

# USELIB_RPI = USE_BCM2835_LIB
# USELIB_RPI = USE_WIRINGPI_LIB
USELIB_RPI = USE_LGPIO_LIB
# USELIB_RPI = USE_DEV_LIB

LIB_RPI=-Wl,--gc-sections
LIB_RPI += -llgpio -lm

DEBUG_RPI = -D $(USELIB_RPI) -D RPI

#
#  I gather MD (markup) is cited for a future version of Autotools?
#

einksysstat.1: einksysstat.md
	pandoc -f markdown -t plain --wrap=none $< -o $@

einksysstat.config.5: einksysstat.config.md
	pandoc -f markdown -t plain --wrap=none $< -o $@
