#include <string>
using namespace std;

#include "token.h"
#include "eoftoken.h"

EofToken::EofToken()
{
  set_token_type (TOKEN_EOF);
}

EofToken::~EofToken()
{
}

string *EofToken::to_string() const
{
  return new string("EOF");
}
