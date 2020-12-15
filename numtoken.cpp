#include <string>
#include <cstdlib>
using namespace std;

#include "token.h"
#include "numtoken.h"

NumToken::NumToken()
{
  set_token_type (TOKEN_NUM);
  attribute = new string("UNINITIALIZED NUMBER ATTRIBUTE");
}

NumToken::NumToken (const string& attr)
{
  set_token_type (TOKEN_NUM);
  attribute = new string(attr);
}

NumToken::~NumToken()
{
  // attribute should never be NULL, but it doesn't hurt to check
  if (attribute != NULL) {
    delete attribute;
  }
}


string *NumToken::get_attribute() const
{
  string *attr = new string(*attribute);
  return attr;
}


void NumToken::set_attribute(const string& attr)
{
  attribute = new string (attr);
}


string *NumToken::to_string() const
{
  return new string ("NUM:" + *attribute);
}
