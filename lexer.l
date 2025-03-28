/* ------------------------ definitions -section ---------------------- */

/*
 * lexer.l - Parse the eink config
 *
 * Example:
 *
 * HOSTNAME(0,0,12),
 * TIMESTAMP(75,0,12),
 *     
 * UPTIME(0,13,16),
 * 
 *     
 * DF(30 ,24,"/lost+found",               "root", df_best,20),
 * DF(55 ,24,"/boot/firmware/config.txt", "SD",   df_best,20),           # line  55
 * DF(80 ,24,"/Q/mount/rest3/lost+found", "NV1",  df_best,20),           # line  80
 * DF(105,24,"/dev/nvme0n1p3/lost+found", "NV2",  df_best,20),           # line 105
 * 
 *     
 * AGE(0,130, 24,"/boot/firmware/config.txt", "BACKUP1",  age_days,20),  # line 130
 * AGE(0,155, 24,"/home/graeme/src",          "BACKUP2",  age_days,180), # line 155
 * FILE(0, 180, 8, "/etc/motm", 0),                                      # line 180
 * 
 * RENDER(),
 * SLEEP(5),
 * 
 * CLEAR(),
 *
 *
 */



%option nodefault noinput nounput noyywrap

%{
#include <errno.h>  /* errno, ERANGE */
#include <limits.h> /* INT_MIN */
#include <stdlib.h> /* strtol */

#include "parser.h"

  // Each time a rule is matched, advance the end cursor/position.
#define YY_USER_ACTION                          \
  yylloc->last_column += (int) yyleng;

  // Move the first position onto the last.
#define LOCATION_STEP()                         \
  do {                                          \
    yylloc->first_line = yylloc->last_line;     \
    yylloc->first_column = yylloc->last_column; \
  } while (0)
%}



%%
%{
  // Each time yylex is called, move the head position to the end one.
  LOCATION_STEP ();
%}


 /* ------------------------ rules -section ---------------------- */

 /*
  * Note we don'r match "small things" like ',' or '(' of ')' these are just passed to parser via match of last resort (MoLR)
  */


";"				{return T_SEMI;}

\n				{yylloc->last_line++; yylloc->last_column = 1; return T_NL; }


 /*  [ \t]				; //  Silently eat all whitespace  (not newline '\n' ) */

 /* Ignore white spaces. ...this is VOODOO ... not the whitespace the LOCATION_STEP macro*/
[ \t]+   LOCATION_STEP (); continue;

#.*                             ;  // Ignore comments
[0-9]+				{yylval->ival = atoi(yytext); return T_INT;}

\"([^\"\\]|\\.)*\"		{yylval->string=yytext; return T_STRING; }        // Double quoted string with escapes
\'([^\'\\]|\\.)*\'		{yylval->string=yytext; return T_STRING; }        // Single quoted string with escapes
\"([^\"\\]|\\.)*		{yylval->string=yytext; return T_BAD_STRING; }    // Double quoted string with escapes (UNTERMINATED)
\'([^\'\\]|\\.)*		{yylval->string=yytext; return T_BAD_STRING; }    // Single quoted string with escapes (UNTERMINATED)



"INIT"				{return T_INIT;}
"init"				{return T_INIT;}

"HOSTNAME"			{return T_HOSTNAME;}
"hostname"			{return T_HOSTNAME;}

"TIMESTAMP"			{return T_TIMESTAMP;}
"timestamp"			{return T_TIMESTAMP;}


"UPTIME"			{return T_UPTIME;}
"uptime"			{return T_UPTIME;}

"DF"				{return T_DF;}
"df"				{return T_DF;}

"AGE"				{return T_AGE;}
"age"				{return T_AGE;}

"FILE"				{return T_FILE;}
"file"				{return T_FILE;}

"RENDER"			{return T_RENDER;}
"render"			{return T_RENDER;}

"SLEEP"				{return T_SLEEP;}
"sleep"				{return T_SLEEP;}

