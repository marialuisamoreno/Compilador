/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the symbtree file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case RETURN:
    case WHILE:
      fprintf(symbtree,
         "reserved word: %s\n",tokenString);
      break;
    case ATRIB: fprintf(symbtree,":=\n"); break;
    case SLT: fprintf(symbtree,"<\n"); break;
    case SLTE: fprintf(symbtree,"<=\n"); break;
    case SGT: fprintf(symbtree,">\n"); break;
    case SGTE: fprintf(symbtree,"<=\n"); break;
    case EQUAL: fprintf(symbtree,"=\n"); break;
    case DIFFERENT: fprintf(symbtree,"!=\n"); break;
    case OPAREN: fprintf(symbtree,"(\n"); break;
    case CPAREN: fprintf(symbtree,")\n"); break;
    case SEMICOLON: fprintf(symbtree,";\n"); break;
    case ADD: fprintf(symbtree,"+\n"); break;
    case SUB: fprintf(symbtree,"-\n"); break;
    case MULT: fprintf(symbtree,"*\n"); break;
    case DIV: fprintf(symbtree,"/\n"); break;
    case ENDFILE: fprintf(symbtree,"EOF\n"); break;
    case NUM:
      fprintf(symbtree,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(symbtree,
          "ID, name= %s\n",tokenString);
      break;
    case ERR:
      fprintf(symbtree,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(symbtree,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(symbtree,"Out of memory error at line %d\n",line_counter);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = line_counter;
    t->type = Void;
    t->decl_line = -1;
    t->func = 0;
    t->atrib = 0;
    t->decl = 0;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(symbtree,"Out of memory error at line %d\n",line_counter);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = line_counter;
    t->type = Void;
    t->decl_line = -1;
    t->func = 0;
    t->atrib = 0;
    t->decl = 0;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = (char*) malloc(n);
  if (t==NULL)
    fprintf(symbtree,"Out of memory error at line %d\n",line_counter);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(symbtree," ");
}

/* procedure printTree prints a syntax tree to the 
 * symbtree file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(symbtree,"If\n");
          break;
        case WhileK:
          fprintf(symbtree,"While\n");
          break;
        case AssignK:
          fprintf(symbtree,"Assign to: %s\n",tree->attr.name);
          break;
        case ReturnK:
          fprintf(symbtree,"Return\n");
          break;
        default:
          fprintf(symbtree,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          fprintf(symbtree,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          fprintf(symbtree,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(symbtree,"Id: %s\n",tree->attr.name);
          break;
        case TypeK:
          fprintf(symbtree,"Type: %s\n",tree->attr.name);
          break;
        default:
          fprintf(symbtree,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(symbtree,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
