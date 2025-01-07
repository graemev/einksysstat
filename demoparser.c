#include "parser.h"
#include "chain.h"

// Bodge , only in Demo ... for real looks at configure.ac
#undef DEBUG
#define DEBUG 2

int main (int argc, const char *argv[])
{
  struct action * p;
  int		  i=0;
  
  // Possibly enable parser runtime debugging.
  yydebug = !!getenv ("YYDEBUG");
  // Enable parse traces on option -p.
  for (int i = 1; i < argc; ++i)
    if (strcmp (argv[i], "-p") == 0)
      yydebug = 1;

  int nerrs = 0;
  yyparse (&nerrs);
  // Exit on failure if there were errors.


  fprintf(stderr, "%d errors in parse\n", nerrs);
  fprintf(stderr, "%d actions parsed\n", action_count);

  fprintf(stderr, "Chain start=%p\n", action_chain);

  
  for (p=action_chain; p; p=p->next)
    {
      fprintf(stderr, "%p: Action[%03d] verb=%-10s,next=%p, prev=%p (%s)\n", p, i++, str_action(p), p->next, p->prev, str_struct_action(p));
    }

  return !!nerrs;
}
