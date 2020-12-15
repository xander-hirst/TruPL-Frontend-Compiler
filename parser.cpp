#include <cstdlib>
#include <string>
using namespace std;

#include "parser.h"

Parser::Parser(Scanner *the_scanner) 
{
  /* Initialize the parser. */
  lex = the_scanner;
  word = lex->next_token();
  current_env = main_env = procedure_name = NULL;
  actual_parm_position = formal_parm_position = -1;
  parsing_formal_parm_list = false;
}

Parser::~Parser() 
{
  /* Delete the parser. */
  if (lex != NULL) {
    delete lex;
  }
  if (word != NULL) {
    delete word;
  }
}

void Parser::advance()
{
  /* Delete the current token - grab the next one from the scanner. */
  delete word;
  word = lex->next_token();
}

// If we have parsed the entire program, then word
// should be the EOF Token.  This function tests
// that condition.
bool Parser::done_with_input()
{
  return word->get_token_type() == TOKEN_EOF;
}

void Parser::parse_error (string *expected)
{
  cout << "Parse error:  Expected \"" << *expected << "\", found "
       << *(word->to_string()) << endl;
}

void Parser::multiply_defined_identifier (string *id) const
{
  cerr << "The identifier " << *id << " has already been declared. " 
       << endl;
  exit (-1);
}

void Parser::undeclared_identifier (string *id) const
{
  cerr << "The identifier " << *id << " has not been declared. " << endl;
  exit (-1);
}

void Parser::type_error (const expr_type expected,
			 const expr_type found) const
{
  cerr << "Type error: expected " << *(stab.type_to_string (expected))
       << " found " << *(stab.type_to_string(found)) << "." << endl;
  exit (-1);
}


void Parser::type_error (const expr_type expected1,
			 const expr_type expected2,
			 const expr_type found) const
{
  cerr << "Type error: expected " << *(stab.type_to_string (expected1))
       << " or " << *(stab.type_to_string (expected2))
       << ", found " << *(stab.type_to_string(found)) << "." << endl;
  exit (-1);
}

void Parser::diagnostic (string *test) const
{
  cout << *test << endl;
}

bool Parser::parse_program()
{
   // PROGRAM -> program identifier ; DECL_LIST BLOCK ;
   // Predict (program identifier ; DECL_LIST BLOCK ;) == {program}

   /* Match keyword program, first symbol on RHS.
      
      Note we have to do a little OO magic here to cast the token to
      its correct derived class type. We always check to token_type first before
      casting to avoid a type error.  We rely on short-circuit evaluation to avoid
      casting the token if it is of the wrong type.
   */
   if (word->get_token_type() == TOKEN_KEYWORD 
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROGRAM) {
     
     /* ADVANCE - We only delete a token on an ADVANCE,
       and, if we ADVANCE, it is the ADVANCE code that is responsible
       for getting the next token.
     */
     advance();

     // Match identifier, 2nd symbol on RHS
     if (word->get_token_type() == TOKEN_ID) {
       
       // Put program name in symbol table.
       string *id_name = static_cast<IdToken *>(word)->get_attribute();
       string *global_env_name = new string ("_EXTERNAL");
       stab.install (id_name, global_env_name, PROGRAM_T);

       // Set the current and main environments
       current_env = new string (*id_name);
       main_env = new string (*id_name);

       // Clean up
       delete global_env_name;
       delete id_name;
        // ADVANCE
       advance();

	// Match semicolon, 3rd symbol on RHS
	if (word->get_token_type() == TOKEN_PUNC
	    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	  // ADVANCE
	  advance();

	  /* Match DECL_LIST, 4th symbol on RHS.  This is an ACTION,
	     not an advance, so we don't grab another token if we
	     succeed.  We are just growing the parse tree at this
	     point.
	  */
	  if (parse_decl_list()) {
	    
	    // Match BLOCK, 5th on RHS - ACTION
	    if (parse_block()) {
	      
	      // Match semicolon(;), 6th and last on RHS -  
	      if (word->get_token_type() == TOKEN_PUNC
		  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
		advance();

		// Parse of PROGRAM non-terminal succeeded.
		return true;
		
		// We failed to match the second semicolon
	      } else {
		parse_error (new string (";"));
		return false;
	      }
	      
	      /* We failed to parse BLOCK.  But we don't print an
		 error here.  The error will be printed in
		 parse_block(). */
	    } else {
	      return false;
	    }
	    
	    // We failed to parse DECL_LIST
	  } else {
	    return false;
	  }
	  
	  // We failed to match the first semicolon
	} else {
	  parse_error (new string (";"));
	  return false;
	}
	
	// We failed to match an identifier
     } else {
       parse_error (new string ("identifier"));
       return false;
     }
     
     // We failed to match the keyword program
   } else {
     parse_error (new string("program"));
     return false;
   }
   
   /* We shouldn't reach this statement, but it is here as a defensive
      programming measure. */
   return false;
}//parse_program

bool Parser::parse_decl_list()
{
   /* DECL_LIST -> VARIABLE_DECL_LIST PROCEDURE_DECL_LIST 

      Predict(DECL_LIST -> VARIABLE_DECL_LIST PROCEDURE_DECL_LIST) 
        = First(VARIABLE_DECL_LIST) 
             \union First (PROCEDURE_DECL_LIST) 
             \union Follow (DECL_LIST) 
        = {identifier, procedure, begin}
   */
  if (word->get_token_type() == TOKEN_ID
      || (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE)
      || (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) ) {
    if (parse_variable_decl_list()) {
      if (parse_procedure_decl_list()) {
	return true; // matched VDL PDL
	
      } else {
	return false; // failed to match PDL
      }
      
    } else {
      return false; // failed to match VDL
    }
    
  } else {
    parse_error (new string ("identifier, procedure, or begin"));
    return false; // token not in predict set
  }
  
}


