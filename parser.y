/* -*- Mode: bison; tab-width: 4;  -*- */ 
/* parser.y - eink_sysstats parser

/* ------------------------ definitions -section ---------------------- */

%expect 0

// Emitted in the header file, after the definition of YYSTYPE.
%code provides
{
  // Tell Flex the expected prototype of yylex.
#define YY_DECL yytoken_kind_t yylex (YYSTYPE* yylval, YYLTYPE *yylloc, int *nerrs)
  YY_DECL;

  void yyerror (const YYLTYPE *loc, int *nerrs, const char *msg);

  #define YYLOCATION_PRINT(gpvFile, gpvLoc)  gpv_location_print(gpvFile, gpvLoc)

  void gpv_syntax_err(char *msg);
}

// Emitted on top of the implementation file.
%code requires
{
#include <stdio.h>  // printf.
#include <stdlib.h> // getenv.
#include <string.h> // strcmp.

#include "config.h"
  
#include "Debug.h"
#include "gadgets.h"
#include "actions.h"

#include "chain.h"
#include "dump_utils.h"

}

%code
{
  static struct action no_action = {do_noverb, 0, NULL, NULL}; // used only in error handling etc, must not be enchain()ed

  static int    gpv_location_print(FILE *yyo, YYLTYPE const * const yylocp);
  static char * flatten_string(char * in);



}

// Include the header in the implementation rather than duplicating it.
%define api.header.include {"parser.h"}

// Don't share global variables between the scanner and the parser.
%define api.pure full


 // OLDSKOLL - we'll be careful we prexix T_ ourselves
// // To avoid name clashes (e.g., with C's EOF) prefix token definitions
//// with TOK_ (e.g., TOK_EOF).
// %define api.token.prefix {TOK_}

// Going OLDSKOL - we choose our own return types in union
// // %token and %type use genuine types (e.g., "%token <int>").  Let
// // %bison define YYSTYPE as a union of all these types.
///%define api.value.type union

// Generate detailed error messages.
%define parse.error detailed

// with locations.
%locations

// Enable debug traces (see yydebug in main).
%define parse.trace

// Error count, exchanged between main, yyparse and yylex.
%param {int *nerrs}

%code{
    char * verb = NULL;
 }


/*
 * So what's with all these XXXXX, yyyyy, zzzzz ?
 *
 * Well they really define a new verb called XXXXX (don't try to use it, it does nothing)
 * yyyyy defines new arguemnt types (you may not need any) and zzzzz defines one (of possibly many) values it can take.
 *
 * So if you want to define a new verb just search for all the XXXXX (case insensitive) uses. Duplicate that bit of code (often just 1 line)
 * then modify the newly created bit to your new verb name ... taking care to match the case (emacs will do this automatically for e.g. query-replace)
 * If you take care to leave the XXXXX (yyyyy, zzzzz) as the last "one" of wherever it is, thme it wall make adding future verbs much simpler.
 */


%union {
  int               ival;
  float             fval;
  enum Eink_colour  colour;
  enum df_units     df_unit;
  enum age_units    age_unit;
  char            * string;
  struct action   * action;
  enum   temp_type  temp_type;
  enum   zzzzz      zzzzz;
}

%token          T_NL "Newline"
%token          T_SEMI "A semicolon"
%token          T_WS "whitespace"

%token<ival>    T_INT "an Integer"
%token<string>  T_STRING "a quoted String"
%token<string>  T_BAD_STRING


%token          T_INIT
%token          T_HOSTNAME
%token          T_TIMESTAMP
%token          T_UPTIME
%token          T_DF
%token          T_AGE
%token          T_FILE
%token          T_RENDER
%token          T_SLEEP
%token          T_LABEL
%token          T_LOOP
%token          T_CLEAR

%token          T_METER
%token          T_TEXT
%token          T_IDENTIFY

%token          T_TEMP
%token          T_THROTTLE
%token          T_FAN

%token          T_FREQ

%token          T_XXXXX



%token<colour>  T_COLOUR "A colour (like is_black_on_grey)"
%token<df_unit> T_DF_UNIT "df unint like G or M of df_best"
%token<age_unit>T_AGE_UNIT

%token<temp_type>T_TEMP_TYPE "core/gpu/cpu/arm"
%token<zzzzz>	T_ZZZZZ