"CLEAR"				{return T_CLEAR;}
"clear"				{return T_CLEAR;}

"LABEL"				{return T_LABEL;}
"label"				{return T_LABEL;}

"LOOP"				{return T_LOOP;}
"loop"				{return T_LOOP;}

"METER"				{return T_METER;}
"meter"				{return T_METER;}

"TEXT"				{return T_TEXT;}
"text"				{return T_TEXT;}

"IDENTIFY"			{return T_IDENTIFY;}
"identify"			{return T_IDENTIFY;}

"TEMP"				{return T_TEMP;}
"temp"				{return T_TEMP;}
"TEMPERATURE"			{return T_TEMP;}
"temperature"			{return T_TEMP;}

"THROTTLE"			{return T_THROTTLE;}
"throttle"			{return T_THROTTLE;}

"FAN"				{return T_FAN;}
"fan"				{return T_FAN;}

"FREQ"				{return T_FREQ;}
"freq"				{return T_FREQ;}

"XXXXX"			{return T_XXXXX;}
"xxxxx"			{return T_XXXXX;}





"is_black_on_grey"		{yylval->colour = is_black_on_grey ; return T_COLOUR;}
"is_red_on_grey"		{yylval->colour = is_red_on_grey   ; return T_COLOUR;}
"is_black_on_red"		{yylval->colour = is_black_on_red  ; return T_COLOUR;}
"is_red_on_black"		{yylval->colour = is_red_on_black  ; return T_COLOUR;}

"df_meg"			{yylval->df_unit=df_meg   ; return T_DF_UNIT;}
"df_geg"			{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"df_gig"			{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"df_ter"			{yylval->df_unit=df_tera  ; return T_DF_UNIT;}
"df_best"			{yylval->df_unit=df_best  ; return T_DF_UNIT;}
"df_pcent"			{yylval->df_unit=df_pcent ; return T_DF_UNIT;}

"meg"				{yylval->df_unit=df_meg   ; return T_DF_UNIT;}
"geg"				{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"gig"				{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"ter"				{yylval->df_unit=df_tera  ; return T_DF_UNIT;}
"best"				{yylval->df_unit=df_best  ; return T_DF_UNIT;}
"pcent"				{yylval->df_unit=df_pcent ; return T_DF_UNIT;}

"MB"				{yylval->df_unit=df_meg   ; return T_DF_UNIT;}
"GB"				{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"TB"				{yylval->df_unit=df_tera  ; return T_DF_UNIT;}

"M"				{yylval->df_unit=df_meg   ; return T_DF_UNIT;}
"G"				{yylval->df_unit=df_geg   ; return T_DF_UNIT;}
"T"				{yylval->df_unit=df_tera  ; return T_DF_UNIT;}


"age_minutes"			{yylval->age_unit=age_minutes ; return T_AGE_UNIT;}
"age_hours"			{yylval->age_unit=age_hours   ; return T_AGE_UNIT;}
"age_days"			{yylval->age_unit=age_days    ; return T_AGE_UNIT;}

"minutes"			{yylval->age_unit=age_minutes ; return T_AGE_UNIT;}
"hours"				{yylval->age_unit=age_hours   ; return T_AGE_UNIT;}
"days"				{yylval->age_unit=age_days    ; return T_AGE_UNIT;}

"cpu"				{yylval->temp_type=temp_cpu       ; return T_TEMP_TYPE;}
"gpu"				{yylval->temp_type=temp_gpu       ; return T_TEMP_TYPE;}
"core"				{yylval->temp_type=temp_gpu       ; return T_TEMP_TYPE;}
"arm"				{yylval->temp_type=temp_arm       ; return T_TEMP_TYPE;}

"yyyyy"				{yylval->zzzzz=yyyyy   ; return T_ZZZZZ;}





.                               { return *yytext; }   /*  MoLR */

%%

/* ------------------------ user code -section ---------------------- */
/* Epilogue (C code). */
