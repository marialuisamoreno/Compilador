/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
/****************************************************/
/* File: code.h                                     */
/* Code emitting utilities for the TINY compiler    */
/* and interface to the TM machine                  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _CODE_H_
#define _CODE_H_

/* PC = program counter  */
#define  PC 7

/* mp = "memory pointer" points
 * to top of memory (for temp storage)
 */
#define  SP 6

/* gp = "global pointer" points
 * to bottom of memory for (global)
 * variable storage
 */
#define GP 5

/* accumulator */
#define  AC0 0

/* 2nd accumulator */
#define  AC1 1

/* code emitting utilities */

/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment(const char * c);

/* Procedure emitQuadruple prints a 
 * quadruple.
 */
void emitQuadruple(const char *op,const char *first, const char *second, const char *third);


#endif
