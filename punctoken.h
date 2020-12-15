#ifndef PUNCTOKEN_H
#define PUNCTOKEN_H

#include <string>
using namespace std;

#include "token.h"

typedef enum punc_attr { PUNC_SEMI = 200,
			 PUNC_COLON = 201,
			 PUNC_COMMA = 202,
			 PUNC_ASSIGN = 203,
			 PUNC_OPEN = 204,
			 PUNC_CLOSE = 205,
			 PUNC_NO_ATTR = 299 } punc_attr_type;

class PuncToken : public Token
{
 public:
  PuncToken();

  ~PuncToken();

  PuncToken(punc_attr_type attr);

  punc_attr_type get_attribute() const;
  void set_attribute (const punc_attr_type type);

  string *to_string() const;

 private:
  punc_attr_type attribute;
};

#endif
