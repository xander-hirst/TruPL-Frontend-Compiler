#include <string>
using namespace std;

#include "token.h"
#include "keywordtoken.h"

/* Create a new token with no attributes. */
KeywordToken::KeywordToken ()
{
  // Set the token type and init the attribute to undefined.
  set_token_type (TOKEN_KEYWORD);
  attribute = KW_NO_ATTR;
}

/* Create a new token with an attribute. */
KeywordToken::KeywordToken (const keyword_attr_type attr)
{
  // Set the token type and init the attribute.
  set_token_type (TOKEN_KEYWORD);
  attribute = attr;
}

/* Nothing to do here. */
KeywordToken::~KeywordToken ()
{
}

/* Return the token's attribute. */
keyword_attr_type KeywordToken::get_attribute() const
{
  return attribute;
}

/* Set the tokens attribute. */
void KeywordToken::set_attribute(keyword_attr_type attr)
{
  attribute = attr;
}

/* Extract the token type and attribute, and return a string*
   containing the information.  Used for debugging only.
*/
string *KeywordToken::to_string() const
{
  string attribute_name;

  switch (attribute) {
  case KW_PROGRAM:
    attribute_name = "PROGRAM";
    break;
  case KW_PROCEDURE:
    attribute_name = "PROCEDURE";
    break;
  case KW_INT:
    attribute_name = "INT";
    break;
  case KW_BOOL:
    attribute_name = "BOOL";
    break;
  case KW_BEGIN:
    attribute_name = "BEGIN";
    break;
  case KW_END:
    attribute_name = "END";
    break;
  case KW_IF:
    attribute_name = "IF";
    break;
  case KW_THEN:
    attribute_name = "THEN";
    break;
  case KW_ELSE:
    attribute_name = "ELSE";
    break;
  case KW_WHILE:
    attribute_name = "WHILE";
    break;
  case KW_LOOP:
    attribute_name = "LOOP";
    break;
  case KW_PRINT:
    attribute_name = "PRINT";
    break;
  case KW_NOT:
    attribute_name = "NOT";
    break;
  case KW_NO_ATTR:
    attribute_name = "NO ATTRIBUTE";
    break;
  default:
    attribute_name = "GARBAGE ATTRIBUTE VALUE";
    break;
  }

  return new string("KEYWORD:" + attribute_name);
}

  
  
