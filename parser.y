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

  static int gpv_location_print(FILE *yyo, YYLTYPE const * const yylocp);

}

// Emitted on top of the implementation file.
%code requires
{
#include <stdio.h>  // printf.
#include <stdlib.h> // getenv.
#include <string.h> // strcmp.
    
#include "gadgets.h"
#include "actions.h"
#include "parse_utils.h"
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

%union {
	int               ival;
	float             fval;
        enum Eink_colour  colour;
        enum df_units     df_unit;
        enum age_units    age_unit;
        char            * string;
}

%token          T_NL "Newline"
%token          T_SEMI
%token          T_WS

%token<ival>    T_INT
%token<string>  T_STRING
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

%token<colour>  T_COLOUR
%token<df_unit> T_DF_UNIT
%token<age_unit>T_AGE_UNIT
%token<ival>    T_ROTATE

%type<ival>     rotate_arg
%type<ival>     int_arg
%type<string>   string_arg
%type<age_unit> age_arg
%type<df_unit>  df_arg


%start actions

%%

   /* ------------------------ rules -section ---------------------- */

actions:
| actions new_action {verb = "None";}


new_action:  T_NL {}                  /* blank line */
| action T_NL {}        /* tidy up and exit */
| action T_SEMI T_NL {} /* tidy up and exit */
| action ';' T_NL {} /* tidy up and exit */
| action  ',' T_NL {}  /* tidy up and exit */
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
string_arg:          T_STRING
|                ',' T_STRING { $$=strndup($2,64); }
;

rotate_arg:          T_ROTATE
|                ',' T_ROTATE { $$=$2; }
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


init:       init_verb      '('  T_COLOUR  rotate_arg    ')'  { fprintf(stderr, "******INIT (%s;%d)\n",   $3, str_colour($4));} ;
hostname:   hostname_verb  '('  T_INT  int_arg  int_arg ')'  { fprintf(stderr, "******HOSTNAME (%d;%d;%d)\n",   $3, $4, $5);} ;
timestamp:  timestamp_verb '('  T_INT  int_arg  int_arg ')'  { fprintf(stderr, "******TIMESTAMP  (%d;%d;%d)\n", $3, $4, $5);} ;
uptime:     uptime_verb    '('  T_INT  int_arg  int_arg ')'  { fprintf(stderr, "******UPTIME  (%d;%d;%d)\n",    $3, $4, $5);} ;

df:         df_verb        '('  T_INT  int_arg  string_arg   string_arg  df_arg int_arg ')'         { fprintf(stderr, "******DF   (%d;%d;%s;%s;%s;%d)\n"   ,    $3, $4, $5, $6, str_df_units($7),$8);} ;
age:	    age_verb       '('  T_INT  int_arg  int_arg  string_arg   string_arg age_arg int_arg ')'  { fprintf(stderr, "******AGE  (%d;%d;%d;%s;%s;%s;%d)\n",  $3, $4, $5, $6, $7,              str_age($8),$9);} ;
file:       file_verb      '('  T_INT  int_arg  int_arg string_arg   int_arg ')'                      { fprintf(stderr, "******FILE (%d;%d;%d;%s;%d)\n"      ,  $3, $4, $5, $6, $7);} ;
render:     render_verb    '('  ')'							   { fprintf(stderr, "******RENDER ()\n");} ;
sleep:      sleep_verb     '('  T_INT ')'						   { fprintf(stderr, "******SLEEP (%d)\n", $3);} ;
clear:      clear_verb     '('  ')'							   { fprintf(stderr, "******CLEAR ()\n");} ;
loop:       loop_verb      '('  ')'							   { fprintf(stderr, "******LOOP ()\n");} ;

%%

/* ------------------------ user code -section ---------------------- */


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

int main (int argc, const char *argv[])
{
  // Possibly enable parser runtime debugging.
  yydebug = !!getenv ("YYDEBUG");
  // Enable parse traces on option -p.
  for (int i = 1; i < argc; ++i)
    if (strcmp (argv[i], "-p") == 0)
      yydebug = 1;

  int nerrs = 0;
  yyparse (&nerrs);
  // Exit on failure if there were errors.
  return !!nerrs;
}
