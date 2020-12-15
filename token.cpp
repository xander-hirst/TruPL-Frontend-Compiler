#include <string>
using namespace std;

#include "token.h"

Token::Token()
{
  type = TOKEN_NO_TYPE;
}

Token::~Token()
{}

void Token::set_token_type (const token_type_type t)
{
  type = t;
}

token_type_type Token::get_token_type() const
{
  return type;
}
