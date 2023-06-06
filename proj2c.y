%code requires{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "proj2c.h"

extern char *yytext;

int yylex(void);


}

//Symbols
%union
{
        int iVal;       /* integer value */
        char *sVal;     /* string value */ 
        nodeType *tPtr;
};


%token <sVal> T_IDENT
%token <iVal> T_NUM
%token <iVal> T_BOOLLIT
%token T_LP
%token T_RP
%token T_ASGN
%token T_SC
%token <sVal> T_OP2
%token <sVal> T_OP3
%token <sVal> T_OP4
%token T_IF
%token T_THEN
%token T_ELSE
%token T_BGN
%token T_END
%token T_WHILE
%token T_DO
%token T_PROGRAM
%token T_VAR
%token T_AS
%token T_INT
%token T_BOOL
%token T_WRITEINT
%token T_READINT

%start Program

%type   <tPtr>  Program Declarations Type StatementSequence Statement Assignment 
                IfStatement ElseClause WhileStatement WriteInt Expression SimpleExpression
                Term Factor 
%%

/* nodeType *create_node(nodeEnum, nodeType*, nodeType*, nodeType*); */
Program:
        T_PROGRAM
        Declarations
        T_BGN
        StatementSequence T_END                 { 
                                                        nodeType *ptr = (nodeType*)malloc(sizeof(nodeType));
                                                        ptr = create_node(progType, $2, $4, NULL); 
                                                        gencode(ptr);
                                                }
        ;       
        
Declarations:   
        /* empty */                             { $$ = (nodeType*)NULL; } 
        | T_VAR T_IDENT T_AS                          
        Type                                   
        T_SC Declarations                       { 
                                                        nodeType *tmp = create_node(declType, ident($2), $4, $6);
                                                        int v = installId($2, $4->type->value);
                                                        if(v <= 0)
                                                                err(tmp, v);
                                                        $$ = tmp;
                                                }
        ;       
        
Type:   
        T_INT                                   { $$ = create_node(intType, NULL, NULL, NULL); }
        | T_BOOL                                { $$ = create_node(boolType, NULL, NULL, NULL); }
        ;       

StatementSequence:
        /* empty */                             { $$ = (nodeType*)NULL; } 
        | Statement T_SC                        
        StatementSequence                       { 
                                                        nodeType *tmp = create_node(stmtSeqType, $1, $3, NULL);
                                                        int v = validateStmt(tmp);
                                                        if(v < 0)
                                                                err(tmp, v);
                                                        $$ = tmp;
                                                }
        ;

Statement:
        Assignment                              { 
                                                        $$ = create_node(stmtType, $1, NULL, NULL);

                                                }
        | IfStatement                           {
                                                        $$ = create_node(stmtType, $1, NULL, NULL);
                                                }
        | WhileStatement                        { 
                                                        $$ = create_node(stmtType, $1, NULL, NULL);
                                                }
        | WriteInt                              { 
                                                        $$ = create_node(stmtType, $1, NULL, NULL);
                                                }
        ;

Assignment:
        T_IDENT T_ASGN                             
        Expression                              { $$ = create_node(exprAsgnStmtType, ident($1), $3, NULL); }
        | T_IDENT T_ASGN T_READINT              { $$ = create_node(readAsgnStmtType, ident($1), NULL, NULL); }
        ;

IfStatement:
        T_IF                                    
        Expression T_THEN                        
        StatementSequence ElseClause T_END      { $$ = create_node(ifStmtType, $2, $4, $5); }
        ;

ElseClause:
        /* empty */                             { $$ = (nodeType*)NULL; } 
        | T_ELSE                                
        StatementSequence                       { $$ = create_node(elseStmtType, $2, NULL, NULL); }
        ;

WhileStatement:
        T_WHILE                                
        Expression T_DO                          
        StatementSequence T_END                 { $$ = create_node(whileStmtType, $2, $4, NULL); }
        ;

WriteInt:
        T_WRITEINT                              
        Expression                              { $$ = create_node(writeStmtType, $2, NULL, NULL); }
        ;

Expression:
        SimpleExpression                        { $$ = create_node(noOpExprType, $1, NULL, NULL); } 
        | SimpleExpression T_OP4                
        SimpleExpression                        { $$ = create_node(opExprType, $1, oper($2,4), $3); }
        ;

SimpleExpression:
        Term T_OP3                               
        Term                                    { $$ = create_node(opSExprType, $1, oper($2,3), $3); }
        | Term                                  { $$ = create_node(noOpSExprType, $1, NULL, NULL); }
        ;

Term:
        Factor T_OP2
        Factor                                  { $$ = create_node(opTermType, $1, oper($2,2), $3); }
        | Factor                                { $$ = create_node(noOpTermType, $1, NULL, NULL); }
        ;

Factor:
        T_IDENT                                 { $$ = create_node(factorType, ident($1), NULL, NULL); }
        | T_NUM                                 { $$ = create_node(factorType, num($1), NULL, NULL); }   
        | T_BOOLLIT                             { $$ = create_node(factorType, boollit($1), NULL, NULL); }  
        | T_LP Expression T_RP                  { $$ = create_node(factorType, $2, NULL, NULL); }
        ;
%%


/* Generate a core dump to troubleshoot on sigsegv */
void sighandler(int signum){
	printf("Process %d got signal %d\n", getpid(), signum);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}

int yyerror(char* s) {
        printf("\n%s: %s\n",s, yytext);
        return 0;
}

int main(void){
        #if YYDEBUG == 1
        extern int yydebug;
        yydebug = 1;
        #endif
        signal(SIGSEGV, sighandler);
        yyparse();        
        printf("\nSUCCESS\n");
}


