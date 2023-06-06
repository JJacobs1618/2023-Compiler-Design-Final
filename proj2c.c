#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj2c.h"
#include "uthash.h"

typedef struct hashnode {
    char id[25];        /* key */
    int type;           /* 0 = int, 1 = bool */
    UT_hash_handle hh;  /* UTHash Req. */
} hashnode;

struct hashnode *symtab = NULL;

/* indention */
int ndt; 

void indent() {
    for (int i = 0; i < ndt; i++) printf("\t");
}

void gencodeident(identNodeType *ident) {
    printf("%s", ident->value);    
}

void gencodenum(numNodeType *num) {
    printf("%d", num->value);
}

void gencodeboollit(boollitNodeType *boollit) {
    printf("%s", (boollit->value == 0 ? "false" : "true"));
}



void gencodefactor(factorNodeType *factor) {
    switch(factor->nType) {
        case identType:
            gencodeident(factor->ident);
            break;
        case numType:
            gencodenum(factor->num);
            break;
        case boollitType:
            gencodeboollit(factor->boollit);
            break;
        default:
            printf("(");
            gencodeexpr(factor->expr);
            printf(")");
            break;
    }
}

void gencodeterm(termNodeType *term) {
    switch(term->nType) {
        case noOpTermType:
            gencodefactor(term->factor1);
            break;
        case opTermType:
            gencodefactor(term->factor1);
            if(strcmp(term->oper->value, "div") == 0) printf(" / ");
            else if(strcmp(term->oper->value, "mod") == 0) printf(" %% ");
            else if(strcmp(term->oper->value, "=") == 0) printf(" == ");
            else printf(" %s ", term->oper->value);
            gencodefactor(term->factor2);
            break;
        default:
            break;
    }   
}

void gencodesexpr(sExprNodeType *sExpr) {
    switch(sExpr->nType) {
        case noOpSExprType:
            gencodeterm(sExpr->term1);
            break;
        case opSExprType:
            gencodeterm(sExpr->term1);
            if(strcmp(sExpr->oper->value, "div") == 0) printf(" / ");
            else if(strcmp(sExpr->oper->value, "mod") == 0) printf(" %% ");
            else if(strcmp(sExpr->oper->value, "=") == 0) printf(" == ");
            else printf(" %s ", sExpr->oper->value);
            gencodeterm(sExpr->term2);
            break;
        default:
            break;
    }   
}

void gencodeexpr(exprNodeType *expr) {     
    switch(expr->nType) {
        case noOpExprType:
            gencodesexpr(expr->sExpr1);
            break;
        case opExprType:
            gencodesexpr(expr->sExpr1);
            if(strcmp(expr->oper->value, "div") == 0) printf(" / ");
            else if(strcmp(expr->oper->value, "mod") == 0) printf(" %% ");
            else if(strcmp(expr->oper->value, "=") == 0) printf(" == ");
            else printf(" %s ", expr->oper->value);
            gencodesexpr(expr->sExpr2);
            break;
        default:
            break;
    }    
}

void gencodewriteint(writeIntNodeType *writeStmt) {
    indent();
    printf("printf(\"%%d\\n\", ");
    gencodeexpr(writeStmt->expr);
    printf(");\n");
}

void gencodewhile(whileStmtNodeType *whileStmt) {
    indent();
    printf("while (");
    gencodeexpr(whileStmt->expr);
    printf(") {\n");
    ndt++;
    if (whileStmt->seq)
        gencodeseq(whileStmt->seq);
    ndt--;
    indent();
    printf("}\n");
}

void gencodeelse(elseStmtNodeType *elseStmt) {
    indent();
    printf("} else");
    int elif = 0;
    if(elseStmt->seq){ /* Empty StatementSeq */
        if(elseStmt->seq->stmt->nType == ifStmtType) { // If Else Stmt
            elif = 1;
            printf(" if (");
            gencodeexpr(elseStmt->seq->stmt->ifStmt->expr);
            printf(")");        
        }
    }
    printf(" {\n");
    ndt++;
    switch(elif) {
        case 0:
            gencodeseq(elseStmt->seq);
            break;
        case 1:
            gencodeseq(elseStmt->seq->stmt->ifStmt->seq);
            break;
        default:
            break;
    }   
    ndt--;
}

