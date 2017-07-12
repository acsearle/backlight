//
//  Tokeniser functions
//

#include "Tokenizer.hpp"

//
// Global variables
//
pDeclared FirstDeclare;

int Test(char** p, const char* t) {
  long i = 0;

  // Run through characters until end of either string

  while (((*p)[i] == t[i]) && ((*p)[i] != 0)) {
    ++i;
  }

  if ((t[i] == 0) &&
      ((!(isalnum((*p)[i]) || ((*p)[i] == '_'))) || ((*p)[i] == 0))) {
    // First deviation occurs at t's terminator AND there is space after
    // => t occurs at *p

    (*p) += i;
    return 1;
  } else {
    return 0;
  }
}

pToken IdentifyKeyword(pToken T, char** p) {
  pDeclared d;
  pToken S;
  long i = 0;

  T->ID() = -1;

  do {
    if (Test(p, Keyword[i].m_String)) {
      T->ID() = Keyword[i].m_ID;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      return T->Next();
    }
  } while (++i < ID_ARRAY_SIZE);

  // We are here => not a keyword. So we now parse for #declared
  // identifiers.

  d = FirstDeclare;

  while (d->Next() != NULL) {
    if (Test(p, d->DeclaredIdentifier())) {
      // Copy token string from declare:

      S = d->FirstToken();

      while (S->ID() != ID_END) {
        T->ID() = S->ID();
        T->Line() = S->Line();
        T->Filename() = S->Filename();
        T->Data() = S->Data();  // This means we can't free 'Data'
                                // when updating declares
        T->Next() = new cToken;
        // Globals.MemoryUsed() += sizeof(cToken);
        S = S->Next();
        T = T->Next();
      }

      return T;
    }
    d = d->Next();
  }

  // We are here => Does not match any identifier

  printf("Undeclared identifier in %s at line %li: %.32s\n", T->Filename(),
         T->Line(), *p);
  assert(false);

  return T;
}

