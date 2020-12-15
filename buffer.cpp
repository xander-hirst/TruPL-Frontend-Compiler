#include <iostream>
using namespace std;

#include "buffer.h"

Buffer::Buffer(char *filename)
{
  source_file.open(filename, ios::in);
  this->fill_buf();
}

Buffer::~Buffer()
{
  source_file.close();
}

void Buffer::buffer_fatal_error() const
{
  cout << "Fatal Buffer Error" << '\n';
  exit(-1);
}

void Buffer::fill_buf()
{
  if (source_file.is_open()){
    char c;
    //grab characters from input file until EOF
    while (source_file.get(c)){
      buffer_state state;
      // if # is seen, change state to comment;
      if (c == COMMENT_MARKER){
	state = comment;
      }//if whitespace seen, change state to whitespace
      else if (is_whitespace(c)){
	state = white_space;
      }//if char is legal alphanumeric or operator char
      else if ((c >= 'a' && c <= 'z') || (c >= '0' && c<= '9') ||
	  (c == ';') || (c == ':') || (c == '(') || (c == ')') ||
	  (c == ',') || (c == '=') || (c == '<') || (c == '>') ||
	  (c == '+') || (c == '-') || (c == '*') || (c == '/')){
	state = legal_char;
      }//if c is not in alphabet
      else {
	state = illegal_char;
      }
      switch (state){
	//at most, include only one space to represent some sequence of whitespace
	//the first char in the buffer should not be whitespace
        case white_space:
	  if (b.back() != ' ' && !b.empty()){
	    b.push_back(' ');
	  }
	  break;
	//if comment is seen, read until newline char and push space if last char in buffer was not space
        case comment:
	  while(state == comment){
	    source_file.get(c);
	    if (c == NEW_LINE){
	      state = white_space;
	    }//if
	  }//while
	  if (b.back() != ' ' && !b.empty()){
	    b.push_back(' ');
	  }//if
	  break;
	//if legal_char, push to back of buffer
        case legal_char:
	  b.push_back(c);
	  break;
        case illegal_char:
	  cout << "Illegal char encountered" << '\n';
	  buffer_fatal_error();
       }//switch
    }//while
    //add EOF char to end of buffer
    b.push_back('$');
  }//if
  else {
    cout << "File not opened correctly" << '\n';
    buffer_fatal_error();
  }
}

void Buffer::dump_b()
{
  //print all chars in list
  for (char c : b){
    cout << c;
  }
  cout << '\n';
}

char Buffer::next_char()
{
  //return head of list and pop the element
  char head = b.front();
  b.pop_front();
  return head;
}

void Buffer::unread_char (char c)
{
  //add c to the front of the list
  b.push_front(c);
}