void gencodeif(ifStmtNodeType *ifStmt) {
    indent();
    printf("if (");
    gencodeexpr(ifStmt->expr);
    printf(") {\n");

    ndt++;
    if (ifStmt->seq)                    // Standard if
        gencodeseq(ifStmt->seq);
    if (ifStmt->elseStmt) {             // Handles else-if
        ndt--;                          // Maintains indentation
        gencodeelse(ifStmt->elseStmt); 
        ndt++;
    }       
    ndt--;

    indent();   
    printf("}\n");
}

void gencodeasgn(asgnStmtNodeType *asgn) {
    indent();
    switch(asgn->nType) {
        case exprAsgnStmtType:
            printf("%s = ", asgn->ident->value);
            gencodeexpr(asgn->expr);
            printf(";\n");
            break; 
        case readAsgnStmtType:            
            printf("scanf(\"%%d\", &%s);\n", asgn->ident->value);
            break;
        default:
            break;
    }
}

void gencodeseq(stmtSeqNodeType *seq) {
    if(seq == NULL)
        return;
    switch(seq->stmt->nType) {
        case asgnStmtType:                          // Updates Indentation
            gencodeasgn(seq->stmt->asgnStmt);
            break;
        case ifStmtType:                            // Updates Indentation
            gencodeif(seq->stmt->ifStmt);
            break;
        case whileStmtType:                         // Updates Indentation
            gencodewhile(seq->stmt->whileStmt);
            break;
        case writeStmtType:
            gencodewriteint(seq->stmt->writeStmt);
            break;
        default:
            break;
    }
    if (seq->seq)
        gencodeseq(seq->seq);
}

void gencodedecl(declNodeType *decl) {
    indent();
    printf("%s", (decl->type->value == 0 ? "int " : "bool "));
    printf("%s = %s;\n", decl->ident->value, (decl->type->value == 0 ? "0" : "false"));

    if(decl->decl)
        gencodedecl(decl->decl);
}

void gencode(nodeType *tPtr) {
    
    printf("#include <stdio.h>\n\n");
    printf("int main() { \n");
    ndt++;                                          // Updates Indentation
    if(tPtr->prog->decl)
        gencodedecl(tPtr->prog->decl);
    printf("\n");
    if(tPtr->prog->seq)
        gencodeseq(tPtr->prog->seq);
    ndt--;                                          //decrease indentation. This should be at 0 at this point
    printf("} \n");    
}

/* Hash table Functions */

/* installId */
/* Installs the provided ID in the symbol table with the specified type */
/* Types: 0 == int, 1 == boollit */
/* ErrorDetection: Redeclarations are caught here and output provided*/
int installId(char *id, int type) {
    struct hashnode *h;

    HASH_FIND_STR(symtab, id, h);
    if (h == NULL) {
        h = (struct hashnode*)malloc(sizeof *h);
        strncpy(h->id, id, sizeof(char*));
        HASH_ADD_STR(symtab, id, h);
        h->type = type;
        return 1;
    }
    /* ID Found in Symbol Table */
    else 
        return -1;
}

struct hashnode *findId(char *id) {
    struct hashnode *h;
    HASH_FIND_STR(symtab, id, h);
    /* Id not found in Symbol Table. Invalid Declaration */
    if (h != NULL) 
        return h;
    else
        return NULL;
}

int findType(char *id) {
    struct hashnode *h;
    HASH_FIND_STR(symtab, id, h);
    if (h != NULL) 
        return h->type;
    else
        return -1;
}

void deleteId(struct hashnode *entry) {
    HASH_DEL(symtab, entry);
    free(entry);
}

void deleteAll() {
    struct hashnode *curr;
    struct hashnode *tmp;

    HASH_ITER(hh, symtab, curr, tmp) {
        HASH_DEL(symtab, curr);
        free(curr);
    }
}

void printIds() {
    struct hashnode *h;
    for (h = symtab; h != NULL; h = (struct hashnode*)(h->hh.next)) {
        printf("Id: %s\nType: %s\n",h->id, (h->type == 0) ? "int" : "bool");
    }
}

int validateExprAsgn(struct hashnode *h, int type) {
    if(h->type == type)
        return 1;
    else
        return -1;
}

int validateInt(struct hashnode *h) {
    if(h->type != 0) 
        return -1;
    else
        return 1;
}


/* return expression type 0 = int, 1 = bool. -1 implies error */
/* validates expression operations */

