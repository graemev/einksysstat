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
 * dump_utils.c:  Used to allow pretty print of structures
 *
 * Large overlap with parse_utils.c... Initally the same file, but dumping is needed in paperman, strawman,woodeman and ironman
 * the parsing utils were only needed in a recursive decent parser, which we no longer use.
 */


#include "dump_utils.h"



struct lookup
{
  char                            *name;
  enum       token {zero=0, one=1} token;
};




static struct
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



char * str_df_units(enum df_units  unit)
{
  char * p;

  switch (unit)
    {
    case(df_meg):
      p="df_meg";
      break;

    case(df_geg):
      p="df_geg";
      break;

    case(df_best):
      p="df_best";
      break;

    case (df_pcent):
      p="df_pcent";
      break;

    default:
      p="df_BAD_VALUE";
      break;
    }
  return p;
}






static struct
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
    


char * str_colour(enum  Eink_colour colour)
{
	return(colour_lookup[colour].name);
}





static struct
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
    


char * str_age(enum  age_units   age)
{
	return(age_lookup[age].name);
}


static struct
{
  char             *name;
  enum  temp_type   temp;
} temp_lookup[] =
  {

    {"temp_none",	   temp_none},
		{"temp_cpu",	   temp_cpu},
		{"temp_gpu",     temp_gpu},
		{"temp_sdram_c", temp_sdram_c},
		{"temp_sdram_i", temp_sdram_i},
		{"temp_sdram_p", temp_sdram_p},

    {NULL	  , temp_none},
  };
    


char * str_temp(enum  temp_type   temp)
{
	return(temp_lookup[temp].name);
}







static struct
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
    



static char * str_verb(enum verb verb)
{
	return(verb_lookup[verb].name);
}


