/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
/****************************************************/
/* File: code.c                                     */
/* TM Code emitting utilities                       */
/* implementation for the TINY compiler             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "code.h"

/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment(const char * c )
{ if (TraceCode) fprintf(code,"* %s\n",c);}

/* Procedure emitQuadruple prints a 
 * quadruple.
 */
void emitQuadruple(const char *op,const char *first, const char *second, const char *third){
    fprintf(code,"(%s, %s, %s, %s)\n", op, first, second, third);
}