int validateExpr(exprNodeType *t_expr) {
    int retVal = -1;
    
    if(t_expr->nType == noOpExprType){
        if(t_expr->sExpr1->nType == noOpSExprType) {
            if(t_expr->sExpr1->term1->nType == noOpTermType) {
                switch(t_expr->sExpr1->term1->factor1->nType){
                    case identType:
                        retVal = findType(t_expr->sExpr1->term1->factor1->ident->value);
                        if(retVal == -1)
                            retVal = -10;
                        return retVal;
                    case numType:
                        retVal = 0;
                        if(t_expr->sExpr1->term1->factor1->num->value < 0) { /*implies both negative and overflow (higher than 2147483647)*/
                            retVal = -77;
                            return retVal;
                        }
                        return retVal;
                    case boollitType:
                        retVal = 1;
                        return retVal;
                    case exprType:
                        retVal = validateExpr(t_expr->sExpr1->term1->factor1->expr);
                        return retVal;
                    default:  /*Should be expr.*/  
                        break;
                }
            } else {
                /* Factor OP2 Factor: Returns Int */
                /* Verify both sides are integers */
                /* left */
                int lt = -1;
                switch(t_expr->sExpr1->term1->factor1->nType) {
                    case identType:
                        lt = findType(t_expr->sExpr1->term1->factor1->ident->value);
                        break;
                    case numType:
                        lt = 0;
                        break;
                    case boollitType:
                        lt = -1;
                        break;
                    default:
                        lt = validateExpr(t_expr->sExpr1->term1->factor1->expr);
                        break;
                }
                /* right */
                int rt = -1;
                switch(t_expr->sExpr1->term1->factor2->nType) {
                    case identType:
                        rt = findType(t_expr->sExpr1->term1->factor2->ident->value);
                        break;
                    case numType:
                        rt = 0;
                        break;
                    case boollitType:
                        rt = -1;
                        break;
                    default:
                        rt = validateExpr(t_expr->sExpr1->term1->factor2->expr);
                        break;
                }
                if(lt != 0 || rt != 0) { /* There's a bool on either side of the statement. OP4 must be integers */
                    retVal = -2;
                } else
                    retVal = 0;
            }
        } else {
            /*Term OP3 Term: Returns Int */
            /* Verify both sides are integers */
            /* left */
            int lt = -1;
            switch(t_expr->sExpr1->term1->factor1->nType) {
                case identType:
                    lt = findType(t_expr->sExpr1->term1->factor1->ident->value);
                    break;
                case numType:
                    lt = 0;
                    break;
                case boollitType:
                    lt = -1;
                    break;
                default:
                    lt = validateExpr(t_expr->sExpr1->term1->factor1->expr);
                    break;
            }
            /* right side */
            int rt = -1;
            switch(t_expr->sExpr1->term2->factor1->nType) {
                case identType:
                    rt = findType(t_expr->sExpr1->term2->factor1->ident->value);
                    break;
                case numType:
                    rt = 0;
                    break;
                case boollitType:
                    rt = -1;
                    break;
                default:
                    rt = validateExpr(t_expr->sExpr1->term2->factor1->expr);
                    break;
            }
            if(lt != 0 || rt != 0) { /* There's a bool on either side of the statement. OP4 must be integers */
                retVal = -3;
            } else
                retVal = 0;
        }
    } else {
        /* sExpr OP4 sExpr: Returns Bool */ 
        /* Verify both sides are integers */
        /* left side */
        int lt = -1;
        switch(t_expr->sExpr1->term1->factor1->nType) {
            case identType:
                lt = findType(t_expr->sExpr1->term1->factor1->ident->value);
                break;
            case numType:
                lt = 0;
                break;
            case boollitType:
                lt = -1;
                break;
            default:
                lt = validateExpr(t_expr->sExpr1->term1->factor1->expr);
                break;
        }
        /* right side */
        int rt = -1;
        switch(t_expr->sExpr2->term1->factor1->nType) {
            case identType:
                rt = findType(t_expr->sExpr2->term1->factor1->ident->value);
                break;
            case numType:
                rt = 0;
                break;
            case boollitType:
                rt = -1;
                break;
            default:
                rt = validateExpr(t_expr->sExpr2->term1->factor1->expr);
                break;
        }
        /* Factor Checks */     /* Default: Expression is Bool (-1) */
        switch(lt) {
            case -1:            /* Left Side is not in the Symbol Table */
                switch(rt) {
                    case -1:    /* Right Side is not in the Symbol Table */
                        retVal = -2;
                        break;
                    case 0:     /* Right Side is an Integer */
                        retVal = -3;
                        break;
                    case 1:     /* Right Side is a Bool */
                        retVal = -4;
                        break;
                    default:    /* Unhandled Error. TODO when this appears. */
                        retVal = -411; 
                        break;
                }
                break;
            case 0:             /* Left Side is an Integer */
                switch(rt) {
                    case -1:    /* Right Side is not in the Symbol Table */
                        retVal = -5;
                        break;
                    case 0:     /* Right Side is an Integer */
                        /* Expression is valid, check oper type */
                        switch(t_expr->oper->type){
                            case 2: /* Integer return */
                                retVal = 0;
                                break;
                            case 3: /* Integer return */
                                retVal = 0;
                                break;
                            case 4:  /* Integer return */
                                retVal = 1;
                                break;
                            default:
                                retVal = -411;
                                break;
                        }                        
                        break;
                    case 1:     /* Right Side is a Bool */
                        retVal = -6;
                        break;
                    default:    /* Unhandled Error. TODO when this appears. */
                        retVal = -411; 
                        break;
                }
                break;
            case 1:             /* Left Side is a Bool */
                switch(rt) {
                    case -1:    /* Right Side is not in the Symbol Table */
                        retVal = -7;
                        break;
                    case 0:     /* Right Side is an Integer */
                        retVal = -8;
                        break;
                    case 1:     /* Right Side is a Bool */
                        retVal = -9;
                        break;
                    default:    /* Unhandled Error. TODO when this appears. */
                        retVal = -411; 
                        break;
                }
                break;
            default:            /* Unhandled Error. TODO when this appears. */
                retVal = -411;   
                break;                
        }
    }
    return retVal;              
}

