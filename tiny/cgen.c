/*  
    Andre Lucas 112175 
    Maria Luisa 111859
*/
/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

char number[11];
int cont_aux = 0, cont_lab = 0, cont_vet = 0;

void intToString(char *str, int v, int n){
    int i, value, cont = 0;
    value = v;
    str[0] = '0';
    str[1] = '\0';
    for (i = 0; i < n; i++){
        if (value == 0)
            break;
        value /= 10;
        cont++;
    }
    for (i = cont - 1; i > -1; i--){
        str[i] = '0' + (v % 10);
        v /= 10;
    }
    if (cont > 0)
        str[cont] = '\0';
}

char *strNumber(const char *str, int number){
    char num[11];
    char *rt;
    int a, b;
    a = strlen(str);
    intToString(num, number, 10);
    b = strlen(num);
    rt = (char *)malloc(sizeof(char) * (a + b));
    memcpy(rt, str, a * sizeof(char));
    memcpy(rt + a * sizeof(char), num, b * sizeof(char));
    return rt;
}

/* prototype for internal recursive code generator */
static void cGen(TreeNode *tree, char **operate);

static void ccGen(TreeNode *tree, char **operate);

/* Procedure genStmt generates code at a statement node */
static void genStmt(TreeNode *tree, char **operate){
    TreeNode *p1, *p2, *p3;
    char *op[3] = {NULL, NULL, NULL};
    char *label1, *label2;
    switch (tree->kind.stmt){
    case IfK:
        if (TraceCode)
            emitComment("-> if");
        p1 = tree->child[0];
        p2 = tree->child[1];
        p3 = tree->child[2];
        /* generate code for test expression */
        label1 = strNumber("LABEL", cont_lab);
        cont_lab++;
        cGen(p1, &op[0]);
        emitQuadruple("IF_NOT", op[0], label1, "-");
        /* recurse on then part */
        cGen(p2, &op[1]);
        if(p3){
            label2 = strNumber("LABEL", cont_lab);
            cont_lab++;
            emitComment("if: jump to end belongs here");
            emitQuadruple("JUMP", label2, "-", "-");
        }
        emitQuadruple("LABEL", label1, "-", "-");
        if(p3){
            /* recurse on else part */
            cGen(p3, &op[1]);
            emitQuadruple("LABEL", label2, "-", "-");
        }
        if (TraceCode)
            emitComment("<- if");
        break; /* if_k */

    case WhileK:
        if (TraceCode)
            emitComment("-> repeat");
        p1 = tree->child[0];
        p2 = tree->child[1];
        emitComment("repeat: jump after body comes back here");
        /* generate code for body */
        label1 = strNumber("LABEL", cont_lab);
        cont_lab++;
        label2 = strNumber("LABEL", cont_lab);
        cont_lab++;
        emitQuadruple("LABEL", label1, "-", "-");
        cGen(p1, &op[0]);
        emitQuadruple("IF_NOT", op[0], label2, "-");
        /* generate code for test */
        cGen(p2, &op[1]);
        emitQuadruple("JUMP", label1, "-", "-");
        emitQuadruple("LABEL", label2, "-", "-");
        if (TraceCode)
            emitComment("<- repeat");
        break; /* repeat */

    case AssignK:
        if (TraceCode)
            emitComment("-> assign");
        /* generate code for rhs */
        cGen(tree->child[0], &op[0]);
        op[1] = op[0];
        cGen(tree->child[1], &op[1]);
        if (strcmp(op[0], op[1]) != 0)
            emitQuadruple("ASSIGN", op[0], op[1], "-");
        /* now store value */
        //emitQuadruple("ASSIGN", "-", "-", "-");
        if (TraceCode)
            emitComment("<- assign");
        *operate = op[0];
        break; /* assign_k */
    case ReturnK:
        if (TraceCode)
            emitComment("-> return");
        cGen(tree->child[0], &op[0]);
        emitQuadruple("JR", op[0], "-", "-");
        if (TraceCode)
            emitComment("<- return");
    default:
        break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp(TreeNode *tree, char **operate){
    TreeNode *p1, *p2;
    char *op[3] = {NULL, NULL, NULL};
    switch (tree->kind.exp){
    case ConstK:
        if (TraceCode)
            emitComment("-> Const");
        intToString(number, tree->attr.val, 10);
        *operate = number;
        if (TraceCode)
            emitComment("<- Const");
        break; /* ConstK */

    case IdK:
        if (TraceCode)
            emitComment("-> Id");
        *operate = tree->attr.name;
        if (tree->func){
            if (tree->decl){
                op[0] = tree->attr.name;
                if(tree->type == 0){
                    emitQuadruple("FUNC", "VOID", op[0], "-");
                }
                else{
                    emitQuadruple("FUNC", "INT", op[0], "-");
                }
                TreeNode *p = tree->child[0];
                while (p){
                    ccGen(p->child[0], &op[0]);
                    emitQuadruple("LOAD", op[0], "-", "-");
                    p = p->sibling;
                }
                cGen(tree->child[1], &op[1]);
            }
            else{
                TreeNode *p = tree->child[0];
                int cont = 0;
                while (p){
                    ccGen(p, &op[0]);
                    emitQuadruple("PARAM", op[0], "-", "-");
                    p = p->sibling;
                    cont++;
                }
                intToString(number, cont, 10);
                op[0] = tree->attr.name;
                op[1] = number;
                emitQuadruple("CALL", op[0], op[1], "-");
            }
        }
        else{
            if (!tree->decl){
                cGen(tree->child[0], &op[2]);
                if (tree->child[0] != NULL){
                    op[0] = strNumber("VET", cont_vet);
                    op[1] = tree->attr.name;
                    cont_vet++;
                    emitQuadruple("ADD", op[0], op[1], op[2]);
                    emitQuadruple("LOAD_ADRESS", op[0], op[0], "-");
                    *operate = op[0];
                }
            }
        }
        if (TraceCode)
            emitComment("<- Id");
        break; /* IdK */

    case OpK:
        if (TraceCode)
            emitComment("-> Op");
        p1 = tree->child[0];
        p2 = tree->child[1];
        op[0] = *operate;
        cGen(p1, &op[1]);
        cGen(p2, &op[2]);
        if (op[0] == NULL){
            op[0] = strNumber("AUX", cont_aux);
            cont_aux++;
        }
        switch (tree->attr.op){
        case ADD:
            emitQuadruple("ADD", op[0], op[1], op[2]);
            break;
        case SUB:
            emitQuadruple("SUB", op[0], op[1], op[2]);
            break;
        case MULT:
            emitQuadruple("MULT", op[0], op[1], op[2]);
            break;
        case DIV:
            emitQuadruple("DIV", op[0], op[1], op[2]);
            break;
        case SLT:
            emitQuadruple("SLT", op[0], op[1], op[2]);
            break;
        case SLTE:
            emitQuadruple("SLTE", op[0], op[1], op[2]);
            break;
        case SGT:
            emitQuadruple("SLT", op[0], op[1], op[2]);
            break;
        case SGTE:
            emitQuadruple("SGTE", op[0], op[1], op[2]);
            break;
        case EQUAL:
            emitQuadruple("EQUAL", op[0], op[1], op[2]);
            break;
        default:
            emitComment("BUG: Unknown operate");
            break;
        } /* case op */
        *operate = op[0];
        if (TraceCode)
            emitComment("<- Op");
        break; /* OpK */
    case TypeK:
        if (TraceCode)
            emitComment("-> type");
        /* generate code for rhs */
        if (tree->decl){
            op[0] = tree->child[0]->attr.name;
            if (tree->child[0]->child[0]){
                cGen(tree->child[0]->child[0], &op[1]);
                emitQuadruple("ALOC_MEN", op[0], op[1], "-");
            }
            else{
                emitQuadruple("ALOC_MEN", op[0], "1", "-");
            }
            cGen(tree->child[0], &op[1]);
        }
        /* now store value */
        if (TraceCode)
            emitComment("<- type");
        break;
    default:
        break;
    }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen(TreeNode *tree, char **operate)
{
    if (tree != NULL){
        switch (tree->nodekind){
        case StmtK:
            genStmt(tree, operate);
            break;
        case ExpK:
            genExp(tree, operate);
            break;
        default:
            break;
        }
        cGen(tree->sibling, operate);
    }
}

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void ccGen(TreeNode *tree, char **operate){
    if (tree != NULL){
        switch (tree->nodekind){
        case StmtK:
            genStmt(tree, operate);
            break;
        case ExpK:
            genExp(tree, operate);
            break;
        default:
            break;
        }
    }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode *syntaxTree, const char *codefile){
    char *s = (char *)malloc(strlen(codefile) + 7);
    char *pointer = NULL;
    strcpy(s, "File: ");
    strcat(s, codefile);
    emitComment("TINY Compilation to TM Code");
    emitComment(s);
    /* generate standard prelude */
    emitComment("Standard prelude:");

    emitComment("End of standard prelude.");
    /* generate code for TINY program */
    TreeNode *tr = syntaxTree;
    while (tr != NULL){
        if (tr->child[0]->func){
            cGen(tr->child[0], &pointer);
            fprintf(code,"\n");
        }
        else{
            cGen(tr, &pointer);
            fprintf(code,"\n");
        }
        tr = tr->sibling;
    }
    /* finish */
    emitComment("End of execution.");
}