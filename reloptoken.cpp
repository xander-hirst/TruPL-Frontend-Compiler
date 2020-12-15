#include <string>
using namespace std;

#include "token.h"
#include "reloptoken.h"

/* Create a new token with no attributes. */
RelopToken::RelopToken ()
{
  // Set the token type and init the attribute to undefined.
  set_token_type (TOKEN_RELOP);
  attribute = RELOP_NO_ATTR;
}

/* Create a new token with an attribute. */
RelopToken::RelopToken (const relop_attr_type attr)
{
  // Set the token type and init the attribute.
  set_token_type (TOKEN_RELOP);
  attribute = attr;
}

/* Nothing to do here. */
RelopToken::~RelopToken ()
{
}

/* Return the token's attribute. */
relop_attr_type RelopToken::get_attribute() const
{
  return attribute;
}

/* Set the tokens attribute. */
void RelopToken::set_attribute(relop_attr_type attr)
{
  attribute = attr;
}

/* Extract the token type and attribute, and return a string*
   containing the information.  Used for debugging only.
*/
string *RelopToken::to_string() const
{
  string attribute_name;

  switch (attribute) {
  case RELOP_EQ:
    attribute_name = "EQUALS";
    break;
  case RELOP_NE:
    attribute_name = "NOT EQUALS";
    break;
  case RELOP_GT:
    attribute_name = "GREATER THAN";
    break;
  case RELOP_GE:
    attribute_name = "GREATER THAN OR EQUAL TO";
    break;
  case RELOP_LT:
    attribute_name = "LESS THAN";
    break;
  case RELOP_LE:
    attribute_name = "LESS THAN OR EQUAL TO";
    break;
  case RELOP_NO_ATTR:
    attribute_name = "NO ATTRIBUTE";
    break;
  default:
    attribute_name = "GARBAGE ATTRIBUTE VALUE";
    break;
  }

  return new string("RELOP:" + attribute_name);
}