int validateAsgn(asgnStmtNodeType *asgn) {
    int retVal = -1;    
    int idType = findType(asgn->ident->value);
    if(idType == -1) { /* Error. This is an undeclared variable. */
        retVal = -2;
        return retVal;  
    }  

    switch(asgn->nType) {
        case exprAsgnStmtType:
            int exprType = validateExpr(asgn->expr);
            if(exprType == -5) { /* This is a number constant out of range */
                retVal = exprType;
            }
            if(idType == exprType)
                retVal = 1;
            break;
        case readAsgnStmtType:
            if(idType != 0) { /*Error. This is a bool. */
                retVal = -3;
            }
            else 
                retVal = 0;
            break;
        default:
            break;
    }
    return retVal;
}

int validateIf(ifStmtNodeType *ifStmt) {
    return validateExpr(ifStmt->expr);
}

int validateStmt(nodeType *n) {
    int retVal = -1;
    switch(n->seq->stmt->nType) {
        case asgnStmtType:
            retVal = validateAsgn(n->seq->stmt->asgnStmt);
            break;
        case ifStmtType:
            retVal = validateIf(n->seq->stmt->ifStmt);
            break;
        case whileStmtType:
            retVal = validateExpr(n->seq->stmt->whileStmt->expr);
            if (retVal == 0) /* Error. This is an integer expression */
                retVal = -1;
            break;
        case writeStmtType:
            retVal = validateExpr(n->seq->stmt->writeStmt->expr);
            if (retVal == 1) /* Error. This is an boolean expression */
                retVal = -1;
            break;
        default: /**/
            break;                         
    }
    return retVal;
}



