/* -*- Mode: c; tab-width: 2;c-basic-offset: 2; c-default-style: "gnu" -*- */

/* This file is part of Einkstat. */

/*     Einkstat is free software: you can redistribute it and/or modify it under
       the terms of the GNU General Public License as published by the Free
       Software Foundation, either version 3 of the License, or (at your option)
       any later version. */

/*     Einkstat is distributed in the hope that it will be useful, but WITHOUT ANY
       WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
       FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
       details. */

/*     You should have received a copy of the GNU General Public License along
       with Einkstat. If not, see <https://www.gnu.org/licenses/>. */

/*     Note included within Einkstat is code covered by a Public Domain (PD)
       licence (which is indicated within those files) these sections
       are subject to that PD licence. If you wish to use those portions
       under "Public Domain" terms it is proably easier to use the original
       found at e.g. https://github.com/waveshareteam/e-Paper */



/*
 * parse_utils.c: Parse the config file
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


#include "parse_utils.h"



struct lookup
{
  char                            *name;
  enum       token {zero=0, one=1} token;
};


/*
 *
 * Really we don't want alpha we want "national" (includes _ # $ @)
 */

/* used to tokenise keywords */
static int parse_word(FILE *stream, struct lookup lookup[])
{
  int            c;

  int  i=0;
  char buffer[64];  // No words bigger than 63
	
  while ((c=fgetc(stream)) != EOF && (isalpha(c) || strchr("_-$#@" ,c ) != NULL) && i <= 63)
    {
      buffer[i++]=c;
    }

  ungetc(c, stream);
  
  buffer[i]='\0';

  for (i=0; lookup[i].name ;++i)      // name is NULL in the last entry
    if (strcasecmp(lookup[i].name, buffer) == 0)
	break;

  return i;
}

/* used to tokenise numbers */
int	parse_int(FILE *stream)
{
  int c;
  int  i=0;
  char buffer[65];  //  No words bigger than 63 .. TBD compiler BUG with 64 ?

  while ((c=fgetc(stream)) != EOF && isdigit(c)  && i <= 63)
    {
      buffer[i++]=c;
    }

  ungetc(c, stream);

  buffer[i]='\0';

  if (i==0)
    return -1;
  else
    return (atoi(buffer));
}



struct
{
  char          *name;
  enum df_units  unit;
} df_lookup[] =
  {
    {"df_meg"  , df_meg},
    {"df_geg"  , df_geg},
    {"df_best" , df_best},
    {"df_pcent", df_pcent},

    {"meg"     , df_meg},
    {"geg"     , df_geg},
    {"best"    , df_best},
    {"pcent"   , df_pcent},

    {"M"        , df_meg},
    {"G"        , df_geg},
    {"percent"  , df_pcent},


    {"megabyte" , df_meg},
    {"gigabyte" , df_geg},
    
    {NULL      , df_none},
  };

 enum df_units parse_df_units(FILE *stream)
 {
   int i;

   i = parse_word(stream, (struct lookup*) df_lookup);

   return (df_lookup[i].unit);
 }







struct
{
  char             *name;
  enum  Eink_colour colour;
} colour_lookup[] =
  {
    {"is_black_on_grey", is_black_on_grey},
    {"is_red_on_grey"  , is_red_on_grey},
    {"is_black_on_red" , is_black_on_red},
    {"is_red_on_black" , is_red_on_black},

    {"black_on_grey"   , is_black_on_grey},
    {"red_on_grey"     , is_red_on_grey},
    {"black_on_red"    , is_black_on_red},
    {"red_on_black"    , is_red_on_black},

    {"black"           , is_black_on_grey},
    {"red"             , is_red_on_grey},
    {NULL	       , is_no_colour},
  };
    

 enum Eink_colour parse_colour(FILE *stream)
 {
   int i;

   i = parse_word(stream, (struct lookup*) colour_lookup);

   return (colour_lookup[i].colour);
 }





