% einksysstat(1) Version 1.0 | Display system stats on an eInk display

NAME
====

**einksysstat** — Display system stats on an eInk display

SYNOPSIS
========

**einksysstat** \[options] \[filename]


DESCRIPTION
===========

Displays a series of system stats on an eInk paper screen attached to e.g. a Raspberry Pi.
This particular implementation is restricted to: 1.54inch e-Paper Module(B) V2 from e.g. Waveshare.
The interface is similar to other boards in the line and other Manufacturers/OEM will no
doubt offer similar boards.

Options
-------

-V, --version

:   Prints the current version number and exits.

-h, --help

:   Prints brief usage information.

-v, --verbose

:   Make output more verbose. (may be repeated)

-i, --identify

:   Identify each display (currently only one) displaying configuration info on display.
    This can be used with or without a script file. Without a file it simply displays the info
	with a script file it displays the info twice. Before and after running the script. This has implications
	if the script defines more displays (currently not implemented) they appear in the latter output. After
	the first display of info the display is not cleared, so if the script does not clear the display
	it will overwrite the info display.

-d, --debug

:   Developer option.  (may be repeated)


The filename names a file which defines a series of verbs that define what is displayed.
The format is given in einksysstat.config(5)



FILES
=====


ENVIRONMENT
===========


BUGS
====

See GitHub Issues: https://github.com/graemev/einksysstat/issues

AUTHOR
======

Graeme Vetterlein graeme period Debian   at-sign  vetterlein period com 

SEE ALSO
========

**einksysstat.config(5)**