%type<ival>     rotate_arg
%type<ival>     int_arg
%type<string>   string_arg
%type<age_unit> age_arg
%type<df_unit>  df_arg
%type<colour>   colour_arg
%type<temp_type> temp_arg


%type<action>   action
%type<action>   new_action

%type<action>   init
%type<action>   hostname
%type<action>   timestamp
%type<action>   uptime

%type<action>   df
%type<action>   age
%type<action>   file
%type<action>   render
%type<action>   sleep
%type<action>   clear
%type<action>   loop

%type<action>   meter
%type<action>   text
%type<action>   identify

%type<action>   temp
%type<action>   throttle
%type<action>   fan

%type<action>   freq

%type<action>   xxxxx




%start actions

%%

   /* ------------------------ rules -section ---------------------- */

actions:
| actions new_action {if ($2  != &no_action) enchain($2); verb = "None"; }


new_action:  T_NL               {$$=&no_action; }                  /* blank line */
|            action T_NL        {$$=$1;}        /* tidy up and exit */
|            action T_SEMI T_NL {$$=$1;} /* tidy up and exit */
|            action ';' T_NL    {$$=$1;} /* tidy up and exit */
|            action  ',' T_NL   {$$=$1;}  /* tidy up and exit */
|            error              { $$=&no_action; }
;

 
action: init
| hostname 
| timestamp
| uptime
| df
| age
| file
| render
| sleep
| loop
| clear
| meter
| text
| identify
| temp
| throttle
| fan
| freq
| xxxxx

;

int_arg:          T_INT
|             ',' T_INT { $$=$2; }
;

df_arg:              T_DF_UNIT
|                ',' T_DF_UNIT { $$=$2; }
;

age_arg:             T_AGE_UNIT
|                ',' T_AGE_UNIT { $$=$2; }
;
string_arg:          T_STRING {$$=strndup(flatten_string($1),64);}
|                ',' T_STRING {$$=strndup(flatten_string($2),64);}
;

rotate_arg:          T_INT
|                ',' T_INT { $$=$2; }
;
colour_arg:          T_COLOUR
|                ',' T_COLOUR { $$=$2; }
;

temp_arg:            T_TEMP_TYPE
|                ',' T_TEMP_TYPE { $$=$2; }
;

    

init_verb:       T_INIT        { verb = "init";};
hostname_verb:   T_HOSTNAME    { verb = "hostname";};
timestamp_verb:  T_TIMESTAMP   { verb = "timestamp";};
uptime_verb:     T_UPTIME      { verb = "uptime";};
df_verb:         T_DF          { verb = "df";};
age_verb:        T_AGE         { verb = "age";};
file_verb:       T_FILE        { verb = "file";};
render_verb:     T_RENDER      { verb = "render";};
sleep_verb:      T_SLEEP       { verb = "sleep";};
clear_verb:      T_CLEAR       { verb = "clear";};
loop_verb:       T_LOOP        { verb = "loop";};
meter_verb:      T_METER       { verb = "meter";};
text_verb:       T_TEXT        { verb = "text";};
identify_verb:   T_IDENTIFY    { verb = "identify";};
temp_verb:       T_TEMP        { verb = "temp";};
throttle_verb:   T_THROTTLE    { verb = "throttle";};
fan_verb:        T_FAN         { verb = "fan";};
freq_verb:       T_FREQ        { verb = "freq";};
xxxxx_verb:      T_XXXXX       { verb = "xxxxx";};

/*
 *  INIT      ( display, colour, rotation)
 *  HOSTNAME  ( display, rol, col, font)
 *  TIMESTAMP ( display, rol, col, font))
 *  UPTIME    ( display, rol, col, font))
 *
 *  RENDER    ( display)
 *
 *  SLEEP     ( display, seconds)
 *  CLEAR     ( display)
 *  LOOP      ( display)
 *
 *  DF        ( display, col, font, pathname, label, units, limit)
 *  AGE       ( display, row, col, font, pathname, label, units, limit)
 *  FILE      ( display, row, col, font, pathname, lines)
 *  TEXT      ( display, row, col, font, text), colour
 *  METER     ( display, row, col, font, value, colour)
 *  IDENTIFY  ( )
 *
 *  TEMP      ( display, row, col, font, measurement, limit)
 *  TEMP      ( display, row, col, font, pathname, limit)
 *  THROTTLE  ( display, row, col, font)
 *  FAN       ( display, row, col, font, pathname, limit)
 *  FREQ      ( display, row, col, font, measurement, limit)

 */




