SUBDIRS=Fonts GUI
AM_CPPFLAGS=-I$(top_srcdir) -I $(top_srcdir)/Fonts -I $(top_srcdir)/GUI

einksysstat_LDADD=Fonts/libFonts.a GUI/libGUI.a                  -llgpio -lm -lproc2


# clean out dead code (in linkers view)
AM_LDFLAGS=-Wl,--gc-sections 

bin_PROGRAMS = einksysstat
dist_bin_scripts  = eink-find-devices


BUILT_SOURCES=parser.h parser.c lexer.c einksysstat.1  einksysstat.config.5



einksysstat_SOURCES    = einksysstat.c   module.c     device.c gadgets.c actions.c parser.y lexer.l dump_utils.c chain.c vcore.c \
	eink_sysstat.h parse_utils.h gadgets.h config.h module.h Fonts/fonts.h Debug.h parser.h device.h dump_utils.h chain.h GUI/GUI_Paint.h actions.h vcore.h


EXTRA_DIST=$(top_srcdir)/einksysstat.md $(top_srcdir)/einksysstat.config.md \
	$(top_srcdir)/einksysstat.conf.disks	$(top_srcdir)/einksysstat.conf.id  $(top_srcdir)/einksysstat.conf.rotate  $(top_srcdir)/einksysstat.conf.sizes  $(top_srcdir)/einksysstat.conf.temps


man1_MANS = einksysstat.1
man5_MANS = einksysstat.config.5

CLEANFILES = $(man1_MANS) $(man5_MANS)


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


