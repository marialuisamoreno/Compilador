/*  
    Desenvolvido por Maria Luisa Santos Moreno Sanches
    e por André Lucas Maegina na disciplina de Compiladores
*/

%{
#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"
#include "tiny/globals.h"
#include "tiny/util.h"
#include "tiny/scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
extern YYSTYPE yylval;
/*
#define IF                  300
#define ELSE                301
#define INT                 302
#define RETURN              303
#define VOID                304
#define WHILE               305
#define ADD                 306
#define SUB                 307
#define MULT                308
#define DIV                 309
#define SLT                 310
#define SLTE                311
#define SGT                 312
#define SGTE                313
#define EQUAL               314
#define DIFFERENT           315
#define ATRIB               316
#define SEMICOLON           317
#define COMMA               318
#define OBRACE              319
#define CBRACE              320
#define OPAREN              321
#define CPAREN              322
#define OBRACT              323
#define CBRACT              324
#define OCOM                325
#define CCOM                326
#define ID                  327
#define NUM                 328
#define END                 329
#define COM                 330
#define ERR                 331
#define IGN                 332
*/

void abrirArq(char *arq){
  source = fopen(arq, "r");
}

void fecharArq(){
    fclose(source);
}

int erroLexico (){
    printf("Erro lexico: %s na linha %d.\n", yytext, line_counter);
    return ERR;
}

%}

DIGIT [0-9]
LETTER [a-zA-Z]
OTHER [^0-9a-zA-Z;"+""*"/\"<>=,"{""}""("")"\-\[\]\n\t\r ]
%%
if                                    return IF;
else                                  return ELSE;
int                                   return INT;
return                                return RETURN;
void                                  return VOID;
while                                 return WHILE;
{LETTER}+                             return ID;
{DIGIT}+                              return NUM;
"+"                                   return ADD;
-                                     return SUB;
"*"                                   return MULT;
"/"                                   return DIV;
"<"                                   return SLT;
"<="                                  return SLTE;
">"                                   return SGT;
">="                                  return SGTE;
==                                    return EQUAL;
!=                                    return DIFFERENT;
=                                     return ATRIB;
;                                     return SEMICOLON;
,                                     return COMMA;
"{"                                   return OBRACE;
"}"                                   return CBRACE;
"("                                   return OPAREN;
")"                                   return CPAREN;
"["                                   return OBRACT;
"]"                                   return CBRACT;
\n                                    line_counter++;
"/*"                                  {
                                        char c1, c2;
                                        do{ 
                                            c1 = yyinput();
                                            LABEL00:
                                            if (c1 == EOF) break;
                                            if (c1 == '\n') line_counter++;
                                        } 
                                        while (c1 != '*');
                                        c2 = yyinput();
                                        if(c2 == EOF) c2 = '/';
                                        if(c2 != '/') goto LABEL00;
                                      }
"*/"                                  return CCOM;
\r
\t
[ ]+
{DIGIT}+({LETTER}|{DIGIT}|{OTHER})*   return erroLexico();
{LETTER}+({LETTER}|{DIGIT}|{OTHER})*  return erroLexico();
{OTHER}+({LETTER}|{DIGIT})*{OTHER}*   return erroLexico();
%%

// int main(int argc, char *argv[])
// {
//   int token, i, buf[1000];
//   FILE *f_in;
//   FILE *exity;
 
//   for (i=0;i<=999;i++) buf[i] = 0;
  
//   /* criando arquivo de saida */
//   exity = fopen("saida.txt", "w");
//   if (exity == NULL) return 1;

//   if (argc == 2)
//   {
//     if(f_in = fopen(argv[1],"r"))  yyin = f_in;
//     else  perror(argv[0]);
//   }
//   else  yyin = stdin;
  
//   i = 0;
//   while ((token=yylex())!=END)
//   {
//     buf[i] = token;
//     i++;
//   }

//   i = 0;
//   while (buf[i] != 0)
//   {
//     switch(buf[i])
//     {
//         case COM:       fprintf(exity, "COM ");       break;
//         case NUM:       fprintf(exity, "NUM ");       break;
//         case IF:        fprintf(exity, "IF ");        break;
//         case ELSE:      fprintf(exity, "ELSE ");      break;
//         case INT:       fprintf(exity, "INT ");       break;
//         case RETURN:    fprintf(exity, "RETURN ");    break;
//         case VOID:      fprintf(exity, "VOID ");      break;
//         case WHILE:     fprintf(exity, "WHILE ");     break;
//         case ID:        fprintf(exity, "ID ");        break;
//         case ADD:       fprintf(exity, "ADD ");       break;
//         case SUB:       fprintf(exity, "SUB ");       break;
//         case MULT:      fprintf(exity, "MULT ");      break;
//         case DIV:       fprintf(exity, "DIV ");       break;
//         case SLT:       fprintf(exity, "SLT ");       break;
//         case SLTE:      fprintf(exity, "SLTE ");      break;
//         case SGT:       fprintf(exity, "SGT ");       break;
//         case SGTE:      fprintf(exity, "SGTE ");      break;
//         case EQUAL:     fprintf(exity, "EQUAL ");     break;
//         case DIFFERENT: fprintf(exity, "DIFFERENT "); break;
//         case ATRIB:     fprintf(exity, "ATRIB ");     break;
//         case SEMICOLON: fprintf(exity, "SEMICOLON\n");break;
//         case COMMA:     fprintf(exity, "COMMA ");     break;
//         case OBRACE:    fprintf(exity, "OBRACE\n");   break;
//         case CBRACE:    fprintf(exity, "CBRACE\n");   break;
//         case OPAREN:    fprintf(exity, "OPARENT ");   break;
//         case CPAREN:    fprintf(exity, "CPARENT ");   break;
//         case OBRACT:    fprintf(exity, "OBRACT ");    break;
//         case CBRACT:    fprintf(exity, "CBRACT ");    break;
//         case OCOM:      fprintf(exity, "OCOM ");      break;
//         case CCOM:      fprintf(exity, "CCOM ");      break;
//         case END:       fprintf(exity, "END ");       break;
//         case ERR:       fprintf(exity, "ERR ");       break;
//         case IGN:       break;
//         default:        fprintf(exity, "other ");
//     }
//     i++;
//   }
//   return(0);
// }

TokenType getToken(void)
{ 
    static int firstTime = TRUE;
    TokenType currentToken = 0;
    if (firstTime)
    { 
        firstTime = FALSE;
        line_counter++;
        yyin = source;
        yyout = listing;
    }
    currentToken = yylex();
    strncpy(tokenString,yytext,MAXTOKENLEN);
    if (TraceScan) {
        fprintf(listing,"\t%d: ",line_counter);
        printToken(currentToken,tokenString);
    }
    return currentToken;
}