pToken IdentifyDirective(pToken T, char** p, long* Current_Line) {
  pToken Q, R, S;
  char* NewName;
  pDeclared d;
  long i;

  if (Test(p, "include")) {
    do {
      (*p)++;
      if (*(*p) == '\n') {
        (*Current_Line)++;
      }
    } while (isspace(*(*p)));

    if (*(*p) != '\"') {
      printf("Expected [\"] in %s at line %li, instead found %.32s\n",
             T->Filename(), *Current_Line, *p);
      assert(false);
    }

    i = -1;

    do {
      (*p)++;
      i++;
    } while ((*(*p) != '\"') && (*(*p) != 0));

    if (*(*p) == 0) {
      printf("Unterminated string constant in %s starting at line %li: %.32s\n",
             T->Filename(), *Current_Line, *p - i - 1);
      assert(false);
    }

    if (i == 0) {
      printf("Null string constant in %s at line %li: %.32s\n", T->Filename(),
             *Current_Line, *p - i - 1);
      assert(false);
    }

    NewName = new char[i + 1];
    // Globals.MemoryUsed() += i + 1;
    memcpy(NewName, *p - i, i);
    NewName[i] = 0;

    T = TokenizeFile(NewName, T);  // Tokenises the include file

    (*p) += 2;

    // Don't free NewName - leave it for future generations of tokens
    // to refer to

    return T;
  }

  if (Test(p, "declare")) {
    do {
      (*p)++;
      if (*(*p) == '\n') {
        (*Current_Line)++;
      }
    } while (isspace(*(*p)));

    // Prevent definition of unaccessable identifiers

    if (!isalpha(**p)) {
      printf(
          "Expected letter to start identifier in %s at line %li, instead "
          "found %.32s\n",
          T->Filename(), *Current_Line, *p);
      assert(false);
    }

    i = 0;
    do {
      (*p)++;
      i++;
    } while (isalnum(*(*p)) || (*(*p) == '_'));

    NewName = new char[i + 1];
    // Globals.MemoryUsed() += i + 1;
    memcpy(NewName, *p - i, i);
    NewName[i] = 0;

    do {
      (*p)++;
      if (*(*p) == '\n') {
        (*Current_Line)++;
      }
    } while (isspace(*(*p)));

    if (*(*p) != '=') {
      printf("Expected [=] in %s at line %li, instead found %.32s\n",
             T->Filename(), *Current_Line, *p);
      assert(false);
    }

    (*p)++;

    // Now, read in the tokens...

    // FIXUP: To define colours, transformations etc. we need the
    // ability to read until depth = 0 on a new line, NOT
    // the retrictive # and # { # # # } forms allowed at the moment

    S = new cToken;
    // Globals.MemoryUsed() += sizeof(cToken);

    S->Line() = *Current_Line;
    S->Filename() = T->Filename();

    R = GetNextToken(S, p, Current_Line);

    R->Line() = *Current_Line;
    R->Filename() = T->Filename();

    if (**p == 0) {
      printf(
          "Expected declared item in %s at line %li, instead found end of "
          "file\n",
          T->Filename(), *Current_Line);
      assert(false);
    }

    switch (S->ID()) {
      case ID_DOUBLE:
        // #declare NUMBER
        // This is a one-token linked list...
        R->ID() = ID_END;
        R->Line() = *Current_Line;
        R->Filename() = T->Filename();
        R->Data() = NULL;
        R->Next() = NULL;
        break;
      case ID_LEFT_ANGLE:
        // #declare < NUMBER, .... NUMBER >
        // Go until we get a '>'

        Q = R;

        do {
          R = Q;

          Q = GetNextToken(R, p, Current_Line);

          Q->Line() = *Current_Line;
          Q->Filename() = T->Filename();

        } while ((R->ID() != ID_RIGHT_ANGLE) && (**p != 0));

        if (**p == 0) {
          printf(
              "Expected continued declaration in %s at line %li, instead found "
              "end of file\n",
              T->Filename(), *Current_Line);
          assert(false);
        }

        // End the list

        Q->ID() = ID_END;
        Q->Line() = *Current_Line;
        Q->Filename() = T->Filename();
        Q->Data() = NULL;
        Q->Next() = NULL;
        break;
      default:
        // #declare Identifier = Previously_Declared OR
        // #declare Identifier = Something { Stuff }

        long i = 0;

        Q = R;

        do {
          R = Q;

          Q = GetNextToken(R, p, Current_Line);

          Q->Line() = *Current_Line;
          Q->Filename() = T->Filename();

          // Use i to keep track of how deep we are in

          if (R->ID() == ID_LEFT_BRACE) {
            i++;
          }
          if (R->ID() == ID_RIGHT_BRACE) {
            i--;
          }

        } while ((i > 0) && (**p != 0));

        if (**p == 0) {
          printf(
              "Expected continued declaration in %s at line %li, instead found "
              "end of file\n",
              T->Filename(), *Current_Line);
          assert(false);
        }

        // End the list

        if (R->ID() == ID_RIGHT_BRACE) {
          // Simple
          Q->ID() = ID_END;
          Q->Line() = *Current_Line;
          Q->Filename() = T->Filename();
          Q->Data() = NULL;
          Q->Next() = NULL;
        } else {
          // Oops - we have read something we shouldn't have.
          // We have to reinsert it into the main stream

          T->ID() = R->ID();
          T->Line() = R->Line();
          T->Filename() = T->Filename();
          T->Data() = R->Data();
          T->Next() = new cToken;
          // Globals.MemoryUsed() += sizeof(cToken);
          T = T->Next();

          // Rid ourselves of the useless Q;

          free(Q);

          // Cap the declare

          R->ID() = ID_END;
          R->Line() = *Current_Line;
          R->Filename() = T->Filename();
          R->Data() = NULL;
          R->Next() = NULL;
        }

        break;
    }

    // Now, we insert the newly declared token string into the list of
    // declared token strings

    d = FirstDeclare;

    while (d->Next() != NULL) {
      if (Test(&NewName, d->DeclaredIdentifier())) {
        // free the old declare

        printf("Warning: Redefining identifier %s\n", NewName);

        R = d->FirstToken();

        while (R != NULL) {
          Q = R->Next();
          free(R);
          R = Q;
        }

        d->FirstToken() = S;
        d->Next() = new cDeclared;
        // Globals.MemoryUsed() += sizeof(cDeclared);
        d = d->Next();
        d->Next() = NULL;
        return T;
      }
      d = d->Next();
    }

    // Does not appear in the list, so we add a new part.

    d->DeclaredIdentifier() = NewName;
    d->FirstToken() = S;
    d->Next() = new cDeclared;
    // Globals.MemoryUsed() += sizeof(cDeclared);
    d = d->Next();
    d->Next() = NULL;

    // So, the result of the above is:
    //      We a have a new or updated string of tokens cDeclared
    //      We have moved *p to the end of the declare
    //      We have not changed T at all, so will continue as though
    //          the declare did not exist

    return T;
  }

  // Neither #include or #declare

  printf("Undefined prepocessor directive in %s at line %li: %.32s\n",
         T->Filename(), T->Line(), *p - 1);
  assert(false);

  return T;
}

