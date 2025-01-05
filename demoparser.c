#include "parser.h"

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
