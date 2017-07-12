#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Global.hpp"

//
// Global variables
//

extern pDeclared FirstDeclare;

//
// Global Function Prototypes
//
pToken TokenizeFile(char* Input, pToken FirstToken);
pToken GetNextToken(pToken T, char** p, long* Current_Line);

//
//  Classes
//

class cToken : public RefCount {
 protected:
  pToken m_next;
  long m_id;
  long m_line;
  char* m_filename;
  void* m_data;

 public:
  cToken() { m_next = NULL; };

  long& ID() { return m_id; };
  long& Line() { return m_line; };
  char*& Filename() { return m_filename; };
  void*& Data() { return m_data; };
  pToken& Next() { return m_next; };
};

class cDeclared : public RefCount {
 protected:
  char* m_declared_identifier;
  pToken m_first_token;
  pDeclared m_next;

 public:
  char*& DeclaredIdentifier() { return m_declared_identifier; };
  pToken& FirstToken() { return m_first_token; };
  pDeclared& Next() { return m_next; };
};

#endif
