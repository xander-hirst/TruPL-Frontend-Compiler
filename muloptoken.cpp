#include <string>
using namespace std;

#include "token.h"
#include "muloptoken.h"

/* Create a new token with no attributes. */
MulopToken::MulopToken ()
{
  // Set the token type and init the attribute to undefined.
  set_token_type (TOKEN_MULOP);
  attribute = MULOP_NO_ATTR;
}

/* Create a new token with an attribute. */
MulopToken::MulopToken (const mulop_attr_type attr)
{
  // Set the token type and init the attribute.
  set_token_type (TOKEN_MULOP);
  attribute = attr;
}

/* Nothing to do here. */
MulopToken::~MulopToken ()
{
}

/* Return the token's attribute. */
mulop_attr_type MulopToken::get_attribute() const
{
  return attribute;
}

/* Set the tokens attribute. */
void MulopToken::set_attribute(mulop_attr_type attr)
{
  attribute = attr;
}

/* Extract the token type and attribute, and return a string*
   containing the information.  Used for debugging only.
*/
string *MulopToken::to_string() const
{
  string attribute_name;

  switch (attribute) {
  case MULOP_MUL:
    attribute_name = "MULTIPLICATION";
    break;
  case MULOP_DIV:
    attribute_name = "DIVISION";
    break;
  case MULOP_AND:
    attribute_name = "AND";
    break;
  case MULOP_NO_ATTR:
    attribute_name = "NO ATTRIBUTE";
    break;
  default:
    attribute_name = "GARBAGE ATTRIBUTE VALUE";
    break;
  }

  return new string("MULOP:" + attribute_name);
}