bool Parser::parse_variable_decl_list()
{
  /* VARIABLE_DECL_LIST -> VARIABLE_DECL ; VARIABLE_DECL_LIST 
                           | \lambda

     Predict (V_D_L -> V_D ; V_D_L) = {identifier}
     Predict (V_D_L -> \lambda) = FOLLOW (V_D_L) = {procedure, begin}
  */
  if (word->get_token_type() == TOKEN_ID) { // V_D_L -> V_D ; V_D_L, check predict
    
    if (parse_variable_decl()) {
      if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	// matched ; ADVANCE
	advance();
	
	if (parse_variable_decl_list()) {
	  return true; // matched V_D ; V_D_L
	} else {
	  return false; // failed to match V_D_L
	}
	
      } else { // failed to match ;
	  parse_error (new string (";"));
	  return false; // failed to match ";"
      }
	
    } else { // failed to parse_variable_decl()
	return false;
    }
      
    // V_D_L -> \lambda, check predict
  } else if ( (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE)
	      || (word->get_token_type() == TOKEN_KEYWORD
		  && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) ) {
    return true; // matched \lambda

    // word not in any predict set.
  } else { 
      parse_error (new string ("identifier, procedure, or begin"));
      return false;
  }
  
} // parse_variable_decl_list


bool Parser::parse_variable_decl()
{ // V_D -> IDENTIFIER_LIST : STANDARD_TYPE
  // Predict (V_D -> I_L ; S_T) = {identifier}
  if (word->get_token_type() == TOKEN_ID) {
    
    if (parse_identifier_list()) {
      
      if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
	//matched :, advance
	advance();

	expression_type standard_type_type;
	if (parse_standard_type(standard_type_type)) {
	  
	  /* Semantic Action */
	  stab.update_type(standard_type_type);
	  /* End Semantic Action */
	  
	  return true;  // matched I_L ; S_T
	  
	} else {
	  return false; // failed to match S_T
	}
	
      } else { // failed to match :
	parse_error (new string (":"));
	return false;
      }
      
    } else { // failed to match I_L
      return false;
    }
    
  } else { // word not in predict set
    parse_error (new string ("identifier"));
    return false;
  }

}// parse_variable_decl

bool Parser::parse_procedure_decl_list()
{
  /* PROCEDURE_DECL_LIST -> PROCEDURE_DECL ; PROCEDURE_DECL_LIST 
                           | \lambda

     Predict (P_D_L -> P_D ; P_D_L) = {procedure}
     Predict (P_D_L -> \lambda) = FOLLOW (P_D_L) = {begin}
  */
  if (word->get_token_type() == TOKEN_KEYWORD 
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE) {
     
     if (parse_procedure_decl()) {
       if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	 // matched ; ADVANCE
	 advance();
	 
	 if (parse_procedure_decl_list()) {
	   return true; //matched P_D ; P_D_L
	   
	 } else {
	   return false; //failed to match P_D_L
	 }
	 
       } else { //failed to parse ;
	 parse_error(new string (";"));
	 return false;
       }
       
     } else { //failed to parse procedure_decl
       return false;
     }
     
     // P_D_L -> \lambda
  } else if (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) {
    //matched \lambda
    return true;
    
    //word not in any predict set
  } else {
    //failed to match procedure or begin
    parse_error (new string ("procedure or begin"));
    return false;
  }
  
} // parse_procedure_decl_list

bool Parser::parse_identifier_list()
{
  //IDENTIFIER_LIST -> identifier IDENTIFIER_LIST_PRIME
  //Predict(I_L -> id I_L_P) = {identifier}
  if (word->get_token_type() == TOKEN_ID) {

    /* Semantic action */
    string *id_name = static_cast<IdToken *>(word)->get_attribute();
    
    if (stab.is_decl (id_name, current_env) ) {
      multiply_defined_identifier (id_name);
    } else {
      //put in symbol table
      stab.install (id_name, current_env, UNKNOWN_T);
    }
    
    //matched identifier, advance
    advance();
    
    if (parse_identifier_list_prm()){
      return true; //matched I_L -> identifier I_L_P
      
    } else {
      return false; //failed to match I_L_P
    }
    
  } else { //word not in predict set
    parse_error(new string("identifier"));
    return false;
  }
  
}// parse_identifier_list

bool Parser::parse_identifier_list_prm() {
  /*IDENTIFIER_LIST_PRM -> , identifier IDENTIFIER_LIST_PRM
                           | \lambda
    Predict (I_L_P -> , id I_L_P) = {,}
    Predict (I_L_P -> \lambda) = {:} */
  if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA) {
    //matched ',' advance
    advance();

    if (word->get_token_type() == TOKEN_ID) {

      /* Semantic Action */
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
    
      if (stab.is_decl (id_name, current_env) ) {
	multiply_defined_identifier (id_name);
      } else {
	
	if (parsing_formal_parm_list) {
	  stab.install (id_name, current_env, UNKNOWN_T, formal_parm_position);
	  formal_parm_position++;
	} else {
	  stab.install (id_name, current_env, UNKNOWN_T);
	}
      }
      
      //matched id, advance
      advance();
      
      if (parse_identifier_list_prm()) {
	return true; //matched I_L_P -> , id I_L_P
	
      } else {
	return false; //failed to match I_L_P
      }
       
    } else {//failed to match id
      parse_error(new string("identifier"));
      return false;
    }

    //I_L_P -> \lambda
  } else if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
    //matched \lambda
    return true;
    
  } else { //word not in predict set
    parse_error(new string(", or :"));
    return false;
  }

}//parse_identifier_list_prm

