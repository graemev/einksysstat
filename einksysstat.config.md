% einksysstat.conf(5) Version 1.0 | Display system stats on an eInk display

NAME
====

**einksysstat.conf** — Display system stats on an eInk display


DESCRIPTION
===========

This is the format of a file used with the einksysstat(1) command.

Displays a series of system stats on an eInk paper screen attached to e.g. a Raspberry Pi.
This particular implementation is restricted to: 1.54inch e-Paper Module(B) V2 from e.g. Waveshare.
The interface is similar to other boards in the line and other Manufacturers/OEM will no
doubt offer similar boards.

Syntax:
------

The syntax is quite lax. Any text following a hash sign (\#) is discarded (comments)
Where commas (,) and semi-colons (;) are used they may typically be missed out, iff the
meaning is unambiguous. 

Strings may quoted using double quotes (") or single quotes (') single quotes
inside double quotes have no special meaning and vice versa. If you need to
include a single quote in a single quoted string it needs to be escaped with a
backslash (\\)  (and similar for double quotes).

Whitespace can be spaces and or tabs.

The input is a series of **lines** each line  defines an **action** the action 
is described by a **verb** followed by arguments . The verb may be in upper or
lower base (but not mixed) and the parentheses  are optional, thus:

action (arg1, arg2)

ACTION arg1 arg2

Are equivalent. The reason is the style can look like "text" or CPP Macros, the
latter can actually be embedded in the code and avoid the need to parse files at
runtime. HOWTO is out of scope for this man page (but in git use the tag ironman
and look at woodenman.c)

Actions:

**INIT**       (  < colour >,  < rotate > ); 

**HOSTNAME**   (  < xstart >,  < ystart >,  < fsize >  ); 

**TIMESTAMP**  (  < xstart >,  < ystart >,  < fsize >  ); 

**UPTIME**     (  < xstart >,  < ystart >,  < fsize >  ); 

**RENDER**     ();

**SLEEP**      (  < seconds > );

**CLEAR**      ();

**LOOP**       ();

**DF**         (            < ystart >, < fsize >, < device >, < label >  < df >  ,< cutoff > );

**AGE**        (  < xstart >, < ystart >, < fsize >, < filename >   ,< label >  < units >  ,< cutoff > );

**FILE**       (  < xstart >, < ystart >, < fsize >, < filename >   ,< lines > );

**METER**      (  < xstart >, < ystart >, < fsize >, < value >,  < colour > );

**TEXT**       (  < xstart >, < ystart >, < fsize >, < text >,  < colour > );

**IDENTIFY**  ( )

**TEMP**      ( display, <xstart>, <ystart>, <fsize>, <measurement>,    <limit>)
**TEMP**      ( display, <xstart>, <ystart>, <fsize>, <syspathname>,    <limit>)
**THROTTLE**  ( display, <xstart>, <ystart>, <fsize>)
**FAN**       ( display, <xstart>, <ystart>, <fsize>, <syspathname>,    <limit>)

Where:

colour:= is\_black\_on\_grey | is\_red\_on\_grey | is\_black\_on\_red | is\_red\_on\_black

df:=   df\_meg | df\_geg | df\_gig | df\_best | df\_pcent | 
	    meg | geg | gig | best | pcent |
        MB | GB |
        M | G

units:= age\_minutes | age\_hours | age\_days | minutes | hours | days

rotate:= 0 | 90 | 180 | 270

xstart:=column to start drawing 0-199

ystart:=row to start drawing 0-199

fsize:=8 | 12 | 16 | 20 | 24 (font size )

seconds:= < int > 

device:=< string >  (a pathname ... actually to a file, not a /dev type device)

label:=< string > (a short display label)

cutoff:=< int >

lines:=< int > (a small number e.g. 1 2 or 3)

limit:=< int >

syspathname:= a file system path, typically in sys (as suggested by the tool eink-find-devices[.sh])

measurement:= gpu | core

Semantics:
---------

**INIT** 
:	Needed at the start to setup the device.
	
**HOSTNAME**
:	Displays the hostname(1) 
	
**TIMESTAMP**
:	Displays the current time of day as "%Y/%m/%d-%H:%M" see strftime(3)
	
**UPTIME**
	Displays uptime(1) in Days+hours+minutes

**RENDER**
:	Actually "paint" the display (takes quite a long time) Normally there is one of these at the end of each file.
	
**SLEEP**
:	Pause for < seconds > , useful if you want multiple "pages" to display
	
**CLEAR**
:	clears the display. Generally if the device will be stored fro long periods it should be cleared

**LOOP**
:	Not implemented (part of a more complex design where this command runs as a daemon)
	
**DF**
:	Akin to the df(1) command. Output occupies the entire line (so there is no xstart)
	queries the free-space of the file system where the given file (as described by the path < device >)
	< label >  defines the "short name" used to describe it , so /lost&found  and "ROOT" might be
	suitable for < device > and < label > respectively . The display number can be in Megabytes, Gigabytes
	or a percentage. If the free space is below *< cutoff > percent free*, the meter is displayed in red
	otherwise it is all in black.
	
**AGE**
:	Check the age of a file < file > and displays it in Days, Hours or Minutes. If the value is older
	than < cutoff > it will be displayed in red, else black. Typical use is a logfile used on a SUCCESSFUL
	backup, if it is older than < cutoff > days that is a red warning.

**FILE**
:	Displays the first < lines > from a < file > . Typically this file will get updated by other jobs
	e.g. a backup failure message.
	
**METER**
:	This is the naked version of the meter from DF
	
**IDENTIFY**
:	Slightly different to other verbs, it does not need a render verb to display
     it. It puts fixed information on all screens (currently 1) about which display
	 it is. See the -i option to einksysstat(1).
	
**TEXT**
:	Just displays a line of fixed text (might be of use e.g with METER)

**TEMP**
:   Display a temperature, there are 2 types the keyword core or GPU gets the
	temperature from the videocore. Using a path (e.g. in /sys) uses the Linux
	version of temperature. Use the tool eink-find-devices(.sh) to help find the
	correct paths. There could be multiple and you can choose (one in each verb)

**THROTTLE**
:   Diplays the throttled status of the Pi. The simplest case is 00000000 which is normal.
	If throttling is active a T will appear, if it **has** been active a lower case t will appear
	the text will be in red in these cases. If the frequency is being capped F is displayed
	if it **has** been capped lowercase f will be shown. Similar for heat (H/h) or volts (V/v).

**FAN**
:  Displays the fan speed. Use the tool eink-find-devices(.sh) to help find the
   correct paths.

FILES
=====


ENVIRONMENT
===========

EXAMPLE
===========

Note there are several example configuration files shipped with einksysstat distribution:

einksysstat.conf.sizes  -- Shows various font sizes
einksysstat.conf.rotate -- Shows the different screen rotations
einksysstat.conf.id     -- Very similar to the -i option on einksysstat(1)
einksysstat.conf.temps  -- Temperatures and fans
einksysstat.conf.disks  -- Disk usage

The samples attempt to exercise different styles and use all the verbs. It's
unlikely you would use these "as is". You would probably take some ideas from each one:

If you have a filesystem for boot, you may want to ensure there is sufficient free space
(in Mega/Gega Bytes) to build a new initrd.

You may have some critical long term task (like backups) that you need to have successfully
run in the past N days.

There may be various jobs, e.g. mail delivery, that would like to put up a one or two lines of
status (the FILE verb)

You may want to have special configuration files invoked under certain conditions. e.g. you
run something like tripwire and choose to shut the system down when intrusion is suspected
you could put up some information on this display before shutting down. You could read this
information, while the system was shutdown and possibly do a non-standard boot.


<pre>
 HOSTNAME(0,0,12),
 TIMESTAMP(75,0,12),
     
 UPTIME(0,13,16),
 
     
 DF(30 ,24,'/lost+found',               "root", df_best,20),
 DF(55 ,24,"/boot/firmware/config.txt", "SD",   df_best,20),           # line  55
 DF(80 ,24,"/Q/mount/rest3/lost+found", "NV1",  df_best,20),           # line  80
 DF(105,24,"/dev/nvme0n1p3/lost+found", "NV2",  df_best 20),           # line 105

 AGE(0,130, 24,"/boot/firmware/config.txt", "BACKUP1",  age_days,20),  # line 130
 AGE(0,155, 24,"/home/graeme/src",          "BACKUP2",  age_days,180), # line 155
 FILE(0, 180, 8, "/etc/motm", 0),                                      # line 180
 
 RENDER(), # This takes quite a long time (e.g. 30 seconds)
 SLEEP(5),
 
 CLEAR(), # This clears the display (after 5 second wait above) ...probably not what you want to do

</pre>


BUGS
====

See GitHub Issues: https://github.com/graemev/einksysstat/issues

AUTHOR
======

Graeme Vetterlein graeme period Debian   at-sign  vetterlein period com 

SEE ALSO
========

**einksysstat(1)**