init:       init_verb      '('  T_INT colour_arg rotate_arg     ')'
{
  /* doing this inline is ugly, sadly errs (&yylloc?) are simply stack allocated locals, so we can't access them in another function */
  if ($5%90 !=0)
	{
	yyerror (&yylloc, nerrs, "rotation must be multiple of 90");
	$5=0;
	}
  $$=new_action_init($3, $4, $5);       Debug("******INIT     (%d,%s;%d)\n",       $3, str_colour($4), $5);
} ;
hostname:   hostname_verb  '('  T_INT  int_arg int_arg  int_arg ')'  { $$=new_action_hostname($3,$4,$5,$6);  Debug("******HOSTNAME (%d;%d;%d;%d)\n",    $3, $4, $5, $6); } ;
timestamp:  timestamp_verb '('  T_INT  int_arg int_arg  int_arg ')'  { $$=new_action_timestamp($3,$4,$5,$6); Debug("******TIMESTAMP(%d;%d;%d;%d)\n",    $3, $4, $5, $6); } ;
uptime:     uptime_verb    '('  T_INT  int_arg int_arg  int_arg ')'  { $$=new_action_uptime($3,$4,$5,$6);    Debug("******UPTIME   (%d;%d;%d;%d)\n",    $3, $4, $5, $6); } ;

render:     render_verb    '(' T_INT ')'							 { $$=new_action_render($3);             Debug("******RENDER   (%d)\n",             $3);     } ;
sleep:      sleep_verb     '(' T_INT int_arg ')'					 { $$=new_action_sleep($3,$4);           Debug("******SLEEP    (%d,%d)\n",          $3, $4); } ;
clear:      clear_verb     '(' T_INT ')'							 { $$=new_action_clear($3);              Debug("******CLEAR    (%d)\n",             $3);     } ;
loop:       loop_verb      '(' T_INT ')'							 { $$=new_action_loop($3);               Debug("******LOOP     (%d)\n",             $3) ;    } ;


df:         df_verb        '('  T_INT  int_arg  int_arg string_arg string_arg df_arg     int_arg         ')' { $$=new_action_df($3,$4,$5,$6,$7,$8,$9);     Debug("******DF      (%d;%d;%d;%s;%s;%s;%d)\n"   ,  $3, $4, $5, $6, $7, str_df_units($8),$9);} ;
age:	    age_verb       '('  T_INT  int_arg  int_arg int_arg    string_arg string_arg age_arg int_arg ')' { $$=new_action_age($3,$4,$5,$6,$7,$8,$9,$10);Debug("******AGE     (%d;%d;%d;%d;%s;%s;%s;%d)\n",  $3, $4, $5, $6, $7, $8, str_age($9),$10); } ;
file:       file_verb      '('  T_INT  int_arg  int_arg int_arg    string_arg int_arg                    ')' { $$=new_action_file($3,$4,$5,$6,$7,$8);      Debug("******FILE    (%d;%d;%d;%d;%s;%d)\n"      ,  $3, $4, $5, $6, $7,$8);                 } ;

meter:      meter_verb     '('  T_INT  int_arg  int_arg int_arg    int_arg    colour_arg                 ')' { $$=new_action_meter($3,$4,$5,$6,$7,$8);     Debug("******METER   (%d;%d;%d;%d;%d;%s)\n"      ,  $3, $4, $5, $6,$7, str_colour($8));} ;
text:       text_verb      '('  T_INT  int_arg  int_arg int_arg    string_arg colour_arg                 ')' { $$=new_action_text($3,$4,$5,$6,$7,$8);      Debug("******TEXT    (%d;%d;%d;%d;%s;%s)\n"      ,  $3, $4, $5, $6,$7, str_colour($8));} ;

identify:   identify_verb  '(' ')'                                                                           { $$=new_action_identify();                   Debug("******IDENTIFY    ()\n");};

