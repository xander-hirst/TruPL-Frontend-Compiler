#ifndef BUFFER_H
#define BUFFER_H

#include <list>
#include <fstream>
using namespace std;

class Buffer
{

  
private: // Instance variables

  // This is the largest we let the buffer grow.  You can ignore.
  static const int MAX_BUFFER_SIZE = 1024;
 
  // Storage for the buffer
  list<char> b;

  // The file we are buffering
  ifstream source_file;
  
  enum buffer_state {white_space, comment, legal_char, illegal_char};
  
public: // Public methods
  /* Class constants. 
     C++11 requires non-int class constants to be initialized outside
     of the class.  We'll do this in the .cpp file. I think this is
     fixed in the most recent version of C++.

     We'll use the $ character to make the end of the buffer.*/
  static const char EOF_MARKER = '$';       // $
  static const char COMMENT_MARKER = '#';   // #
  static const char SPACE = ' ';            // ' '
  static const char TAB = '\t';             // \t
  static const char NEW_LINE = '\n';        // \n
  
  // Print an error message and die.
  void buffer_fatal_error() const;
  
  /* Fill the buffer from the file.  The buffer filling code is where
     all the action happens.  This function should place all
     characters from the file in the buffer, with the exception of
     comments (which should be stripped out) and chunks of whitespace.
     All chunks of whitespace should be compressed to a single
     space. */
  void fill_buf();
  
  bool in_prologue;
  
  // Useful method to check if a character is whitespace.
  bool is_whitespace (const char c) const
  {
    return (c == SPACE || c == TAB || c == NEW_LINE);
  }
  
  // For debugging: dump the current contents of the buffer.  You may
  // or may not find this useful.
  void dump_b();
  
  
  // Open the program file and initialize the scanner buffer.
  Buffer(char *filename);
  
  // Close the file and cleanup.
  ~Buffer();
  
  // Remove the next character from the buffer and
  // return it.
  char next_char();
  
  // Put a character back at the front of the buffer.
  void unread_char (char c);
};

#endif