bool Parser::parse_standard_type(expr_type &standard_type_type)
{
  //STANDARD_TYPE -> int | bool
  //Predict (S_T -> int) = int
  //Predict (S_T -> bool) = bool
  if ( (word->get_token_type() == TOKEN_KEYWORD
	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_INT) ) {

    /* Semantic Action */
    standard_type_type = INT_T;
    
    //matched int, advance
    advance();
    return true;
    
  } else if ( (word->get_token_type() == TOKEN_KEYWORD
	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_BOOL) ) {

    /* Semantic Action */
    standard_type_type = BOOL_T;
    
    //matched bool, advance
    advance();
    return true;
    
  } else {//word not in predict set
    parse_error(new string("int or bool"));
    return false;
  }

}//parse_standard_type

bool Parser::parse_block()
{
  //BLOCK -> begin STMT_LIST end
  //Predict(BLOCK -> begin S_L end) = {begin}
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN) {
    //matched begin, advance
    advance();

    if (parse_stmt_list()) {
      if (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_END) {
	//matched B -> begin S_L end, advance
	advance();
	return true;
	
      } else {//failed to match end
	parse_error(new string("end"));
	return false;
      }
      
    } else {//failed to match stmt_list
      return false;
    }
    
  } else {//failed to match begin
    parse_error(new string("begin"));
    return false;
  }
}//parse_block

bool Parser::parse_procedure_decl()
{
  /*PROCEDURE_DECL -> procedure identifier ( PROCEDURE_ARGS ) V_D_L BLOCK
    Predict(P_D -> p i ( P_A ) V_D_L B) = {procedure}*/
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE) {
    //matched procedure, advance
    advance();
    
    if (word->get_token_type() == TOKEN_ID){

      /* Semantic Action */
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      
      if (stab.is_decl (id_name, current_env) ) {
	multiply_defined_identifier(id_name);
      } else {
	stab.install (id_name, current_env, PROCEDURE_T);
	current_env = id_name;
	formal_parm_position = 0;
      }
      
      //matched identifier, advance
      advance();

      if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {
	//matched (, advance
	advance();

	if (parse_procedure_args()) {
	  if (word->get_token_type() == TOKEN_PUNC
	      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
	    //matched ), advance
	    advance();
	    
	    if (parse_variable_decl_list()){
	      if (parse_block()){

		/* Semantic Action */
		current_env = main_env;

		//succesfully parsed PROCEDURE_DECL
		return true;
		
	      } else {//failed to parse block
		return false;
	      }
	      
	    } else {//failed to parse V_D_L
	      return false;
	    }
	    
	  } else {//failed to match )
	    parse_error(new string(")"));
	    return false;
	  }
	  
	} else {//failed to parse procedure_args
	  return false;
	}
	
      } else {//failed to match (
	parse_error(new string("("));
	return false;
      }
      
    } else {//failed to match identifier
      parse_error(new string("identifier"));
      return false;
    }
    
  } else {//failed to match procedure
    parse_error(new string("procedure"));
    return false;
  }

}//parse_procedure_decl

bool Parser::parse_procedure_args()
{
  /*PROCEDURE_ARGS -> FORMAL_PARM_LIST
                      | \lambda
    Predict(P_A -> F_P_L) = {identifier}
    Predict(P_A -> \lambda) = { ) }*/
  // P_A -> F_P_L
  if (word->get_token_type() == TOKEN_ID) {

    /* Semantic Action */
    parsing_formal_parm_list = true;
    formal_parm_position = 0; 
    
    if (parse_formal_parm_list()) {

      /* Semantic Action */
      parsing_formal_parm_list = false;
      
      //sucessfully parsed P_A -> F_P_L
      return true;
      
    } else {//failed to parse F_P_L
      return false;
    }
    
  //P_A -> \lambda
  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
    //parsed P_D -> \lambda
    return true;
    
  } else {//word not in any predict set
    parse_error(new string("identifier or )"));
    return false;
  }

}//parse_procedure_args

bool Parser::parse_formal_parm_list()
{
  /*FORMAL_PARM_LIST -> identifier IDENTIFIER_LIST_PRM : STANDARD_TYPE FORMAL_PARM_LIST_HAT
    Predict(F_P_L -> id I_L_P : S_T F_P_L_H) = {identifier}*/
  if (word->get_token_type() == TOKEN_ID) {

    /* Semantic Action */
    string *id_name = static_cast<IdToken *>(word)->get_attribute();
    
    if (stab.is_decl (id_name, current_env) ) {
      multiply_defined_identifier (id_name);
    } else {
      stab.install (id_name, current_env, UNKNOWN_T, formal_parm_position);
      formal_parm_position++;
    }
    
    //matched identifier, advance
    advance();

    if (parse_identifier_list_prm()) {
      if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
	//matched :, advance
	advance();

	expression_type standard_type_type;
	if (parse_standard_type(standard_type_type)) {

	  
	  /* Semantic Action */
//This could be buggy if not in correct environment
	  stab.update_type (standard_type_type);
	  
	  if (parse_formal_parm_list_hat()) {
	    //succesfully parsed FORMAL_PARM_LIST
	    return true;
	    
	  } else {//failed to parse FORMAL_PARM_LIST_HAT
	    return false;
	  }
	  
	} else {//failed to parse STANDARD_TYPE
	  return false;
	}

      } else {//failed to match :
	parse_error(new string(":"));
	return false;
      }
      
    } else {//failed to parse IDENTIFIER_LIST_PRM
      return false;
    }
  } else {//failed to match identifier
    parse_error(new string("identifier"));
    return false;			   
  }
}//parse_formal_parm_list

