/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
%{
//GLC para gerar parser para calculadora simples

#include <iostream>
using namespace std;

#define YYPARSER /* distinguishes Yacc output from other code files */

#include "tiny/globals.h"
#include "tiny/util.h"
#include "tiny/scan.h"
#include "tiny/parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static char * scope;
static int sc = 1;
int func_id = 0;
char * func[256];
ExpType type[256];
static int savedLine;
static TreeNode * savedTree; /* stores syntax tree for later return */

int yylex(void);
extern char* yytext;
extern int line_counter;
void yyerror(const char *msg);

void init_parse(){
    func[0] = (char*) malloc(sizeof(char)*6);
    func[1] = (char*)  malloc(sizeof(char)*7);
    memcpy(func[0], "input\0", sizeof(char)*6);
    memcpy(func[1], "output\0", sizeof(char)*7);
    type[0] = Integer;
    type[1] = Void;
    func_id = 2;
}

void yyerror(char *);
%}

%start programa
%token IF ELSE WHILE         
%token INT VOID           
%token RETURN               
%left ADD SUB           
%left MULT DIV           
%token SLT SLTE SGT SGTE EQUAL DIFFERENT     
%token ATRIB                       
%token OPAREN CPAREN
%token OBRACT CBRACT        
%token OBRACE CBRACE        
%token ID NUM         
%token SEMICOLON COMMA
%token OCOM CCOM                              
%token COM           
%token ERR                      

