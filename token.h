/* This is an include guard.  This prevents the programmer
   from inadvertently including a header more than once.  All of your
   header files should include an ifndef..define..endif include guard.
 */
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

/* An enum is like a series of int constant definitions,
   except the enum provides some rudimentary type checking.
*/
typedef enum token_type { TOKEN_KEYWORD =  0,
			  TOKEN_PUNC    =  1,
			  TOKEN_RELOP   =  2,
			  TOKEN_ADDOP   =  3,
			  TOKEN_MULOP   =  4,
			  TOKEN_ID      =  5,
			  TOKEN_NUM     =  6,
			  TOKEN_EOF     =  7,
			  TOKEN_NO_TYPE = 99 } token_type_type;

/* Token is a abstract superclass.  That means that we will not
   construct any objects of type Token.  All tokens will be created
   from one of the subclasses. */
class Token
{

 public:
  // Default constructor
  Token();

  /* The keyword "virtual" marks the function as a virtual function.
     That means that if we override the function in a derived (sub)
     class, then the derived class function will be called, even if we
     reference the object through a base (super) class object.
  */
  virtual ~Token();

  /* Form a string consisting of the token type and it's attribute.
     Useful for debugging. 
     
     The form of the output should be TOKEN_TYPE:Attribute. 

     The "= 0" notation marks this as a pure virtual function.  It
     must be over-ridden in the derived class.  Having such a function
     in a base class turns it into an abstract class.
  */
  virtual string *to_string() const = 0;

  // Set the token type.
  void set_token_type (const token_type_type t);


  // Return the type of this token.
  token_type_type get_token_type() const;

 private:
  // The type of the tokens
  token_type_type type;
  
};

#endif