bool Parser::parse_formal_parm_list_hat()
{
  /*FORMAL_PARM_LIST_HAT -> ; FORMAL_PARM_LIST | \lambda
    Predict(F_P_L_H -> ; F_P_L) = {;}
    Predict(F_P_L_H -> \lambda) = { ) }*/
  if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
    //matched ;, advance
    advance();

    if (parse_formal_parm_list()) {
      //successfully parsed F_P_L_H -> ; F_P_L
      return true;
      
    } else {//failed to parse FORMAL_PARM_LIST
      return false;
    }

  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
    //successfully parsed F_P_L_H -> \lambda
    return true;

  } else {//word not in any predict set
    parse_error(new string("; or )"));
    return false;
  }
}//parse_formal_parm_list_hat

bool Parser::parse_stmt_list()
{
  /*STMT_LIST -> STMT ; STMT_LIST_PRM
    Predict(STMT_LIST -> S ; S_L_P) = {identifier, while, if, print} */
  if ( (word->get_token_type() == TOKEN_ID)
    || (word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE)
    || (word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF)
    || (word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) ) {
    if (parse_stmt()) {
      if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	//matched ;, advance
	advance();

	if (parse_stmt_list_prm()) {
	  //succesfully parsed STMT_LIST
	  return true;
	  
	} else {//failed to parse STMT_LIST_PRM
	  return false;
	}

      } else {//failed to match ;
	parse_error(new string(";"));
	return false;
      }
      
    } else {//failed to parse STMT
      return false;
    }

  } else {//word not in any predict sets
    parse_error(new string("identifier, while, if, or print"));
    return false;
  }

}//parse_stmt_list

bool Parser::parse_stmt_list_prm()
{
  /*STMT_LIST_PRM -> STMT ; STMT_LIST_PRM | \lambda
    Predict(STMT_LIST_PRM -> S ; S_L_P) = {identifier, while, if, print}
    Predict(STMT_LIST_PRM -> \lambda) = {end} */
  
  //STMT_LIST_PRM -> STMT ; STMT_LIST_PRM
  if ( (word->get_token_type() == TOKEN_ID)
    || (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE)
    || (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF)
    || (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) ) {
    if (parse_stmt()) {
      if (word->get_token_type() == TOKEN_PUNC
          && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
	//matched ;, advance
	advance();

	if (parse_stmt_list_prm()) {
	  //succesfully parsed STMT_LIST
	  return true;
	  
	} else {//failed to parse STMT_LIST_PRM
	  return false;
	}

      } else {//failed to match ;
	parse_error(new string(";"));
	return false;
      }
      
    } else {//failed to parse STMT
      return false;
    }

    //STMT_LIST_PRM -> \lambda
  } else if (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_END) {
    //matched \lambda
    return true;

  } else {//word not in any predict sets
    parse_error(new string("identifier, while, if, print, or end"));
    return false;
  }

}//parse_stmt_list_prm

bool Parser::parse_stmt()
{
  /*STMT -> IF_STMT | WHILE_STMT | PRINT_STMT | identifier AD_HOC_AS_PC_TAIL
    Predict(STMT -> IF_STMT) = {if}
    Predict(STMT -> WHILE_STMT) = {while}
    Predict(STMT -> PRINT_STMT) = {print}
    Predict(STMT -> identifier A_H_A_P_T) = {identifier} */
  
  //Predict(STMT -> IF_STMT) = {if}
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF) {
    if (parse_if_stmt()) {
      return true;
      
    } else {//failed to parse IF_STMT
      return false;
    }
    //Predict(STMT -> WHILE_STMT) = {while}
  } else if (word->get_token_type() == TOKEN_KEYWORD
	     && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE) {
    if (parse_while_stmt()) {
      return true;
      
    } else {//failed to parse WHILE_STMT
      return false;
    }
    //Predict(STMT -> PRINT_STMT) = {print}
  } else if (word->get_token_type() == TOKEN_KEYWORD
	     && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) {
    if (parse_print_stmt()) {
      return true;
      
    } else {//failed to parse PRINT_STMT
      return false;
    }
    //Predict(STMT -> identifier A_H_A_P_T) = {identifier}
  } else if (word->get_token_type() == TOKEN_ID) {

    /* Semantic Action */
    string *id_name = static_cast<IdToken *>(word)->get_attribute();
    procedure_name = id_name;

    if (!stab.is_decl (id_name, current_env) ) {
      undeclared_identifier (id_name);
    }
    
    //matched identifier, advance
    advance();

    expression_type ad_hoc_as_pc_tail_type;
    if (parse_ad_hoc_as_pc_tail(ad_hoc_as_pc_tail_type)) {

      /* Semantic Action */
      if (ad_hoc_as_pc_tail_type != stab.get_type (id_name, current_env) ) {
	//expected ad_hoc_as_pc_tail_type, found other
	type_error (ad_hoc_as_pc_tail_type, stab.get_type (id_name, current_env) );
      }
      
      return true;
      
    } else {//failed to parse AD_HOC_AS_PC_TAIL
      return false;
    }
    
  } else {//word not in any predict sets
    parse_error(new string("identifier, while, if, or print"));
    return false;
  }

}//parse_stmt