pToken GetNextToken(pToken T, char** p, long* Current_Line) {
  long i;

  // This is massive - tests for all the possible first letters
  // individually (!)

  // Move to next text

  while (isspace(*(*p))) {
    if (*(*p) == '\n') {
      (*Current_Line)++;
      T->Line()++;
    }
    (*p)++;
  }

  switch ((*p)[0]) {
    case 0:
      // We do nothing: the token remains unused, and the
      // 0 character breaks the calling loop
      break;

    case '/':  // Comments

      switch ((*p)[1]) {
        case '/':  // For these comments, nesting is unimportant
          (*p)++;
          do {
            (*p)++;
          } while ((*(*p) != '\n') && (*(*p) != 0));
          break;

        case '*':  // These comments can be nested
          // Ironically, DJGPP doesn't seem to
          // actually support nested comments
          // of this type.
          i = 1;
          (*p) += 2;
          do {
            switch ((*p)[0]) {
              case '/':
                switch ((*p)[1]) {
                  case '*':
                    i++;
                    (*p) += 2;
                    break;

                  default:
                    (*p)++;
                    break;
                }
                break;

              case '*':
                switch ((*p)[1]) {
                  case '/':
                    i--;
                    (*p) += 2;
                    break;

                  default:
                    (*p)++;
                    break;
                }
                break;

              default:
                (*p)++;
                break;
            }

            if (*(*p) == '\n') {
              (*Current_Line)++;
              T->Line()++;
            }

          } while ((*(*p) != 0) && (i != 0));

          if (i != 0) {
            printf("Unterminated comment in %s\n", T->Filename());
            assert(false);
          }
          break;

        default:
          printf("Unknown identifier [/] in %s line %li : %.32s ...\n",
                 T->Filename(), (*Current_Line), (*p));
          assert(false);
          break;
      }
      break;

    case '0':  // doubles. Cases overflow to produce an OR effect;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case '+':
    case '-':

      T->ID() = ID_DOUBLE;
      T->Data() = new double;  // Read in double
      // Globals.MemoryUsed() += sizeof(double);
      *((double*)T->Data()) = strtod((*p), p);
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);

      T = T->Next();

      break;

    // Punctuation: Each case sets
    case '{':
      T->ID() = ID_LEFT_BRACE;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case '}':
      T->ID() = ID_RIGHT_BRACE;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case '<':
      T->ID() = ID_LEFT_ANGLE;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case '>':
      T->ID() = ID_RIGHT_ANGLE;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case ',':
      T->ID() = ID_COMMA;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case '=':
      T->ID() = ID_EQUALS;
      T->Data() = NULL;
      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;
      break;

    case '\"':
      T->ID() = ID_STRING;
      (*p)++;
      i = 0;

      while ((**p != '\"') && (**p != 0)) {
        if (**p == '\n') {
          (*Current_Line)++;
        }

        i++;
        (*p)++;
      }

      if (**p == 0) {
        printf("Unterminated string constant in %s starting line %li",
               T->Filename(), T->Line());
        assert(false);
      }

      T->Line() = *Current_Line;

      if (i == 0) {
        printf("Null string constant in %s starting line %li", T->Filename(),
               T->Line());
        assert(false);
      }

      T->Data() = new char*[i + 1];
      // Globals.MemoryUsed() += i + 1;

      memcpy(T->Data(), *p - i, i);

      ((char*)T->Data())[i] = 0;

      T->Next() = new cToken;
      // Globals.MemoryUsed() += sizeof(cToken);
      T = T->Next();
      (*p)++;

      break;

    // Preprocessor directives:

    case '#':
      (*p)++;

      T = IdentifyDirective(T, p, Current_Line);
      break;

    // Keywords or identifiers. Note that while identifiers can
    // contain anything EXCEPT whitespace, they can only start with
    // an upper or lower case letter

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':

      T = IdentifyKeyword(T, p);

      break;

    default:
      printf("Disallowed character %.1s in %.32s\n", *p, T->Filename());
      assert(false);
      break;
  }

  // A precaution...

  T->Filename() = NULL;

  return T;
}