struct
{
  char             *name;
  enum  age_units   age;
} age_lookup[] =
  {
    {"age_none"   , age_none},

    {"age_minutes", age_minutes},
    {"age_hours"  , age_hours},
    {"age_days"   , age_days},

    {"minutes"    , age_minutes},
    {"hours"      , age_hours},
    {"days"       , age_days},
    
    {NULL	  , age_none},
  };
    

 enum age_units parse_age(FILE *stream)
 {
   int i;

   i = parse_word(stream, (struct lookup*) age_lookup);

   return (age_lookup[i].age);
 }



struct
{
  char      *name;
  enum verb verb;
} verb_lookup[] =
  {
    {"none"          , do_none},
    {"label"         , do_label},
    {"loop"          , do_loop},
    {"init_module"   , do_init_module},
    {"init_display"  , do_init_display},
    {"init_image"    , do_init_image},
    {"release_image" , do_release_image},
    {"release_module", do_release_module},
    {"render"        , do_render},
    {"clear"         , do_clear},
    {"text"          , do_text},
    {"hostname"      , do_hostname},
    {"timestamp"     , do_timestamp},
    {"uptime"        , do_uptime},
    {"meter"         , do_meter},
    {"df"            , do_df},
    {"age"           , do_age},
    {"file"          , do_file},
    {"sleep"         , do_sleep},
    {NULL             ,	0},		// do_noverb  This value returned on no match
  };
    

 
enum verb parse_verb(FILE *stream)
{
   int i;

   i = parse_word(stream, (struct lookup*) verb_lookup);

   return (verb_lookup[i].verb);
}



// This guy POPS when he sees an unguarded (matching) quote
int eat_string(FILE * stream, char **buffer, char quote,int limit) // The special character is quote ...(single or double quote)
{
  char  *p   = *buffer;
  int    c;
  int    len=0;

  while ((c=fgetc(stream)) != EOF && limit>1) // 1 for the '\0'
    {
      if ( c == '\\')
	c=getc(stream);           // so now c might be a quote, but we won't check (DANGER it could be a null, EOF or \n)
      else if (c == quote)
	break;
      *p++ = c;        // Just stash it
      --limit;
      ++len;
    }

  *p = '\0';  // Don't move forward (our caller may overwrite this NULL, if they carry on....also don't inc the length (same reason)
  *buffer    = p;  // So the variable in our caller points at the NULL

  return len;
}

/*
 * Parse text is a little different, it might just be:
 *
 *  foo .... but it could be
 *  "foo bar" or 'foo bar' ... but we have to allow for
 *  "foo \"bar\"" "foo 'bar'" 'foo \'bar\'' of 'foo "bar"'
 *
 */

// This guy PUSHES when he sees an unguarded quote 
int parse_text(FILE *stream, char * string, int limit)
{
  int   i=0;
  int   len=0;;
  int   c;
  char *p;

  p = string;
  
  while ((c=fgetc(stream)) != EOF && limit>1) 
    {
      if (c=='"' || c=='\'')
	{
	  len+=eat_string(stream, &p, c, limit);
	}
      else if (isalnum(c))  // Just store it
	{
	  *p++ = c;
	  --limit;
	  ++len;
	}
      else
	break;
    }

  ungetc(c, stream);
  
  *p = '\0';
  
  return len;
}


static int lineno=1;


void this_line(FILE *stream)
{
  int c;
  
  while ((c=fgetc(stream)) != EOF && c != '\n') ;
  ungetc(c, stream);  // Don' eat the newline
}

bool next_line(FILE *stream)
{
  int c;
  bool hit_eof = false;
  
  while ((c=fgetc(stream)) != EOF && c != '\n') ;
  ++lineno;

  if (c == EOF)
    hit_eof = true;

  return (hit_eof);
}

bool  skip_ws(FILE * stream)  // Will gobble through newline
{
  int c;

  bool hit_eof = false;

  char skip[] = " \t\n\r";
  
  while ((c=fgetc(stream)) != EOF)
    {
      if ( c == '\n')
	++lineno;
      if (c == '#')  // means we skip the rest of line
	hit_eof=next_line(stream);
      else if (strchr(skip, c) == NULL)
	break;
    }

  if (c == EOF)
      hit_eof = true;
  else
    ungetc(c, stream);  // can never break with a '#' 

  return (hit_eof);
}