int err(nodeType *n, int t){
    const size_t arraySize = 256;
    char buf[arraySize];
    memset(&(buf[0]), 0, arraySize);
    if(!n) { /* Number constant defined outside of size of integer. From Lexer */ 
        strcpy(buf, "NumberConstantDeclaration: Number declarations must be between 0-2147483647. (Displayed Output does not match file.)");
        printf("%s\n", buf);
        return 0;
    }
    switch(n->nType) {
        case declType:
            strcpy(buf, "VariableRedeclaration: ");
            strcat(buf, n->decl->ident->value);
            strcat(buf, " has already been declared.");
            
            break;
        case stmtSeqType:
            switch(n->seq->stmt->nType) {
                case asgnStmtType:
                    switch(t) {
                        case -5: /* Out of Range Integer */
                            strcpy(buf, "NumberConstantDeclaration: Number declarations must be between 0-2147483647.(Displayed Output does not match file.)");
                            break;
                        case -2: /* Undeclared Variable */
                            strcpy(buf, "UndeclaredVariable: ");
                            strcat(buf, n->seq->stmt->asgnStmt->ident->value);
                            strcat(buf, " must be declared before use.");
                            break;
                        case -3: /* Bad Read Assignment */
                            strcpy(buf, "BadReadAssignment: Variable used to store read integer is of type bool.");
                            break;
                        case -1: /* TypeMismatch */
                            strcpy(buf, "TypeMismatch: ");
                            strcat(buf, n->seq->stmt->asgnStmt->ident->value);
                            strcat(buf, " is of type ");
                            strcat(buf, (findType(n->seq->stmt->asgnStmt->ident->value)) == 0 ? "int." : "bool.");
                            break;
                        default: 
                            break;
                    }
                    break;
                case ifStmtType:
                    switch(t){
                        case -411:   /* Unhandled Error Type */
                            strcpy(buf, "BadIfStatement: This is wrong, but we can't say why yet. TODO.");
                            break;
                        case -77:
                            strcpy(buf, "BadIfStatement: Integer value provided is very wrong...");    
                            break;
                        case -11:   /* Value passed */
                            strcpy(buf, "BadIfStatement: Variable is undeclared.");    
                            break;
                        case -10:   /* Single variable statement undeclared */
                            strcpy(buf, "BadIfStatement: Variable is undeclared.");    
                            break;
                        case -9:    /* Left side is bool, right is a bool */
                            strcpy(buf, "BadIfStatement: Both sides of expression are of type bool.");    
                            break;
                        case -8:    /* Left side is bool, right is an integer */
                            strcpy(buf, "BadIfStatement: Left value of expression is of type bool.");    
                            break;
                        case -7:    /* Left side is bool, right not declared. */
                            strcpy(buf, "BadIfStatement: Left values of expression is of type bool, right variable undeclared.");    
                            break;
                        case -6:    /* Left side is an int, right is bool */
                            strcpy(buf, "BadIfStatement: Right value of expression is of type bool.");    
                            break;
                        case -5:    /* Left side is an int, right is undeclared */
                            strcpy(buf, "BadIfStatement: Right variable must be declared before use.");    
                            break;
                        case -4:    /* Left not declared, right is bool */
                            strcpy(buf, "BadIfStatement: Left variable undeclared, right value of expression is of type bool.");
                            break;
                        case -3:    /* Left variable has not been declared */
                            strcpy(buf, "BadIfStatement: Left variable must be declared before use.");
                            break;
                        case -2:    /* Neither variable has been declared */
                            strcpy(buf, "BadIfStatement: Neither variable has be declared before use.");
                            break;
                        case -1:    /* Expression is Boolean */
                            strcpy(buf, "BadIfStatement: Expression evaluation must be of type bool.");
                            break;
                        default:
                            break;
                    }   
                    break;
                case whileStmtType:
                    switch(t){
                        case -411:   /* Unhandled Error Type */
                            strcpy(buf, "BadWhileStatement: This is wrong, but we can't say why yet. TODO.");
                            break;
                        case -77:
                            strcpy(buf, "BadWhileStatement: Integer value provided is very wrong...");    
                            break;
                        case -11:   /* Value passed */
                            strcpy(buf, "BadWhileStatement: Variable is undeclared.");    
                            break;
                        case -10:   /* Single variable statement undeclared */
                            strcpy(buf, "BadWhileStatement: Variable is undeclared.");    
                            break;
                        case -9:    /* Left side is bool, right is a bool */
                            strcpy(buf, "BadWhileStatement: Both sides of expression are of type bool.");    
                            break;
                        case -8:    /* Left side is bool, right is an integer */
                            strcpy(buf, "BadWhileStatement: Left value of expression is of type bool.");    
                            break;
                        case -7:    /* Left side is bool, right not declared. */
                            strcpy(buf, "BadWhileStatement: Left values of expression is of type bool, right variable undeclared.");    
                            break;
                        case -6:    /* Left side is an int, right is bool */
                            strcpy(buf, "BadWhileStatement: Right value of expression is of type bool.");    
                            break;
                        case -5:    /* Left side is an int, right is undeclared */
                            strcpy(buf, "BadWhileStatement: Right variable must be declared before use.");    
                            break;
                        case -4:    /* Left not declared, right is bool */
                            strcpy(buf, "BadWhileStatement: Left variable undeclared, right value of expression is of type bool.");
                            break;
                        case -3:    /* Left variable has not been declared */
                            strcpy(buf, "BadWhileStatement: Left variable must be declared before use.");
                            break;
                        case -2:    /* Neither variable has been declared */
                            strcpy(buf, "BadWhileStatement: Neither variable has be declared before use.");
                            break;
                        case -1:    /* Expression is Boolean */
                            strcpy(buf, "BadWhileStatement: Expression evaluation must be of type bool.");
                            break;
                        default:
                            break;
                    }
                    break;
                case writeStmtType:
                    switch(t){
                        case -411:   /* Unhandled Error Type */
                            strcpy(buf, "BadWriteStatement: This is wrong, but we can't say why yet. TODO.");
                            break;
                        case -77:
                            strcpy(buf, "BadWriteStatement: Integer value provided is very wrong...");    
                            break;
                        case -11:   /* Value passed */
                            strcpy(buf, "BadWriteStatement: Variable is undeclared.");    
                            break;
                        case -10:   /* Single variable statement undeclared */
                            strcpy(buf, "BadWriteStatement: Variable is undeclared.");    
                            break;
                        case -9:    /* Left side is bool, right is a bool */
                            strcpy(buf, "BadWriteStatement: Both sides of expression are of type bool.");    
                            break;
                        case -8:    /* Left side is bool, right is an integer */
                            strcpy(buf, "BadWriteStatement: Left value of expression is of type bool.");    
                            break;
                        case -7:    /* Left side is bool, right not declared. */
                            strcpy(buf, "BadWriteStatement: Left values of expression is of type bool, right variable undeclared.");    
                            break;
                        case -6:    /* Left side is an int, right is bool */
                            strcpy(buf, "BadWriteStatement: Right value of expression is of type bool.");    
                            break;
                        case -5:    /* Left side is an int, right is undeclared */
                            strcpy(buf, "BadWriteStatement: Right variable must be declared before use.");    
                            break;
                        case -4:    /* Left not declared, right is bool */
                            strcpy(buf, "BadWriteStatement: Left variable undeclared, right value of expression is of type bool.");
                            break;
                        case -3:    /* Left variable has not been declared */
                            strcpy(buf, "BadWriteStatement: Left variable must be declared before use.");
                            break;
                        case -2:    /* Neither variable has been declared */
                            strcpy(buf, "BadWriteStatement: Neither variable has be declared before use.");
                            break;
                        case -1:    /* Expression is an bool */
                            strcpy(buf, "BadWriteStatement: Expression evaluation must be of type int.");
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    printf("%s\n", buf);
    return 0;
}

nodeType *create_node(nodeEnum nType, nodeType* n1, nodeType* n2, nodeType* n3){
        nodeType *n;
        if ((n = (nodeType*)malloc(sizeof(nodeType))) == NULL) yyerror("CreateNode: Out Of Memory\n");

        switch(nType)
        {
                case progType:
                        if ((n->prog = (progNodeType*)malloc(sizeof(progNodeType))) == NULL) yyerror("Program: Out Of Memory\n");
                                                
                        n->nType = nType;
                        if(n1) 
                            n->prog->decl = n1->decl;
                        if(n2) 
                            n->prog->seq = n2->seq;

                        break;
                case declType:
                        if ((n->decl = (declNodeType*)malloc(sizeof(declNodeType))) == NULL) yyerror("Declaration: Out Of Memory\n");

                        n->nType = nType;
                        n->decl->nType = nType;
                        n->decl->ident = n1->ident;
                        n->decl->type = n2->type;
                        if(n3) 
                            n->decl->decl = n3->decl;

                        break;
                case intType:
                        if ((n->type = (typeNodeType*)malloc(sizeof(typeNodeType))) == NULL) yyerror("Type[Int]: Out Of Memory\n");

                        n->nType = typeType;
                        n->type->nType = nType;
                        n->type->value = 0;

                        break;
                case boolType:
                        if ((n->type = (typeNodeType*)malloc(sizeof(typeNodeType))) == NULL) yyerror("Type[Bool]: Out Of Memory\n");
                        
                        n->nType = typeType;
                        n->type->nType = nType;
                        n->type->value = 1;

                        break;
                case stmtSeqType:
                        if ((n->seq = (stmtSeqNodeType*)malloc(sizeof(stmtSeqNodeType))) == NULL) yyerror("StatementSequence: Out of Memory\n");                      
                        
                        n->nType = nType;
                        n->seq->nType = nType;
                        n->seq->stmt = n1->stmt;
                        if(n2) 
                            n->seq->seq = n2->seq;

                        break;
                case stmtType:
                        if ((n->stmt = (stmtNodeType*)malloc(sizeof(stmtNodeType))) == NULL) yyerror("Statement: Out of Memory\n");
                        
                        n->nType = stmtType;
                        n->stmt->nType = n1->nType;
                        
                        switch(n1->nType)
                        {
                                case asgnStmtType:
                                    n->stmt->asgnStmt = n1->asgnStmt;
                                    break;
                                case ifStmtType:
                                    n->stmt->ifStmt = n1->ifStmt;
                                    break;
                                case whileStmtType:
                                    n->stmt->whileStmt = n1->whileStmt;
                                    break;
                                case writeStmtType:
                                    n->stmt->writeStmt = n1->writeStmt;
                                    break;
                                default:
                                    printf("Default. Unhandled. Somethin' went wrong.\n");
                                    break;                         
                        }

                        break;
                case ifStmtType:
                        if ((n->ifStmt = (ifStmtNodeType*)malloc(sizeof(ifStmtNodeType))) == NULL) yyerror("IfStatement: Out of Memory\n");
                        
                        n->nType = nType;
                        n->ifStmt->nType = nType;
                        n->ifStmt->expr = n1->expr;
                        if(n2) 
                            n->ifStmt->seq = n2->seq;
                        if(n3) 
                            n->ifStmt->elseStmt = n3->elseStmt;

                        break;
                case whileStmtType:  
                        if ((n->whileStmt = (whileStmtNodeType*)malloc(sizeof(whileStmtNodeType))) == NULL) yyerror("WhileStatement: Out Of Memory\n");
                        
                        n->nType = nType;
                        n->whileStmt->expr = n1->expr;
                        if(n2) 
                            n->whileStmt->seq = n2->seq;

                        break;
                case exprAsgnStmtType:
                        if ((n->asgnStmt = (asgnStmtNodeType*)malloc(sizeof(asgnStmtNodeType))) == NULL) yyerror("AssignmentStatement (Expression): Out Of Memory\n");

                        n->nType = asgnStmtType;
                        n->asgnStmt->nType = nType;
                        n->asgnStmt->ident = n1->ident;
                        n->asgnStmt->expr = n2->expr;

                        break;
                case readAsgnStmtType:
                        if ((n->asgnStmt = (asgnStmtNodeType*)malloc(sizeof(asgnStmtNodeType))) == NULL) yyerror("AssignmentStatement (Read): Out Of Memory\n");

                        n->nType = asgnStmtType;
                        n->asgnStmt->nType = nType;
                        n->asgnStmt->ident = n1->ident;

                        break;
                case elseStmtType:
                        if ((n->elseStmt = (elseStmtNodeType*)malloc(sizeof(elseStmtNodeType))) == NULL) yyerror("ElseClause: Out Of Memory\n");
                
                        n->nType = nType;
                        n->elseStmt->nType = nType;

                        if(n1) 
                            n->elseStmt->seq = n1->seq;               

                        break;
                case writeStmtType: 
                        if ((n->writeStmt = (writeIntNodeType*)malloc(sizeof(writeIntNodeType))) == NULL) yyerror("WriteIntStatement: Out Of Memory\n");

                        n->nType = nType;
                        n->writeStmt->expr = n1->expr;

                        break;
                case noOpExprType:
                        if ((n->expr = (exprNodeType*)malloc(sizeof(exprNodeType))) == NULL) yyerror("NoOpExprStatement: Out Of Memory\n");

                        n->nType = exprType;      
                        n->expr->nType = nType;
                        n->expr->sExpr1 = n1->sExpr;

                        break;
                case opExprType:                                        
                        if ((n->expr = (exprNodeType*)malloc(sizeof(exprNodeType))) == NULL) yyerror("OpExprStatement: Out Of Memory\n");
                        
                        n->nType = exprType;      
                        n->expr->nType = nType;
                        n->expr->sExpr1 = n1->sExpr;
                        n->expr->oper = n2->oper;
                        n->expr->sExpr2 = n3->sExpr;
                        
                        break;
                case noOpSExprType:
                        if ((n->sExpr = (sExprNodeType*)malloc(sizeof(sExprNodeType))) == NULL) yyerror("NoOpSExprStatement: Out Of Memory\n");

                        n->nType = sExprType;       
                        n->sExpr->nType = nType;
                        n->sExpr->term1 = n1->term;
                        
                        break;
                case opSExprType:
                        if ((n->sExpr = (sExprNodeType*)malloc(sizeof(sExprNodeType))) == NULL) yyerror("OpSExprStatement: Out Of Memory\n");

                        n->nType = sExprType;       
                        n->sExpr->nType = nType;
                        n->sExpr->term1 = n1->term;
                        n->sExpr->oper = n2->oper;
                        n->sExpr->term2 = n3->term;

                        break;
                case noOpTermType:
                        if ((n->term = (termNodeType*)malloc(sizeof(termNodeType))) == NULL) yyerror("NoOpTermStatement: Out Of Memory\n");

                        n->nType = termType;       
                        n->term->nType = nType;
                        n->term->factor1 = n1->factor;
                        break;
                case opTermType:
                        if ((n->term = (termNodeType*)malloc(sizeof(termNodeType))) == NULL) yyerror("OpTermStatement: Out Of Memory\n");

                        n->nType = termType;       
                        n->term->nType = nType;
                        n->term->factor1 = n1->factor;
                        n->term->oper = n2->oper;
                        n->term->factor2 = n3->factor;

                        break;
                case factorType:
                        if ((n->factor = (factorNodeType*)malloc(sizeof(factorNodeType))) == NULL) yyerror("Factor: Out Of Memory\n");
                        
                        n->nType = n1->nType;
                        n->factor->nType = n1->nType;

                        switch(n1->nType)
                        {
                            case identType:
                                n->factor->ident = n1->ident;
                                break;
                            case numType:
                                n->factor->num = n1->num;
                                break;
                            case boollitType: 
                                n->factor->boollit = n1->boollit;
                                break;
                            case exprType:
                                n->factor->expr = n1->expr;
                                break;            
                            default:
                                printf("\n\nDefault. Unhandled\n\n");
                                break;                            
                        } 
                        break;
                default:
                    printf("Default. Unhandled\n");
                    break;         
                        
        }
        return n;
}

nodeType *ident(char *value) {
        nodeType *i;

        if ((i = (nodeType*)malloc(sizeof(nodeType))) == NULL) yyerror("IdentNodeCreation: Out Of Memory\n");
        if ((i->ident = (identNodeType*)malloc(sizeof(identNodeType))) == NULL) yyerror("IdentCreation: Out Of Memory\n");
        
        i->nType = identType;     
        i->ident->value = strdup(value);
        return i;
}

nodeType *num(int value) {
        nodeType *n;

        if ((n = (nodeType*)malloc(sizeof(nodeType))) == NULL) yyerror("NumNodeCreation: Out Of Memory\n");
        if ((n->num = (numNodeType*)malloc(sizeof(numNodeType))) == NULL) yyerror("NumCreation: Out Of Memory\n");
        
        n->nType = numType;
        n->num->value = value;

        return n;
}

nodeType *boollit(int value) {
        nodeType *b;

        if ((b = (nodeType*)malloc(sizeof(nodeType))) == NULL) yyerror("BoollitNodeCreation: Out Of Memory\n");
        if ((b->boollit = (boollitNodeType*)malloc(sizeof(boollitNodeType))) == NULL) yyerror("BoollitCreation: Out Of Memory\n");
        
        b->nType = boollitType;
        b->boollit->value = value;
        return b;
}

nodeType *oper(char *value, int type) {
        nodeType *o;
        nodeEnum t;

        if ((o = (nodeType*)malloc(sizeof(nodeType))) == NULL) yyerror("OperNodeCreation: Out Of Memory\n");
        if ((o->oper = (operNodeType*)malloc(sizeof(operNodeType))) == NULL) yyerror("OperCreation: Out Of Memory\n");
        

        switch(type) {
                case 2:
                        t = oper2Type;
                        break;
                case 3:
                        t = oper3Type;
                        break;
                case 4:
                        t = oper4Type;
                        break;
                default:
                        break;
        }
        o->nType = t;
        o->oper->value = strdup(value);
        o->oper->type = type;             /* 2 = OP2, 3 = OP3, 4 = OP4 */

        return o;
}