
/*
 * Chain:  Utilities to manage a chain (a doubly linked list) of actions 
 * 
 * This has globals, because it needs to integrate with the (Bison generated)
 * parser. The parser uses things like yyin (a global) to link with user code.
 *
 * Normal "good practice" would be to #include "eink_sysstat.h" so that 
 * all sysmols are checked agaist all others, avoiding wierd linking issues.
 * But here we are looking to limit exposure to these externals. So we create
 * chain.h to expose these and these will NOT be in eink_sysstat.
 */

#include "actions.h"

extern struct action *action_chain;
extern int            action_count;

int enchain(struct action *action);
int dechain(struct action *action);
