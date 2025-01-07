#ifndef _DUMP_UTILS_H_
#define _DUMP_UTILS_H_


#include <ctype.h>
#include <stdlib.h>
#include "actions.h"
#include <stdio.h>
#include <string.h>



extern char          *  str_colour   (enum Eink_colour colour);
extern char          *  str_df_units (enum df_units    colour);
extern char          *  str_age      (enum age_units   colour);

#endif