temp:       temp_verb      '(' T_INT  int_arg int_arg  int_arg string_arg int_arg ')'                        { $$=new_action_linux_temp($3,$4,$5,$6,$7,$8);Debug("******TEMP    (%d;%d;%d;%d;%s;%d)\n"      ,  $3, $4, $5, $6, $7,$8);} ;
temp:       temp_verb      '(' T_INT  int_arg int_arg  int_arg temp_arg int_arg ')'                          { $$=new_action_vcore_temp($3,$4,$5,$6,$7,$8);Debug("******TEMP    (%d;%d;%d;%d;%s;%d)\n"      ,  $3, $4, $5, $6, str_temp($7),$8);} ;
throttle:   throttle_verb  '(' T_INT  int_arg int_arg  int_arg ')'                                           { $$=new_action_throttle($3,$4,$5,$6);        Debug("******THROTTLE   (%d;%d;%d;%d)\n"		    ,  $3, $4, $5, $6); } ;
fan:        fan_verb       '(' T_INT  int_arg int_arg  int_arg string_arg int_arg ')'                        { $$=new_action_fan($3,$4,$5,$6,$7,$8);       Debug("******FAN    (%d;%d;%d;%d;%s;%d)\n"       ,  $3, $4, $5, $6, $7,$8);} ;

freq:       freq_verb      '(' T_INT  int_arg int_arg  int_arg temp_arg int_arg ')'                          { $$=new_action_freq($3,$4,$5,$6,$7,$8);Debug("******FREQ    (%d;%d;%d;%d;%s;%d)\n"      ,  $3, $4, $5, $6, str_temp($7),$8);} ;

xxxxx:    xxxxx_verb       '(' ')'                                 { $$=new_action_xxxxx();                  Debug("******XXXXX    ()\n");};



%%

			/* ------------------------ user code -section ---------------------- */

			/* WARNING, NOT REENTRANT!
			 *
			 * If the lexer eats a quited string, e.g in  text(0,155, 24,"IRONMAN\"text",  is_red_on_grey)
			 * The ne actual sting it will pass up is "IRONMAN\"text" ...it contains double quotes at either
			 * end and an actual backslash (and aother quote) inside the string (it is 15 bytes long)
			 *
			 * What we would like to pass into the eink tool is IRONMAN"text ..(12 bytes). Some other examples
			 *
			 *  "foo'&'bar"  ---> foo'&'bar  (single quotes are not special inside double quotes)
			 *  'bar"&\ bat' ---> bar"& bat  (double quote is not special inside single, escaped space, is just space)
			 *  "o'in\" qu'h" --> o'in" qu'h -->  (single quote need not be escaped or balanced, inner double qote needs escape
			 *
			 *
			 */

static char * flatten_string(char * in)
{
  static char  out[256];

  char  special;
  char  *pin;
  char  *pout;
  char  c;
  
  special=in[0];

  if (special == '\"' || special == '\'')
	pin=in+1;   // As expected, starts with single or double quote
  else
	{
	  special='\0';
	  pin=in;	// Naked string, no quotes, not what we expect
	}
  pout=out;

  
  while ((c=*pin++) && (pout<(out+255))) // not at final NULL

	{
	  if (c == '\\')
		{
		c=*pin++; // Potentially eats the final null (in a naked string ...unclear what we should do here??)

		switch (c)  // These change to their escaped flavour (and \x is just x , and \\ is just \ )
		  {
		  case '\0':
			pin--;   // step back again (we won't bypass a NULL due to an escape)
			break;
		  case 'n':
			c='\n';
			break;
		  case 'r':
			c='\r';
			break;
		  case '0':
			c='\0';
			break;
		  }
		}
	  else if (c == special)
		{
		  *pout++='\0';
		  break;
		}
	  *pout++ = c;  
	}
  *pout = '\0';

  return out;
}


// Epilogue (C code).


YY_ATTRIBUTE_UNUSED
static int
gpv_location_print(FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;

  int  multicol = (0 <= end_col && yylocp->first_column < end_col);  // bool really
  
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "On line:%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
		res += YYFPRINTF (yyo, " @ column%c:%d", ((multicol)?'s':' '), yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
	  res += YYFPRINTF (yyo, "up to line:%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ",column:%d", end_col);
        }
      else if (multicol)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}


void yyerror (const YYLTYPE *loc, int *nerrs, const char *msg)
{
  YYLOCATION_PRINT (stderr, loc);
  fprintf (stderr, ":%s while processiing action '%s'\n", msg, (verb?verb:"None"));
  ++*nerrs;
}