bool Parser::parse_ad_hoc_as_pc_tail(expr_type &ad_hoc_as_pc_tail_type)
{
  /*AD_HOC_AS_PC_TAIL -> := EXPR | ( EXPR_LIST )
    Predict(A_H_A_P_T -> := EXPR) = {:=}
    Predict(A_H_A_P_T -> ( EXPR_LIST ) ) = { ( } */
  if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_ASSIGN) {
    //matched :=, advance
    advance();

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {

      /* Semantic Action */
      ad_hoc_as_pc_tail_type = expr_type_result;
      
      //successfully parsed AD_HOC_AS_PC_TAIL -> := EXPR
      return true;
      
    } else {//failed to parse EXPR
      return false;
    }

  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {

    /* Semantic Action */
    if (stab.get_type (procedure_name, main_env) != PROCEDURE_T) {
      //expected PROCEDURE_T, found other
      type_error (PROCEDURE_T, stab.get_type (procedure_name, main_env) );
    }
    actual_parm_position = 0;
    
    //matched (, advance
    advance();

    if (parse_expr_list()) {//successfully parsed AD_HOC_AS_PC_TAIL -> ( EXPR_LIST )
      if (word->get_token_type() == TOKEN_PUNC
	  && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {

	/* Semantic Action */
	ad_hoc_as_pc_tail_type = PROCEDURE_T;
	
	//matched ), advance
	advance();

	//successfully parsed ( EXPR_LIST )
	return true;
	
      } else {//failed to match )
	parse_error(new string(")"));
	return false;
      }
      
    } else {//failed to parse EXPR_LIST
      return false;
    }
    
  } else {//word not in any predict set
    parse_error(new string(":= or ("));
    return false;
  }
}//parse_ad_hoc_as_pc_tail

bool Parser::parse_if_stmt()
{
  /*IF_STMT -> if EXPR then BLOCK IF_STMT_HAT
    Predict(IF_STMT -> if E then B I_S_H) = {if} */
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF) {
    //matched if, advance
    advance();

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {

      /* Semantic Action */
      if (expr_type_result != BOOL_T) {
	//expected BOOL_T, found other
	type_error (BOOL_T, expr_type_result);
      }
      
      if (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN) {
	//matched then, advance
	advance();

	if (parse_block()) {
	  if (parse_if_stmt_hat()) {//successfully parsed IF_STMT -> if EXPR then BLOCK IF_STMT_HAT
	    return true;
	    
	  } else {//failed to parse IF_STMT_HAT
	    return false;
	  }
	} else {//failed to parse BLOCK
	  return false;
	}
      } else {//failed to match then
	parse_error(new string("then"));
	return false;
      }
      
    } else {//failed to parse EXPR
      return false;
    }
    
  } else {//failed to match if
    parse_error(new string("if"));
    return false;
  }
}//parse_if_stmt

bool Parser::parse_if_stmt_hat()
{
  /*IF_STMT_HAT -> else BLOCK | \lambda
    Predict (IF_STMT_HAT -> else BLOCK) = {else}
    Predict (IF_STMT_HAT -> \lambda) = {;} */

  //IF_STMT_HAT -> else BLOCK
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_ELSE) {
    //matched else, advance
    advance();
    
    if (parse_block()) {//successfully parsed IF_STMT_HAT -> else BLOCK
      return true;
      
    } else {//failed to parse BLOCK
      return false;
    }
    //IF_STMT_HAT -> \lambda
  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
    //matched \lambda
    return true;
    
  } else {//word not in any predict set
    parse_error(new string("else or ;"));
    return false;
  }
}//parse_if_stmt_hat

bool Parser::parse_while_stmt()
{
  /*WHILE_STMT -> while EXPR loop BLOCK
    Predict(WHILE_STMT -> w E l B) = {while} */
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE) {
    //matched while, advance
    advance();

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {

      /* Semantic Action */
      if (expr_type_result != BOOL_T) {
	//expected BOOL_T, found other
	type_error (BOOL_T, expr_type_result);
      }
      
      if (word->get_token_type() == TOKEN_KEYWORD
	  && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP) {
	//matched loop, advance
	advance();

	if (parse_block()) {//successfully parsed WHILE_STMT -> while EXPR loop BLOCK
	  return true;
	  
	} else {//failed to parse BLOCK
	  return false;
	}
      } else {//failed to match loop
	parse_error(new string("loop"));
	return false;
      }
    } else {//failed to parse EXPR
      return false;
    }

  } else {//failed to match while
    parse_error(new string("while"));
    return false;
  }

}//parse_while_stmt

bool Parser::parse_print_stmt()
{
  /*PRINT_STMT -> print EXPR
    Predict(PRINT_STMT -> print EXPR) = {print} */
  if (word->get_token_type() == TOKEN_KEYWORD
      && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT) {
    //matched print, advance
    advance();

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {

      /* Semantic Action */
      if (expr_type_result != BOOL_T && expr_type_result != INT_T) {
	//expected BOOL_T or INT_T, found other
	type_error (BOOL_T, INT_T, expr_type_result);
      }

      //successfully parsed PRINT_STMT
      return true;
      
    } else {//failed to parse EXPR
      return false;
    }
  } else {//failed to match print
    parse_error(new string("print"));
    return false;
  }

}//parse_print_stmt

