#include <string>
using namespace std;

#include "token.h"
#include "addoptoken.h"

/* Create a new token with no attributes. */
AddopToken::AddopToken ()
{
  // Set the token type and init the attribute to undefined.
  set_token_type (TOKEN_ADDOP);
  attribute = ADDOP_NO_ATTR;
}

/* Create a new token with an attribute. */
AddopToken::AddopToken (const addop_attr_type attr)
{
  // Set the token type and init the attribute.
  set_token_type (TOKEN_ADDOP);
  attribute = attr;
}

/* Nothing to do here. */
AddopToken::~AddopToken ()
{
}

/* Return the token's attribute. */
addop_attr_type AddopToken::get_attribute() const
{
  return attribute;
}

/* Set the tokens attribute. */
void AddopToken::set_attribute(addop_attr_type attr)
{
  attribute = attr;
}

/* Extract the token type and attribute, and return a string*
   containing the information.  Used for debugging only.
*/
string *AddopToken::to_string() const
{
  string attribute_name;

  switch (attribute) {
  case ADDOP_ADD:
    attribute_name = "ADDITION";
    break;
  case ADDOP_SUB:
    attribute_name = "SUBTRACTION";
    break;
  case ADDOP_OR:
    attribute_name = "OR";
    break;
  case ADDOP_NO_ATTR:
    attribute_name = "NO ATTRIBUTE";
    break;
  default:
    attribute_name = "GARBAGE ATTRIBUTE VALUE";
    break;
  }

  return new string("ADDOP:" + attribute_name);
}
