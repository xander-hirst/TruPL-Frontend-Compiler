#include <string>
#include <iostream>
using namespace std;

#include "keywordtoken.h"
#include "punctoken.h"
#include "reloptoken.h"
#include "addoptoken.h"
#include "muloptoken.h"
#include "idtoken.h"
#include "numtoken.h"
#include "eoftoken.h"
#include "scanner.h"

Scanner::Scanner (char * filename)
{
  this->buf = new Buffer(filename);
}

Scanner::~Scanner()
{
}

Token *Scanner::next_token()
{
  int done = -1;
  int state = 0;
  string attr = "";
  char c;
  Token *t;
  
  while (state != done){
    c = buf->next_char();
    switch (state){
      case 0:
        if (c == 'b'){
	  state = 1;
	  attr += c;
        } else if (c == 'e'){
	  state = 9;
	  attr += c;
	} else if (c == 'i'){
	  state = 15;
	  attr += c;
	} else if (c == 'l'){
	  state = 19;
	  attr += c;
	} else if (c == 'n'){
	  state = 23;
	  attr += c;
	} else if (c == 'p'){
	  state = 26;
	  attr += c;
	} else if (c == 't'){
	  state = 42;
	  attr += c;
	} else if (c == 'w'){
	  state = 46;
	  attr += c;
	} else if (c == ';'){
	  state = 100;
	} else if (c == ':'){
	  state = 101;
	} else if (c == ','){
	  state = 103;
	} else if (c == '('){
	  state = 104;
	} else if (c == ')'){
	  state = 105;
	} else if (c == '='){
	  state = 106;
	} else if (c == '<'){
	  state = 107;
	} else if (c == '>'){
	  state = 110;
	} else if (c == '+'){
	  state = 200;
	} else if (c == '-'){
	  state = 201;
	} else if (c == 'o'){
	  state = 202;
	  attr += c;
	} else if (c == '*'){
	  state = 300;
	} else if (c == '/'){
	  state = 301;
	} else if (c == 'a'){
	  state = 302;
	  attr += c;
	} else if (is_alpha(c)){
          state = 400;
	  attr += c;
	} else if (is_digit(c)){
	  state = 500;
	  attr += c;
	} else if (c == '$'){
	  state = done;
	  t = new EofToken();
	} else {
	  scanner_fatal_error("Unknown character detected");
	}
	break;
      case 1:
	if (c == 'e'){
	  state = 2;
	  attr += c;
	} else if (c == 'o'){
	  state = 6;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 2:
        if (c == 'g'){
	  state = 3;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
	break;
      case 3:
        if (c == 'i'){
	  state = 4;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 4:
	if (c == 'n'){
	  state = 5;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 5:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_BEGIN);
	} else {
	  state = done;
	  t = new KeywordToken(KW_BEGIN);
	  buf->unread_char(c);
	}
        break;
      case 6:
        if (c == 'o'){
	  state = 7;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 7:
	if (c == 'l'){
	  state = 8;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 8:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_BOOL);
	} else {
	  state = done;
	  t = new KeywordToken(KW_BOOL);
	  buf->unread_char(c);
	}
        break;
      case 9:
	if (c == 'l'){
	  state = 10;
	  attr += c;
	} else if (c == 'n'){
	  state = 13;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (c == ' '){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 10:
	if (c == 's'){
	  state = 11;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 11:
	if (c == 'e'){
	  state = 12;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 12:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_ELSE);
	} else {
	  state = done;
	  t = new KeywordToken(KW_ELSE);
	  buf->unread_char(c);
	}
        break;
      case 13:
	if (c == 'd'){
	  state = 14;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 14:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_END);
	} else {
	  state = done;
	  t = new KeywordToken(KW_END);
	  buf->unread_char(c);
	}
        break;
      case 15:
	if (c == 'f'){
	  state = 16;
	  attr += c;
	} else if (c == 'n'){
	  state = 17;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 16:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_IF);
	} else {
	  state = done;
	  t = new KeywordToken(KW_IF);
	  buf->unread_char(c);
	}
        break;
      case 17:
	if (c == 't'){
	  state = 18;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 18:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_INT);
	} else {
	  state = done;
	  t = new KeywordToken(KW_INT);
	  buf->unread_char(c);
	}
        break;
      case 19:
	if (c == 'o'){
	  state = 20;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 20:
	if (c == 'o'){
	  state = 21;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 21:
	if (c == 'p'){
	  state = 22;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 22:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_LOOP);
	} else {
	  state = done;
	  t = new KeywordToken(KW_LOOP);
	  buf->unread_char(c);
	}
        break;
      case 23:
	if (c == 'o'){
	  state = 24;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (c == ' '){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 24:
	if (c == 't'){
	  state = 25;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 25:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_NOT);
	} else {
	  state = done;
	  t = new KeywordToken(KW_NOT);
	  buf->unread_char(c);
	}
        break;
      case 26:
	if (c == 'r'){
	  state = 27;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 27:
	if (c == 'i'){
	  state = 28;
	  attr += c;
	} else if (c == 'o'){
	  state = 31;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 28:
	if (c == 'n'){
	  state = 29;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 29:
	if (c == 't'){
	  state = 30;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 30:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_PRINT);
	} else {
	  state = done;
	  t = new KeywordToken(KW_PRINT);
	  buf->unread_char(c);
	}
        break;
      case 31:
	if (c == 'c'){
	  state = 32;
	  attr += c;
	} else if (c == 'g'){
	  state = 38;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 32:
	if (c == 'e'){
	  state = 33;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 33:
	if (c == 'd'){
	  state = 34;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 34:
	if (c == 'u'){
	  state = 35;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 35:
	if (c == 'r'){
	  state = 36;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 36:
	if (c == 'e'){
	  state = 37;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 37:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_PROCEDURE);
	} else {
	  state = done;
	  t = new KeywordToken(KW_PROCEDURE);
	  buf->unread_char(c);
	}
        break;
      case 38:
	if (c == 'r'){
	  state = 39;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 39:
	if (c == 'a'){
	  state = 40;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 40:
	if (c == 'm'){
	  state = 41;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 41:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_PROGRAM);
	} else {
	  state = done;
	  t = new KeywordToken(KW_PROGRAM);
	  buf->unread_char(c);
	}
        break;
      case 42:
	if (c == 'h'){
	  state = 43;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 43:
	if (c == 'e'){
	  state = 44;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 44:
	if (c == 'n'){
	  state = 45;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 45:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_THEN);
	} else {
	  state = done;
	  t = new KeywordToken(KW_THEN);
	  buf->unread_char(c);
	}
        break;
      case 46:
	if (c == 'h'){
	  state = 47;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 47:
	if (c == 'i'){
	  state = 48;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 48:
	if (c == 'l'){
	  state = 49;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 49:
	if (c == 'e'){
	  state = 50;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 50:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new KeywordToken(KW_WHILE);
	} else {
	  state = done;
	  t = new KeywordToken(KW_WHILE);
	  buf->unread_char(c);
	}
        break;
      case 100:
        if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_SEMI);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_SEMI);
	  buf->unread_char(c);
	}
        break;
      case 101:
	if (c == '='){
	  state = 102;
	} else if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_COLON);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_COLON);
	  buf->unread_char(c);
	}
        break;
      case 102:
	if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_ASSIGN);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_ASSIGN);
	  buf->unread_char(c);
	}
        break;
      case 103:
	if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_COMMA);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_COMMA);
	  buf->unread_char(c);
	}
        break;
      case 104:
	if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_OPEN);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_OPEN);
	  buf->unread_char(c);
	}
        break;
      case 105:
	if (is_space(c)){
	  state = done;
	  t = new PuncToken(PUNC_CLOSE);
	} else {
	  state = done;
	  t = new PuncToken(PUNC_CLOSE);
	  buf->unread_char(c);
	}
        break;
      case 106:
	if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_EQ);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_EQ);
	  buf->unread_char(c);
	}
        break;
      case 107:
	if (c == '>'){
	  state = 108;
	} else if (c == '='){
	  state = 109;
	} else if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_LT);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_LT);
	  buf->unread_char(c);
	}
        break;
      case 108:
	if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_NE);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_NE);
	  buf->unread_char(c);
	}
        break;
      case 109:
	if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_LE);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_LE);
	  buf->unread_char(c);
	}
        break;
      case 110:
	if (c == '='){
	  state = 111;
	} else if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_GT);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_GT);
	  buf->unread_char(c);
	}
        break;
      case 111:
        if (is_space(c)){
	  state = done;
	  t = new RelopToken(RELOP_GE);
	} else {
	  state = done;
	  t = new RelopToken(RELOP_GE);
	  buf->unread_char(c);
	}
        break;
      case 200:
        if (is_space(c)){
	  state = done;
	  t = new AddopToken(ADDOP_ADD);
	} else {
	  state = done;
	  t = new AddopToken(ADDOP_ADD);
	  buf->unread_char(c);
	}
        break;
      case 201:
	if (is_space(c)){
	  state = done;
	  t = new AddopToken(ADDOP_SUB);
	} else {
	  state = done;
	  t = new AddopToken(ADDOP_SUB);
	  buf->unread_char(c);
	}
        break;
      case 202:
	if (c == 'r'){
	  state = 203;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 203:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new AddopToken(ADDOP_OR);
	} else {
	  state = done;
	  t = new AddopToken(ADDOP_OR);
	  buf->unread_char(c);
	}
        break;
      case 300:
	if (is_space(c)){
	  state = done;
	  t = new MulopToken(MULOP_MUL);
	} else {
	  state = done;
	  t = new MulopToken(MULOP_MUL);
	  buf->unread_char(c);
	}
        break;
      case 301:
	if (is_space(c)){
	  state = done;
	  t = new MulopToken(MULOP_DIV);
	} else {
	  state = done;
	  t = new MulopToken(MULOP_DIV);
	  buf->unread_char(c);
	}
        break;
      case 302:
	if (c == 'n'){
	  state = 303;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 303:
	if (c == 'd'){
	  state = 304;
	  attr += c;
	} else if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
        break;
      case 304:
        if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new MulopToken(MULOP_AND);
	} else {
	  state = done;
	  t = new MulopToken(MULOP_AND);
	  buf->unread_char(c);
	}
        break;
      case 400:
	if (is_alphanum(c)){
	  state = 400;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new IdToken(attr);
	} else {
	  state = done;
	  t = new IdToken(attr);
	  buf->unread_char(c);
	}
	break;
      case 500:
	if (is_digit(c)){
	  state = 500;
	  attr += c;
	} else if (is_space(c)){
	  state = done;
	  t = new NumToken(attr);
	} else {
	  state = done;
	  t = new NumToken(attr);
	  buf->unread_char(c);
	}
	break;
    }//switch
  }//while
  return t;
}//next_token

void Scanner::scanner_fatal_error(const string& mesg)
{
  cout << mesg << '\n';
  exit(-1);
}
