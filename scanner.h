#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "buffer.h"

class Scanner
{
 public:
  Scanner (char *filename);

  ~Scanner();

  // Return the next token in the file.
  Token *next_token();

 private:

  Buffer *buf;

  inline bool is_alpha (char c) const 
  {
    return c >= 'a' && c <= 'z';
  }

  inline bool is_digit (char c) const
  {
    return c >= '0' && c <= '9';
  }

  inline bool is_alphanum (char c) const
  {
    return is_alpha(c) || is_digit(c);
  }

  inline bool is_space(char c) const
  {
    return c == ' ';
  }

  void scanner_fatal_error(const string& mesg);

};
#endif
  
