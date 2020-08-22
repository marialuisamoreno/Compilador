/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

int erro = 0;

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     int type;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { char * name;
     char * idName;
     int func;
     LineList lines;
     LineList atrib;
     LineList decl_line;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
   } * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, char *idName, int lineno, int decl_line, int type, int func, int atrib, int loc )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->idName = idName;
    l->func = func;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    if(decl_line > -1){
        l->decl_line = (LineList) malloc(sizeof(struct LineListRec));
        l->decl_line->lineno = decl_line;
        l->decl_line->type = type;
        l->decl_line->next = NULL;
    }
    else l->decl_line = NULL;
    if(atrib > -1){
        l->atrib = (LineList) malloc(sizeof(struct LineListRec));
        l->atrib->lineno = lineno;
        l->atrib->type = type;
        l->atrib->next = NULL;
    }
    else l->atrib = NULL;
    
    l->next = hashTable[h];
    hashTable[h] = l; }
  else /* found in table, so just add line number */
  { 
    LineList t;
    
    
    if(atrib > -1){
        LineList p;
        p = (LineList) malloc(sizeof(struct LineListRec));
        p->lineno = lineno;
        p->type = type;
        p->next = NULL;
        t = l->atrib;
        if(t != NULL){
            while (t->next != NULL) t = t->next;
            t->next = p;
        }
        else l->atrib = p;
    } 
    else{
        LineList p;
        p = (LineList) malloc(sizeof(struct LineListRec));
        p->lineno = lineno;
        p->next = NULL;
        t = l->lines;
        if(t != NULL){
            while (t->next != NULL) t = t->next;
            t->next = p;
        }
        else l->lines = p;
    }
    if(decl_line > -1){
        LineList p;
        p = (LineList) malloc(sizeof(struct LineListRec));
        p->lineno = decl_line;
        p->type = type;
        p->next = NULL;
        t = l->decl_line;
        if(t != NULL){
            while (t->next != NULL) t = t->next;
            t->next = p;
        }
        else l->decl_line = p;
    }
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i, padding = 2, count;
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { 
        count = 0;
        LineList s = l->decl_line;
        while (s != NULL){
            count++;
            s = s->next;
        }
        padding = (count > padding) ? count : padding;
        l = l->next;
      }
    }
  }
  fprintf(listing,"Variable Name        Location  Declaration    ");
  for(int j = 2; j < padding; j++) fprintf(listing, "         "); 
  fprintf(listing, "  Line Numbers\n");
  fprintf(listing,"-------------------  --------  ---------------");
  for(int j = 2; j < padding; j++) fprintf(listing, "---------");
  fprintf(listing,"  -------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { 
        LineList t = l->lines;
        LineList r = l->decl_line;
        fprintf(listing,"%-20s ",l->name);
        fprintf(listing,"%-8d  ",l->memloc);
        for(int j = 0; j < padding; j++){
            if(r != NULL){
                fprintf(listing, "%-4d(%d)|", r->lineno, r->type);
                r = r->next;
            }
            else{
                fprintf(listing, "       |");
            }
        }
        fprintf(listing, " ");
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */

void printScope(char *name, FILE *listing){
    int i = 0;
    while(name[i] != ' '){
        fprintf(listing, "%c", name[i]);
        i++;
    }
}

void notUniqueVariable(FILE * listing){ 
    int i, count = 0;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList s = l->decl_line;
                while (s != NULL){
                    count++;
                    if(count > 1){
                        fprintf(listing,"Erro semantico no escopo ");
                        printScope(l->name, listing);
                        fprintf(listing," na linha %d: declaração inválida de variável %s, já foi declarada previamente.\n",
                        s->lineno, l->idName);
                        erro = 1;
                    }
                    s = s->next;
                }
                count = 0;
                l = l->next;
            }
        }
    }
}

void notVoidVariable(FILE * listing){ 
    int i;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList s = l->decl_line;
                while (s != NULL){
                    if(!l->func && !s->type){
                        fprintf(listing,"Erro semantico no escopo ");
                        printScope(l->name, listing);
                        fprintf(listing," na linha %d: declaração inválida de variável %s, void só pode ser usado para declaração de função.\n",
                        s->lineno, l->idName);
                        erro = 1;
                    }
                    s = s->next;
                }
                l = l->next;
            }
        }
    }
}

void variableNotDeclared(FILE * listing){ 
    int i;
    char *name;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList s = l->lines;
                while (s != NULL){
                    name = idScopeName("GLOBAL", l->idName);
                    if(!l->decl_line && !l->func){
                        if(st_lookup(name) == -1){
                            fprintf(listing,"Erro semantico no escopo ");
                            printScope(l->name, listing);
                            fprintf(listing," na linha %d: variável %s não declarada.\n",
                            s->lineno, l->idName);
                            erro = 1;
                        }
                    }
                    s = s->next;
                }
                free(name);
                l = l->next;
            }
        }
    }
}

void functionNotDeclared(FILE * listing){ 
    int i;
    char *name;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList s = l->lines;
                name = idScopeName("GLOBAL", l->idName);
                while (s != NULL){
                    if(l->func && st_lookup(name) == -1 && strcmp(name, l->name) != 0){
                        fprintf(listing,"Erro semantico no escopo ");
                        printScope(l->name, listing);
                        fprintf(listing," na linha %d: função %s não declarada.\n",
                        s->lineno, l->idName);
                        erro = 1;
                    }
                    s = s->next;
                }
                free(name);
                l = l->next;
            }
        }
    }
}

void mainNotDeclared(FILE *listing){
    int i, error = 1;
    char *name;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                if(strcmp("GLOBAL main", l->name) == 0 || !erro){
                    error = 0;
                    break;
                }
                free(name);
                l = l->next;
            }
        }
    }
    if(error){
        fprintf(listing, "Erro semantico: função main() não declarada.\n");
        erro = 1;
    }
}

void variableIsFunction(FILE *listing){
    int i, j = 0, k = 0, m;
    char *name;
    BucketList func[2*SIZE];
    BucketList var[2*SIZE];
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                name = idScopeName("GLOBAL", l->idName);
                if(strcmp(name, l->name) == 0 && l->func){
                    func[j] = l;
                    j++;
                }
                else if(!l->func){
                    var[k] = l;
                    k++;
                }
                free(name);
                l = l->next;
            }
        }
        
    }
    for(i = 0; i < j; i++){
        for(m = 0; m < k; m++){
            if(strcmp(var[m]->idName, func[i]->idName) == 0){
                LineList s = var[m]->decl_line;
                while(s){
                    fprintf(listing,"Erro semantico no escopo ");
                    printScope(var[m]->name, listing);
                    fprintf(listing," na linha %d: %s já foi declarada como nome de função.\n",
                    s->lineno, var[m]->idName);
                    s = s->next;
                    erro = 1;
                }
            }
        }
    }
}

void voidAtribuition(FILE *listing){
    int i;
    for (i = 0; i < SIZE; i++){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList s = l->atrib;
                while (s != NULL){
                    if(!s->type){
                        fprintf(listing,"Erro semantico no escopo ");
                        printScope(l->name, listing);
                        fprintf(listing," na linha %d: atribuição void em %s.\n",
                        s->lineno, l->idName);
                        erro = 1;
                    }
                    s = s->next;
                }
                l = l->next;
            }
        }
    }
}

int semantical(FILE *listing){
    notUniqueVariable(listing);
    notVoidVariable(listing);
    variableNotDeclared(listing);
    functionNotDeclared(listing);
    mainNotDeclared(listing);
    variableIsFunction(listing);
    voidAtribuition(listing);
    return erro;
}