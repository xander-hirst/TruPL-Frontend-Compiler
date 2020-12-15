#ifndef IDTOKEN_H
#define IDTOKEN_H

#include <string>
using namespace std;

class IdToken : public Token
{
 public:
  IdToken();
  IdToken(const string& attr);
  ~IdToken();

  string *get_attribute() const;
  void set_attribute(const string& attr);

  string *to_string() const;

 private:
  string *attribute;
};

#endif
