#ifndef NUMTOKEN_H
#define NUMTOKEN_H

#include <string>
using namespace std;

class NumToken : public Token
{
 public:
  NumToken();
  NumToken(const string& attr);
  ~NumToken();

  string *get_attribute() const;
  void set_attribute(const string& attr);

  string *to_string() const;

 private:
  // We'll store the number as a string throughout the compiler.
  string *attribute;
};

#endif