bool  gobble_chars(FILE * stream, char skip[])  // Will gobble through newline
{
  int c;
  bool hit_eof = false;

  while ((c=fgetc(stream)) != EOF)
    {
      if (c == '#')  // means we skip the rest of line
	this_line(stream);
      else if (strchr(skip, c) == NULL)
	break;
    }
  if (c == EOF)
    {
      fprintf(stderr, "hit eof\n");
      hit_eof = true;
    }
  ungetc(c, stream);  // can never break with a '#' (may push back a newline or EOF)

  return hit_eof;
}

void syntax_error(FILE * stream, char * attempt)
{
  char rest[64];

  fgets(rest, sizeof(rest), stream);
  
  fprintf(stderr, "Syntax error on line %d, attempting to parse %s, at text starting %s\n",
	  lineno,
	  attempt,
	  rest);
  exit(1);
}

/* only ever reports one */
void validate (int x, int y, int fsize, int rotation)
{
  char * reason = NULL;
  
  if ( x > 200 || x < 0)
    reason = "X value must be in range 0-200";
  
  if ( y > 200 || y < 0)
    reason = "Y value must be in range 0-200";

  if ( fsize !=8 && fsize !=12 && fsize !=16 && fsize !=20 && fsize !=24)
    reason = "font size value must be 8,12,16,20 or 24";

  if ( rotation !=0 && rotation !=90 && rotation !=180 && rotation !=240)
    reason = "rotation value must be 0,90,180 or 270";

  if (reason)
    fprintf(stderr, "Semantic error on line %d, %s\n",
	    lineno,
	    reason);
}

// We've seen the word hostname
struct action * parse_hostname(FILE *stream)
{
  int x;
  int y;
  int fsize;
  int c;
  struct action * p;
  
  gobble_chars(stream, " (\t");

  if ((x = parse_int(stream)) < 0)
    syntax_error(stream, "HOSTNAME, x value");

  gobble_chars(stream, " ,\t");
  
  if ((y = parse_int(stream)) < 0)
    syntax_error(stream, "HOSTNAME y value");


  gobble_chars(stream, " ,\t");
  
  if ((fsize = parse_int(stream)) < 0)
    syntax_error(stream, "HOSTNAME fsize");

  gobble_chars(stream, " ),;\t");   // Allow spurious ',' and ';' in case these were copied from code

  if ((c=getc(stream)) == '\n' || c == EOF)  
    ungetc(c, stream);  // Dont' eat the newline
  else
    {
      ungetc(c, stream);
      syntax_error(stream, "HOSTNAME spurious text following a valid HOSTNAME definition");
    }

  validate(x,y,fsize, 0);

  p = (struct action*) malloc(sizeof(struct action));

  p->next                = NULL;
  p->prev                = NULL;

  p->verb                = do_hostname;
  p->args.hostname.xstart= x;
  p->args.hostname.ystart= y;
  p->args.hostname.fsize = fsize;

  return p;
}  // parse_hostname




// We've seen the word timestamp
struct action * parse_timestamp(FILE *stream)
{
  int x;
  int y;
  int fsize;
  int c;
  struct action * p;
  
  gobble_chars(stream, " (\t");

  if ((x = parse_int(stream)) < 0)
    syntax_error(stream, "TIMESTAMP, x value");

  gobble_chars(stream, " ,\t");
  
  if ((y = parse_int(stream)) < 0)
    syntax_error(stream, "TIMESTAMP y value");


  gobble_chars(stream, " ,\t");
  
  if ((fsize = parse_int(stream)) < 0)
    syntax_error(stream, "TIMESTAMP fsize");

  gobble_chars(stream, " ),;\t");   // Allow spurious ',' and ';' in case these were copied from code
  
  if ((c=getc(stream)) == '\n' || c == EOF)    
    ungetc(c, stream);  // Dont' eat the newline
  else
    {
      ungetc(c, stream);
      syntax_error(stream, "TIMESTAMP spurious text following a valid TIMESTAMP definition");
    }

  validate(x,y,fsize, 0);

  p = (struct action*) malloc(sizeof(struct action));

  p->next                = NULL;
  p->prev                = NULL;