bool Parser::parse_expr_list()
{
  /*EXPR_LIST -> ACTUAL_PARM_LIST | \lambda
    Predict(EXPR_LIST -> ACTUAL_PARM_LIST) = {identifier, num, (, +, -, not}
    Predict(EXPR_LIST -> \lambda) = { ) } */

  //EXPR_LIST -> ACTUAL_PARM_LIST
  if ( (word->get_token_type() == TOKEN_ID)
    || (word->get_token_type() == TOKEN_NUM)
    || (word->get_token_type() == TOKEN_PUNC
	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
    || (word->get_token_type() == TOKEN_ADDOP
        && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
    || (word->get_token_type() == TOKEN_ADDOP
        && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
    || (word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {
    if (parse_actual_parm_list()) {//successfully parsed EXPR_LIST -> ACTUAL_PARM_LIST
      return true;
      
    } else {//failed to parse ACTUAL_PARM_LIST
      return false;
    }
    //EXPR_LIST -> \lambda
  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
    //matched \lambda
    return true;
    
  } else {//word not in any predict set
    parse_error(new string("identifier, num, (, +, -, not, or )"));
    return false;
  }

}//parse_expr_list

bool Parser::parse_actual_parm_list()
{
  /*ACTUAL_PARM_LIST -> EXPR ACTUAL_PARM_LIST_HAT
    Predict (A_P_L -> EXPR A_P_L_H) = {identifier, num, (, +, -, not} */
  if ( (word->get_token_type() == TOKEN_ID)
    || (word->get_token_type() == TOKEN_NUM)
    || (word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
    || (word->get_token_type() == TOKEN_ADDOP
        && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
    || (word->get_token_type() == TOKEN_ADDOP
        && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
    || (word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {

      /* Semantic Action */
      if (stab.get_type (procedure_name, actual_parm_position) != expr_type_result) {
	//expected expr_type_result, found other
	type_error (expr_type_result, stab.get_type (procedure_name, actual_parm_position) );
      }
      actual_parm_position++;
      
      if (parse_actual_parm_list_hat()) {//successfully parsed ACTUAL_PARM_LIST
	return true;
	
      } else {//failed to parse ACTUAL_PARM_LIST_HAT
	return false;
      }
      
    } else {//failed to parse EXPR
      return false;
    }
	
  } else {//word not in predict set
    parse_error(new string("identifier, num, (, +, -, or not"));
    return false;
  }

}//parse_actual_parm_list

bool Parser::parse_actual_parm_list_hat()
{
  /*ACTUAL_PARM_LIST_HAT -> , ACTUAL_PARM_LIST | \lambda
    Predict(ACTUAL_PARM_LIST_HAT -> , ACTUAL_PARM_LIST) = {,}
    Predict(ACTUAL_PARM_LIST_HAT -> \lambda) = { ) } */

  //ACTUAL_PARM_LIST_HAT -> , ACTUAL_PARM_LIST
  if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA) {
    //matched ,, advance
    advance();

    if (parse_actual_parm_list()) {//successfully parsed A_P_L_H -> , A_P_L
      return true;
      
    } else {//failed to parse ACTUAL_PARM_LIST
      return false;
    }
    
    //ACTUAL_PARM_LIST_HAT -> \lambda
  } else if (word->get_token_type() == TOKEN_PUNC
	     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {
    //matched \lambda
    return true;

  } else {//word not in any predict sets
    parse_error(new string(", or )"));
    return false;
  }
  
}//parse_actual_parm_list_hat

bool Parser::parse_expr(expr_type &expr_type_result)
{
  /* EXPR -> SIMPLE_EXPR EXPR_HAT
     Predict(EXPR) = {identifier, num, (, +, -, not} */
  if ( (word->get_token_type() == TOKEN_ID)
    || (word->get_token_type() == TOKEN_NUM)
    || (word->get_token_type() == TOKEN_PUNC
	&& static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
    || (word->get_token_type() == TOKEN_ADDOP
	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
    || (word->get_token_type() == TOKEN_ADDOP
	&& static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
    || (word->get_token_type() == TOKEN_KEYWORD
	&& static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {

    expression_type simple_expr_type;
    if (parse_simple_expr(simple_expr_type)) {

      expression_type expr_hat_type;
      if (parse_expr_hat(expr_hat_type)) {

	/* Semantic Action */
	if (expr_hat_type == NO_T) {
	  expr_type_result = simple_expr_type;
	} else if (simple_expr_type == INT_T && expr_hat_type == INT_T) {
	  expr_type_result = BOOL_T;
	} else {
	  //expected INT_T or NO_T, found other
	  type_error (INT_T, NO_T, expr_hat_type);
	}

	//successfully parsed EXPR
	return true;
	
      } else {//failed to parse EXPR_HAT
	return false;
      }
      
    } else {//failed to parse SIMPLE_EXPR
      return false;
    }
    
  } else {//word not in predict set
    parse_error(new string("identifier, num, (, +, -, or not"));
    return false;
  }
  
}//parse_expr

bool Parser::parse_expr_hat(expr_type &expr_hat_type)
{
  /*EXPR_HAT -> relop SIMPLE_EXPR | \lambda
    Predict(EXPR_HAT -> relop SIMPLE_EXPR) = {relop}
    Predict(EXPR_HAT -> \lambda) = { ), ,, loop, then, ;} */
  if (word->get_token_type() == TOKEN_RELOP) {
    //matched relop, advance
    advance();

    expression_type simple_expr_type;
    if (parse_simple_expr(simple_expr_type)) {

      /* Semantic Action */
      if (simple_expr_type == INT_T) {
	expr_hat_type = INT_T;
      } else {
	//expected INT_T, found other
	type_error (INT_T, simple_expr_type);
      }
      
      //successfully parsed EXPR_HAT -> relop SIMPLE_EXPR
      return true;
      
    } else {//failed to parse SIMPLE_EXPR
      return false;
    }
  } else if ( (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
	   || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
           || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP) ) {

    /* Semantic Action */
    expr_hat_type = NO_T;
    
    //matched \lambda
    return true;

  } else {//word not in any predict set
    parse_error(new string("=, <>, >, >=, <, <=, ,, loop, then, ;, or )"));
    return false;
  }
}//parse_expr_hat

bool Parser::parse_simple_expr(expr_type &simple_expr_type)
{
  /*SIMPLE_EXPR -> TERM SIMPLE_EXPR_PRM
    Predict(SIMPLE_EXPR) = {identifier, num, (, +, -, not} */
   if ( (word->get_token_type() == TOKEN_ID)
     || (word->get_token_type() == TOKEN_NUM)
     || (word->get_token_type() == TOKEN_PUNC
	 && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
     || (word->get_token_type() == TOKEN_ADDOP
	 && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
     || (word->get_token_type() == TOKEN_ADDOP
	 && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
     || (word->get_token_type() == TOKEN_KEYWORD
	 && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {

     expression_type term_type;
     if (parse_term(term_type)) {

       expression_type simple_expr_prm_type;
       if (parse_simple_expr_prm(simple_expr_prm_type)) {
	 
	 /* Semantic Action */
	 if (simple_expr_prm_type == NO_T) {
	   simple_expr_type = term_type;
	 } else if (term_type == simple_expr_prm_type) {
	   simple_expr_type = term_type;
	 } else {
	   //expected NO_T or term_type, found other
	   type_error (NO_T, term_type, simple_expr_prm_type);
	 }
	 
	 //successfully parsed SIMPLE_EXPR
	 return true;
	 
       } else {//failed to parse SIMPLE_EXPR_PRM
	 return false;
       }
     } else {//failed to parse TERM
       return false;
     }
   } else {//word not in predict set
     parse_error(new string("identifier, num, (, +, -, or not"));
     return false;
   }

}//parse_simple_expr

bool Parser::parse_simple_expr_prm(expr_type &simple_expr_prm0_type)
{
  /*SIMPLE_EXPR_PRM -> addop TERM SIMPLE_EXPR_PRM | \lambda
    Predict(S_E_P -> addop TERM S_E_P) = {addop} 
    Predict(S_E_P -> \lambda) = {relop, ), ,, loop, then, ;} */
  if (word->get_token_type() == TOKEN_ADDOP) {

    /* Semantic Analysis.
       What kind of addop is this?  int or bool? */
    expr_type addop_type = GARBAGE_T;
    addop_attr_type addop_attr = ADDOP_NO_ATTR;
    
    addop_attr = static_cast<AddopToken *>(word)->get_attribute();
    if (addop_attr == ADDOP_ADD || addop_attr == ADDOP_SUB) {
      addop_type = INT_T;
    } else {
      addop_type = BOOL_T;
    }
    
    //matched addop, advance
    advance();

    expression_type term_type;
    if (parse_term(term_type)) {

      expression_type simple_expr_prm1_type;
      if (parse_simple_expr_prm(simple_expr_prm1_type)) {

	/* Semantic Action */
	if (simple_expr_prm1_type == NO_T) {
	  if (addop_type == term_type) {
	    simple_expr_prm0_type = addop_type;
	  } else {
	    //expected term_type, found other
	    type_error (term_type, addop_type);
	  }
	} else if (addop_type == term_type && term_type == simple_expr_prm1_type) {
	  simple_expr_prm0_type = addop_type;
	} else {
	  //expected addop_type or term_type, found other
	  type_error (addop_type, term_type, simple_expr_prm1_type);
	}
	
	//successfully parsed S_E_P -> addop TERM S_E_P
	return true;
	
      } else {//failed to parse SIMPLE_EXPR_PRM
	return false;
      }
      
    } else {//failed to parse TERM
      return false;
    }
    
  } else if ( (word->get_token_type() == TOKEN_RELOP)
	   || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
	   || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
           || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP) ) {

    /* Semantic Action */
    simple_expr_prm0_type = NO_T;
    
    //matched \lambda
    return true;

  } else {//word not in any predict set
    parse_error(new string("+, -, or, =, <>, >, >=, <, <=, ), ,, ;, then, or loop"));
    return false;
  }
  
}//parse_simple_expr_prm

bool Parser::parse_term(expr_type &term_type)
{
  /*TERM -> FACTOR TERM_PRM
    Predict(TERM) = {identifier, num, (, +, -, not} */
   if ( (word->get_token_type() == TOKEN_ID)
     || (word->get_token_type() == TOKEN_NUM)
     || (word->get_token_type() == TOKEN_PUNC
	 && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN)
     || (word->get_token_type() == TOKEN_ADDOP
	 && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
     || (word->get_token_type() == TOKEN_ADDOP
	 && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
     || (word->get_token_type() == TOKEN_KEYWORD
	 && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {

     expression_type factor_type;
     if (parse_factor(factor_type)) {

       expression_type term_prm_type;
       if (parse_term_prm(term_prm_type)) {
         /* Semantic Analysis */
         if (term_prm_type == NO_T) {
           term_type = factor_type;
         } else if (factor_type == term_prm_type) {
	   term_type = factor_type;
         } else {
	  // Expected factor_type, found term_prm_type.
	   type_error (factor_type, term_prm_type);
         }
         /* End of semantic analysis */
	 //successfully parsed TERM -> FACTOR TERM_PRM
	 return true;
	 
       } else {//failed to parse TERM_PRM
	 return false;
       }
       
     } else {//failed to parse FACTOR
       return false;
     }
     
   } else {//word not in predict set
     parse_error(new string("identifier, num, (, +, -, or not"));
     return false;
   }

}//parse_term

bool Parser::parse_term_prm(expr_type &term_prm0_type)
{
  /*TERM_PRM -> mulop FACTOR TERM_PRM | \lambda
    Predict(TERM_PRM -> mulop FACTOR TERM_PRM) = {mulop}
    Predict(TERM_PRM -> \lambda) = {addop, relop, ), ,, loop, then, ;} */
  if (word->get_token_type() == TOKEN_MULOP) {
    
    /* Semantic Analysis.
       What kind of mulop is this?  int or bool? */
    expr_type mulop_type = GARBAGE_T;
    mulop_attr_type mulop_attr = MULOP_NO_ATTR;
    
    mulop_attr = static_cast<MulopToken *>(word)->get_attribute();
    if (mulop_attr == MULOP_MUL || mulop_attr == MULOP_DIV) {
      mulop_type = INT_T;
    } else {
      mulop_type = BOOL_T;
    }

    //matched mulop, advance
    advance();

    expression_type factor_type;
    if (parse_factor(factor_type)) {

      expression_type term_prm1_type;
      if (parse_term_prm(term_prm1_type)) {

	/* Semantic Analysis continued.  Determine TERM_PRM type. */
	if (term_prm1_type == NO_T && mulop_type == factor_type) {
	  term_prm0_type = mulop_type;
	} else if (mulop_type == factor_type && factor_type == term_prm1_type) {
	  term_prm0_type = mulop_type;
	} else {
	  type_error (mulop_type, factor_type, term_prm1_type);
	}
	//successfully parsed TERM_PRM -> mulop FACTOR TERM_PRM
	return true;
	
      } else {//failed to parse TERM_PRM
	return false;
      }
      
    } else {//failed to parse FACTOR
      return false;
    }
    
  } else if ( (word->get_token_type() == TOKEN_ADDOP)
	   || (word->get_token_type() == TOKEN_RELOP)
	   || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE)
	   || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA)
           || (word->get_token_type() == TOKEN_PUNC
               && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN)
           || (word->get_token_type() == TOKEN_KEYWORD
               && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP) ) {
    //matched \lambda
    term_prm0_type = NO_T;
    return true;
    
  } else {//word not in any predict set
    parse_error(new string("*, /, and, +, -, or, =, <>, >, >=, <, <=, ), ,, ;, then, or loop"));
    return false;
  }
  
}//parse_term_prm

bool Parser::parse_factor(expr_type &factor0_type)
{
  /*FACTOR -> identifier | num | ( EXPR ) | SIGN FACTOR
    Predict(FACTOR -> identifier) = {identifier}
    Predict(FACTOR -> num) = {num}
    Predict(FACTOR -> ( EXPR ) ) = { ( }
    Predict(FACTOR -> SIGN FACTOR) = {+, -, not} */

  //FACTOR -> identifier
  if (word->get_token_type() == TOKEN_ID) {

    /* Semantic Action */
    string *id_name = static_cast<IdToken *>(word)->get_attribute();

    if (!stab.is_decl (id_name, current_env) ) {
      undeclared_identifier (id_name);
    } else {
      factor0_type = stab.get_type (id_name, current_env);
    }
    
    //matched identifier, advance
    advance();
    
    //successfully parsed FACTOR -> identifier
    return true;

    //FACTOR -> num
  } else if (word->get_token_type() == TOKEN_NUM) {

    /* Semantic Action */
    factor0_type = INT_T;
    
    //matched num, advance
    advance();
    
    //successfully parsed FACTOR -> num
    return true;

    //FACTOR -> ( EXPR )
  } else if (word->get_token_type() == TOKEN_PUNC
             && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN) {
    //matched (, advance
    advance();

    expression_type expr_type_result;
    if (parse_expr(expr_type_result)) {
      if (word->get_token_type() == TOKEN_PUNC
          && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE) {

	/* Semantic Action */
	factor0_type = expr_type_result;
	
	//matched ), advance
	advance();

	//successfully parsed FACTOR -> ( EXPR )
	return true;
	
      } else {//failed to match )
	parse_error(new string(")"));
	return false;
      }
    } else {//failed to parse EXPR
      return false;
    }

    //FACTOR -> SIGN FACTOR
  } else if ( (word->get_token_type() == TOKEN_ADDOP
	       && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB)
           || (word->get_token_type() == TOKEN_ADDOP
	       && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD)
           || (word->get_token_type() == TOKEN_KEYWORD
	       && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) ) {

    expression_type sign_type;
    if (parse_sign(sign_type)) {

      expression_type factor1_type;
      if (parse_factor(factor1_type)) {

	/* Semantic Action */
	if (sign_type != factor1_type) {
	  //expected sign_type, found other
	  type_error (sign_type, factor1_type);
	} else {
	  factor0_type = factor1_type;
	}
	
	//successfullly parsed FACTOR0 -> SIGN FACTOR1
	return true;
	
      } else {//failed to parse FACTOR
	return false;
      }
    } else {//failed to parse SIGN
      return false;
    }
    
  } else {//word not in any predict set
    parse_error(new string("identifier, num, (, -, +, or not"));
    return false;
  }
  
}//parse_factor

bool Parser::parse_sign(expr_type &sign_type)
{
  /*SIGN -> + | - | not
    Predict(SIGN -> +) = {+}
    Predict(SIGN -> -) = {-}
    Predict(SIGN -> not) = {not} */
  if (word->get_token_type() == TOKEN_ADDOP
      && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB) {

    /* Semantic action */
    sign_type = INT_T;
    
    //matched -, advance
    advance();
    
    //successfully parsed SIGN -> -
    return true;
    
  } else if (word->get_token_type() == TOKEN_ADDOP
	     && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD) {

    /* semantic action */
    sign_type = INT_T;
    
    //matched +, advance
    advance();
    
    //successfully parsed SIGN -> +
    return true;
    
  } else if (word->get_token_type() == TOKEN_KEYWORD
	     && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT) {

    /* Semantic Action */
    sign_type = BOOL_T;
    
    //matched not, advance
    advance();
    
    //successfully parsed SIGN -> not
    return true;
    
  } else {//word not in any predict set
    parse_error(new string("+, -, or not"));
    return false;
  }
}//parse_sign
