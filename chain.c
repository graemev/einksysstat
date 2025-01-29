/* -*- Mode: c; tab-width: 4;c-basic-offset: 4; c-default-style: "gnu" -*- */
#include <assert.h>
#include "actions.h"
#include "chain.h"

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
 * Chain:  Utilities to manage a chain (a doubly linked list) of actions 
 * 
 * This has globals, because it needs to integrate with the (Bison generated)
 * parser. The parser uses things like yyin (a global) to link with user code.
 *
 * Normal "good practice" would be to #include "eink_sysstat.h" so that 
 * all sysmols are checked agaist all others, avoiding wierd linking issues.
 * But here we are looking to limit exposure to these externals. So we create
 * chain.h to expose these and these will NOT be in eink_sysstat.h"
 *
 * Now, if we want propper encapsulation 
 *
 *
 *
 */

/* GLOBAL*/ struct action *action_chain = NULL;
/* GLOBAL*/ int            action_count = 0;


static struct action *chain_end = NULL;

/*
 * Add (an existing action) to the chain
 */
int enchain(struct action *action)
{
  if (action_chain == NULL) // special case , empty list
    {
      action->next = NULL;
      action->prev = NULL;
      action_chain = action;
      chain_end    = action;
    }
  else
    {
      assert(chain_end !=NULL);

      chain_end->next = action;

      action->prev    = chain_end;
      action->next    = NULL;

      chain_end       = action;
      
    }
  return(++action_count);
}


/*
 * Remove (an existing enchained action) from the chain
 *
 * REMEMBER TO FREE(3) IT! after this
 */
int dechain(struct action *action)
{
  struct action *before;
  struct action *after;

  assert(action       != NULL);

  before = action->prev;
  after  = action->next;
  
  /* Two special cases. We are the first and we are the last action (can be both) */

  if (before == NULL)
    action_chain = action; // we are new start
  else
    before->next = after;  // just bypass us

  if (after = NULL)
    chain_end = action;	   // we are the new end
  else
    after->prev = before;  // just bypass us

  action->next = NULL;
  action->prev = NULL;

  return(--action_count);
}



