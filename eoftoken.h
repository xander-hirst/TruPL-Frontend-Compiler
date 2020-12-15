#ifndef EOFTOKEN_H
#define EOFTOKEN_H

class EofToken : public Token
{
 public:
  EofToken ();
  ~EofToken ();

  string *to_string() const;

};

#endif

  
