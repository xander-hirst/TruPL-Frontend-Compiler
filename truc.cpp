#include <iostream>
#include <cstdlib>

using namespace std;

#include "parser.h"
#include "scanner.h"

int main (int argc, char **argv)
{
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <input filename> " << endl;
    exit (-1);
  }

  Scanner *s = new Scanner(argv[1]);
  Parser *p = new Parser (s);

  if (p->parse_program()) {
    if (p->done_with_input()) {
      cout << "Parse succeeded." << endl;
    } else {
      cout << "Extra input at end of program." << endl;
    }
  } else {
    cout << "Parse failed." << endl;
  }
}
