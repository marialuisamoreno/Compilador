#include <iostream>
using namespace std;

extern void abrirArq(char *arq);
extern void fecharArq();
#include "tiny/globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "tiny/util.h"
#if NO_PARSE
#include "tiny/scan.h"
#else
#include "tiny/parse.h"
#if !NO_ANALYZE
#include "tiny/analyze.h"
#if !NO_CODE
#include "tiny/cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int line_counter = 0;
FILE * source = NULL;
FILE * listing = stdout;
FILE * symbtab = stdout;
FILE * symbtree = stdout;
FILE * code = stdout;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

extern int semantical(FILE *listing);

int main(int argc, char **argv){
  TreeNode *raiz;
  int erro;
  cout << "\nParser em execução...\n"; 
  if(argc == 1) return 1;
  abrirArq(argv[1]);
  if(argc > 2)
    symbtree = fopen(argv[2], "w");
  if(argc > 3)
    symbtab = fopen(argv[3], "w");
  if(argc > 4)
    code = fopen(argv[4], "w");
  raiz = parse();
  printTree(raiz);
  buildSymtab(raiz);
  erro = semantical(listing);
  if(!erro)
    codeGen(raiz, "code.txt");
  fecharArq();
  return 0;
}