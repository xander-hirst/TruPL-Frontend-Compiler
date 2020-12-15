#include "symbol_table.h"

SymbolTable::SymbolTable()
{
}


SymbolTable::~SymbolTable()
{
}

/* Install an identifier from environment env with type t into
   the symbol table.  Does not check for duplicates. */
void SymbolTable::install (const string *id, const string *env, 
			    const expr_type t)
{
  STAB_ENTRY new_entry;
  new_entry.id = *id;
  new_entry.env = *env;
  new_entry.position = -1;
  new_entry.type = t;
  stab.push_back (new_entry);
}

/* Install an identifier from the environment env with type t
   and position pos into the symbol table.  Used only for 
   formal parameters.  Does not check for duplicates. */
void SymbolTable::install (const string *id, const string *env, 
				 const expr_type t, const int pos)
{
  STAB_ENTRY new_entry;
  new_entry.id = *id;
  new_entry.env = *env;
  new_entry.position = pos;
  new_entry.type = t;
  stab.push_back (new_entry);
}


/* Is the identifier id declared in the environment env? */
bool SymbolTable::is_decl (const string *id, const string *env)
{
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->id.compare(*id) == 0 && it->env.compare(*env) == 0) {
      return true;
    }
  }
  return false;
}


/* Get the expression type of the identifier id in the environment
   env from the symbol table.  Return GARBAGE_T if that id
   is not found. */
expr_type SymbolTable::get_type (string *id, string *env)
{
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->id.compare(*id) == 0 && it->env.compare(*env) == 0) {
      return it->type;
    }
  }
  
  return GARBAGE_T;
}


/* Get the type of the formal parameter proc_id in position pos from
   the symbol table.  The name of the procedure will be the
   identifier's environment as stored in the symbol table.  Return
   GARBAGE_T if there is not a formal parameter in that position for
   this procedure.  */
expr_type SymbolTable::get_type (string *proc_id, const int pos)
{
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->env.compare(*proc_id) == 0 
	&& it->position == pos) {
      return it->type;
    }
  }

  return GARBAGE_T;
}

/* Change the type of all symbol table variables with type UNKNOWN_T
   to t.  This method is used to set the types of variables in the
   symbol table, once we discover them. */
void SymbolTable::update_type (expr_type t)
{
  /* Change the type of all symbol table variables with type UNKNOWN_T
     to standard_type_type. */
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->type == UNKNOWN_T) {
      it->type = t;
    }
  }
}

/* Convert an expr_type to a string.  Useful for debugging. */
string *SymbolTable::type_to_string (const expr_type t) const
{
  string *result;

  switch (t) {
  case INT_T:
    result = new string ("INT_T");
    break;
  case BOOL_T:
    result = new string ("BOOL_T");
    break;
  case PROCEDURE_T:
    result = new string ("PROCEDURE_T");
    break;
  case PROGRAM_T:
    result = new string ("PROGRAM_T");
    break;
  case NO_T:
    result = new string ("NO_T");
    break;
  case UNKNOWN_T:
    result = new string ("UNKNOWN_T");
    break;
  default:
    result = new string ("GARBAGE_T");
    break;
  }

  return result;
}
