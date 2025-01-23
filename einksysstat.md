% einksysstat(1) Version 1.0 | Display system stats on an eInk display

NAME
====

**einksysstat** — Display system stats on an eInk display

SYNOPSIS
========

**einksysstat** \[options] \[filename]


DESCRIPTION
===========

Displays a series of system stats on an eInk paper screen attached to e.g. a Rasberry Pi.
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

See GitHub Issues: https://github.com/graemev/TBD

AUTHOR
======

Graeme Vetterlein graeme period debian   at-sign  vetterlein period com 

SEE ALSO
========

**einksysstat.config(5)**
