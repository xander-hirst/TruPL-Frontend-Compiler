#include <string>
#include <cstdlib>
using namespace std;

#include "token.h"
#include "idtoken.h"

IdToken::IdToken()
{
  set_token_type (TOKEN_ID);
  attribute = new string("UNINITIALIZED IDENTIFIER ATTRIBUTE");
}

IdToken::IdToken (const string& attr)
{
  set_token_type (TOKEN_ID);
  attribute = new string(attr);
}

IdToken::~IdToken()
{
  // attribute should never be NULL, but it doesn't hurt to check
  if (attribute != NULL) {
    delete attribute;
  }
}


string *IdToken::get_attribute() const
{
  string *attr = new string(*attribute);
  return attr;
}


void IdToken::set_attribute(const string& attr)
{
  attribute = new string (attr);
}


string *IdToken::to_string() const
{
  return new string ("ID:" + *attribute);
}