pToken TokenizeFile(char* Input, pToken FirstToken) {
  FILE* Handle;
  char* Text;
  char* p;
  pToken T;
  long FileLength, Current_Line;

  if (Globals.Verbose() >= 1) {
    printf("Opening %.32s\n", Input);
  }

  Handle = fopen(Input, "rt");

  if (Handle == NULL) {
    printf("Fatal Error: Can't m_open %s\n", Input);
    exit(0);
  }

  FileLength = 0;

  do {
    FileLength++;
  } while (getc(Handle) != EOF);

  if (Globals.Verbose() >= 2) {
    printf("File length is %li bytes\n", FileLength);
  }

  Text = new char[FileLength];
  // Globals.MemoryUsed() += FileLength;

  fseek(Handle, 0, 0);
  fread(Text, 1, FileLength, Handle);

  // Don't close until the file is finished - this means it can't be
  // opened again by itself or it's #include files - and so is safe
  // from cyclic effects ?

  Text[FileLength - 1] = 0;

  T = FirstToken;
  Current_Line = 1;
  p = Text;

  // Parsing...

  while (*p != 0) {
    T->Line() = Current_Line;
    T->Filename() = Input;

    T = GetNextToken(T, &p, &Current_Line);
  }

  free(Text);

  // We can now safely allow other subroutines to access the file

  fclose(Handle);

  if (Globals.Verbose() >= 2) {
    printf("File read and closed\n");
  }

  // Return the final token

  return T;
}

void PrintToken(pToken T) {
  printf("%s:%li: ", T->Filename(), T->Line());

  switch (T->ID()) {
    case ID_LEFT_BRACE:
      printf("{ ");
      break;
    case ID_RIGHT_BRACE:
      printf("} ");
      break;
    case ID_LEFT_ANGLE:
      printf("< ");
      break;
    case ID_RIGHT_ANGLE:
      printf("> ");
      break;
    case ID_COMMA:
      printf(", ");
      break;
    case ID_EQUALS:
      printf("= ");
      break;
    case ID_DOUBLE:
      printf("%g ", *((double*)T->Data()));
      break;
    case ID_STRING:
      // printf("String = \"%s\" ", ((char* ) T->Data()));
      printf("String\n");
      break;
    case ID_END:
      printf("End of file ");
      break;
    default:
      if (T->ID() < ID_ARRAY_SIZE) {
        printf("%s ", Keyword[T->ID()].m_String);
      } else {
        printf("Weird T->ID(): %li\n", T->ID());
        assert(false);
      }
      break;
  }
}