%%
programa:	declaracao_lista { savedTree = $1;} 
;
declaracao_lista: declaracao_lista declaracao
{ 
    YYSTYPE t = $1;
    if (t != NULL) { 
        while (t->sibling != NULL)
            t = t->sibling;
        t->sibling = $2;
        $$ = $1; 
    }
    else $$ = $2;
}
| declaracao { $$ = $1; }
;
declaracao: var_declaracao { sc = 1; $$ = $1; } | fun_declaracao { sc = 1; $$ = $1; }
;
var_declaracao: tipo_especificador identificador SEMICOLON 
{
    $$ = $1;
    $$->child[0] = $2;
    $2->decl_line = $2->lineno;
    $2->type = $1->type;
    $2->decl = 1;
    $1->decl = 1;
} 
| tipo_especificador identificador OBRACT numero CBRACT SEMICOLON 
{ 
    $$ = $1;
    $$->child[0] = $2;
    $$->child[0]->child[0] = $4;
    $2->decl_line = $2->lineno;
    $2->type = $1->type;
    $2->decl = 1;
    $1->decl = 1;
} 
;
identificador: ID 
{ 
    $$ = newExpNode(IdK);
    $$->attr.name = copyString(yytext);
    if(sc){
        scope = copyString($$->attr.name);
        $$->scope = (char*) malloc(sizeof(char)*7);
        memcpy($$->scope, "GLOBAL\0", 7);
        sc = 0;
    }
    else $$->scope = scope;
}
;
numero: NUM 
{ 
    $$ = newExpNode(ConstK);
    $$->attr.val = atoi(yytext);
    $$->type = Integer;
}
;
tipo_especificador: INT 
{ 
    $$ = newExpNode(TypeK);
    $$->attr.name = (char*) malloc(sizeof(char)*4);
    memcpy($$->attr.name, "INT\0", 4);
    $$->type = Integer;
} 
| VOID 
{ 
    $$ = newExpNode(TypeK);
    $$->attr.name = (char*) malloc(sizeof(char)*5);
    memcpy($$->attr.name, "VOID\0", 4);
    $$->type = Void; 
}
;
fun_declaracao: tipo_especificador identificador OPAREN params CPAREN composto_decl
{
    $$ = $1;
    $$->child[0] = $2;
    $$->child[0]->child[0] = $4;
    $$->child[0]->child[1] = $6;
    $2->decl_line = $2->lineno;
    $2->type = $1->type;
    if(func_id < 2) init_parse();
    $2->func = 1;
    $2->decl = 1;
    func[func_id] = $2->attr.name;
    type[func_id] = $2->type;
    func_id++;
    /*printf("params:\n");
    printTree($4);
    printf("compos:\n");
    printTree($6);*/
}
;
params: param_lista { $$ = $1; } | VOID { $$ = NULL; }
;
param_lista: param_lista COMMA param
{ 
   YYSTYPE t = $1;
    if (t != NULL) { 
        while (t->sibling != NULL)
            t = t->sibling;
        t->sibling = $3;
        $$ = $1; 
    }
    else $$ = $3;
}
| param { $$ = $1; }
;
param: tipo_especificador identificador 
{  
    $$ = $1;
    $$->child[0] = $2;
    $2->decl_line = $2->lineno;
    $2->type = $1->type;
    $2->decl = 1;
    $1->decl = 1;
} 
| tipo_especificador identificador OBRACT CBRACT
{ 
    $$ = $1;
    $$->child[0] = $2; 
    $2->decl_line = $2->lineno;
    $2->type = $1->type;
    $2->decl = 1;
    $1->decl = 1;
}
;
composto_decl: OBRACE local_declaracoes statement_lista CBRACE
{ 
    YYSTYPE t = $2;
    if(t != NULL){
        while(t->sibling != NULL)
            t = t->sibling;
        t->sibling = $3;
        $$ = $2;
    }
    else $$ = $3;
}
;
local_declaracoes: local_declaracoes var_declaracao
{
    YYSTYPE t = $1;
    if(t != NULL){
        while(t->sibling != NULL)
            t = t->sibling;
        t->sibling = $2;
        $$ = $1;
    }
    else $$ = $2;
}
| { $$ = NULL; }
;
statement_lista: statement_lista statement
{
    YYSTYPE t = $1;
    if(t != NULL){
        while(t->sibling != NULL)
            t = t->sibling;
        t->sibling = $2;
        $$ = $1;
    }
    else $$ = $2;
} 
| { $$ = NULL; }
;
statement: expressao_decl { $$ = $1; }
| composto_decl { $$ = $1; }
| selecao_decl { $$ = $1; }
| iteracao_decl { $$ = $1; }
| retorno_decl { $$ = $1; }
;
expressao_decl: expressao SEMICOLON { $$ = $1; }
| SEMICOLON
;
selecao_decl: IF OPAREN expressao CPAREN statement 
{ 
    $$ = newStmtNode(IfK);
    $$->child[0] = $3;
    $$->child[1] = $5;
}
| IF OPAREN expressao CPAREN statement ELSE statement
{ 
    $$ = newStmtNode(IfK);
    $$->child[0] = $3;
    $$->child[1] = $5;
    $$->child[2] = $7;
}
;
iteracao_decl: WHILE OPAREN expressao CPAREN statement
{
    $$ = newStmtNode(WhileK);
    $$->child[0] = $3;
    $$->child[1] = $5;
}
;
retorno_decl: RETURN SEMICOLON | RETURN expressao SEMICOLON
{
    $$ = newStmtNode(ReturnK);
    $$->child[0] = $2;
}
;
expressao: var ATRIB expressao
{
    $$ = newStmtNode(AssignK);
    $$->attr.name = $1->attr.name;
    $$->atrib = 1;
    $$->type = ($1->type == $3->type) ? $3->type : Void;
    $$->scope = scope;
    $$->child[0] = $1;
    $$->child[1] = $3;
} 
| simples_expressao { $$ = $1; }
;
var: identificador
{ 
    $$ = $1;
    $$->type = Integer;
} 
| identificador OBRACT expressao CBRACT
{
    $$ = $1;
    $$->child[0] = $3;
    $$->type = Integer;
}
;
simples_expressao: soma_expressao relacional soma_expressao 
{
    $$ = $2;
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->type = ($1->type == $3->type) ? $1->type : Void;
}
| soma_expressao { $$ = $1; }
;
relacional: SLTE 
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = SLTE; 
} 
| SLT 
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = SLT;
} 
| SGT 
{
    $$ = newExpNode(OpK);
    $$->attr.op = SGT;
} 
| SGTE 
{
    $$ = newExpNode(OpK);
    $$->attr.op = SGTE; 
} 
| EQUAL 
{
    $$ = newExpNode(OpK);
    $$->attr.op = EQUAL;
} 
| DIFFERENT 
{
    $$ = newExpNode(OpK);
    $$->attr.op = DIFFERENT;
} 
;
soma_expressao: soma_expressao soma termo
{
    $$ = $2;
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->type = ($1->type == $3->type) ? $1->type : Void;
} 
| termo { $$ = $1; }
;
soma: ADD
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = ADD; 
} 
| SUB 
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = SUB; 
}
;
termo: termo mult fator
{
    $$ = $2;
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->type = ($1->type == $3->type) ? $1->type : Void;
}  
| fator { $$ = $1; }
;
mult: MULT
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = MULT; 
} 
| DIV
{ 
    $$ = newExpNode(OpK);
    $$->attr.op = DIV; 
} 
;
fator: OPAREN expressao CPAREN { $$ = $2; } 
| var { $$ = $1; } 
| ativacao { $$ = $1; } 
| numero { $$ = $1; }
;
ativacao: identificador OPAREN simples_expressao CPAREN 
{ 
    $$ = $1;
    $$->child[0] = $3;
    $1->func = 1;
    ExpType t = Void;
    for(int i = 0; i < func_id; i++){
        if( strcmp($1->attr.name, func[i]) == 0 ){
            t = type[i];
            break;
        }
    }
    $$->type = t;
} 
| identificador OPAREN args CPAREN 
{ 
    $$ = $1; 
    $$->child[0] = $3;
    $1->func = 1;
    ExpType t = Void;
    for(int i = 0; i < func_id; i++){
        if( strcmp($1->attr.name, func[i]) == 0 ){
            t = type[i];
            break;
        }
    }
    $$->type = t;
}
;
args: arg_lista { $$ = $1; } | { $$ = NULL; }
;
arg_lista: arg_lista COMMA expressao
{
    YYSTYPE t = $1;
    if(t != NULL){
        while(t->sibling != NULL)
            t = t->sibling;
        t->sibling = $3;
        $$ = $1;
    }
    else $$ = $3;
}
| expressao { $$ = $1; }
;
%%

void yyerror(const char * msg)
{
  extern char* yytext;
  cout << msg << ": " << yytext << " " << yylval << " " << yychar << " line " << line_counter << endl;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}