  p->verb                = do_timestamp;
  p->args.timestamp.xstart= x;
  p->args.timestamp.ystart= y;
  p->args.timestamp.fsize = fsize;

  return p;
}


// We've seen the word df
struct action * parse_df(FILE *stream)
{
  int x;
  int y;
  int fsize;
  int c;
  int cutoff;
  enum df_units units;
  char device[65];
  char label[65];

  struct action * p;
  
  gobble_chars(stream, " (\t");

  if ((y = parse_int(stream)) < 0)
    syntax_error(stream, "DF y value");    // ystart

  gobble_chars(stream, " ,\t");
  
  if ((fsize = parse_int(stream)) < 0)
    syntax_error(stream, "DF fsize");      // fsize

  gobble_chars(stream, " ,\t");
  
  if (parse_text(stream, device, 64) <= 0) // device name
    syntax_error(stream, "DF device");
      
  gobble_chars(stream, " ,\t");

  if (parse_text(stream, label, 64) <= 0)  // label
    syntax_error(stream, "DF label");

  gobble_chars(stream, " ,\t");

  if ((units=parse_df_units(stream)) == df_none)     // df_units
    syntax_error(stream, "DF units");

  gobble_chars(stream, " ,\t");

  if ((cutoff = parse_int(stream)) < 0)     // cutoff
    syntax_error(stream, "DF cutoff");
	     
  gobble_chars(stream, " ),;\t");   // Allow spurious ',' and ';' in case these were copied from code

  if ((c=getc(stream)) == '\n' || c == EOF)
    ungetc(c, stream);  // Dont' eat the newline
  else
    {
      ungetc(c, stream);
      syntax_error(stream, "DF spurious text following a valid DF definition");
    }
  
  validate(x,y,fsize, 0);

  p = (struct action*) malloc(sizeof(struct action));

  p->next                = NULL;
  p->prev                = NULL;
  
  p->verb           = do_df;
  p->args.df.ystart = y;
  p->args.df.fsize  = fsize;
  p->args.df.units  = units;
  p->args.df.cutoff = cutoff;

  strncpy(p->args.df.label, label,  40);
  strncpy(p->args.df.device,device, 40);

  return p;
}






struct action * start   = NULL;
struct action * current = NULL;;
struct action * new     = NULL;;


int parse_file(FILE *stream)
{
  int c;

  enum verb verb;
  

  for(; /* ever */;)
    {
      if (skip_ws(stream))  // eats whitespace and comments
	break;  // hit EOF

      verb=parse_verb(stream);

      switch (verb)
	{
	case(do_noverb):
	case(do_none):
	  syntax_error(stream, "Finding verb name");
	  break;

	case(do_label):
	  break;

	case(do_loop):
	  break;

	case(do_init_module):
	  break;

	case(do_init_display):
	  break;

	case(do_init_image):
	  break;

	case(do_release_image):
	  break;

	case(do_release_module):
	  break;

	case(do_render):
	  break;

	case(do_clear):
	  break;

	case(do_text):
	  break;

	case(do_hostname):
	  Debug("Parsing HOSTNAME on line %d\n", lineno);
	  new=parse_hostname(stream);
	  Debug("HOSTNAME on line %d is %p\n", lineno, new);
	  break;

	case(do_timestamp):
	  Debug("Parsing TIMESTAMP on line %d\n", lineno);
	  new=parse_timestamp(stream);
	  Debug("TIMESTAMP on line %d is %p\n", lineno, new);
	  break;

	case(do_uptime):
	  break;

	case(do_meter):
	  break;

	case(do_df):
	  Debug("Parsing DF on line %d\n", lineno);
	  new=parse_df(stream);
	  Debug("DF on line %d is %p\n", lineno, new);
	  break;

	case(do_age):
	  break;

	case(do_file):
	  break;

	case(do_sleep):
	  break;
	}


      if (new == NULL)
	{
	  fprintf(stderr, "New action not created (INTERNAL ERROR)\n");
	  exit(2);
	}
      if (start == NULL)
	start = new;  // we are the start

      if (current != NULL)
	{
	current->next = new;
	new->prev     = current;
	}

      current = new;

      if (next_line(stream))
	break; // hit eof
    }
}


