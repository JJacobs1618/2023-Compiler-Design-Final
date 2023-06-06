typedef enum {  progType, declType, typeType, intType, boolType, stmtSeqType, 
                stmtType, asgnStmtType, ifStmtType, whileStmtType, writeStmtType,
                exprAsgnStmtType, readAsgnStmtType, elseStmtType, exprType, noOpExprType, opExprType, 
                sExprType, noOpSExprType, opSExprType, termType, noOpTermType, opTermType, 
                factorType, identType, numType, boollitType, operType, emptyType, 
                oper2Type, oper3Type, oper4Type
} nodeEnum;



typedef struct progNodeType {
    nodeEnum nType;
    struct declNodeType *decl;
    struct stmtSeqNodeType *seq;
} progNodeType;

typedef struct declNodeType {
    nodeEnum nType;
    struct identNodeType *ident;
    struct typeNodeType *type;
    struct declNodeType *decl;
} declNodeType; 

typedef struct typeNodeType {
    nodeEnum nType;                 
    int value;                      /* 0 = bool, 1 = int*/
} typeNodeType;

typedef struct stmtSeqNodeType {
    nodeEnum nType;
    struct stmtNodeType *stmt;
    struct stmtSeqNodeType *seq;
} stmtSeqNodeType;


typedef struct stmtNodeType {
    nodeEnum nType;
    struct asgnStmtNodeType *asgnStmt;
    struct ifStmtNodeType *ifStmt;
    struct whileStmtNodeType *whileStmt;
    struct writeIntNodeType *writeStmt;
} stmtNodeType;

typedef struct asgnStmtNodeType {
    nodeEnum nType;                 /* exprAsgnStmtNode or readAsgnStmtNode */
    struct identNodeType *ident;    /* both use this */
    struct exprNodeType *expr;      /* only for <expression> */
} asgnStmtNodeType;

typedef struct ifStmtNodeType {
    nodeEnum nType;
    struct exprNodeType *expr;
    struct stmtSeqNodeType *seq;
    struct elseStmtNodeType *elseStmt;
} ifStmtNodeType;

typedef struct elseStmtNodeType {
    nodeEnum nType;
    struct stmtSeqNodeType *seq;
} elseStmtNodeType;

typedef struct whileStmtNodeType {
    nodeEnum nType;
    struct exprNodeType *expr;
    struct stmtSeqNodeType *seq;    
} whileStmtNodeType;

typedef struct writeIntNodeType {
    nodeEnum nType;
    struct exprNodeType *expr;
} writeIntNodeType;

typedef struct exprNodeType {
    nodeEnum nType;                 /* noOpExprNode, opExprNode */
    struct sExprNodeType *sExpr1;   /* used for both */
    struct operNodeType *oper;      /* only for expressions with operators */
    struct sExprNodeType *sExpr2;   /* only for expressions with operators */
} exprNodeType;

typedef struct sExprNodeType {
    nodeEnum nType;                 /* noOpSExprNode, opSExprNode */             
    struct termNodeType *term1;     /* used for both */
    struct operNodeType *oper;      /* only for expressions with operators */
    struct termNodeType *term2;     /* only for expressions with operators */
} sExprNodeType;

typedef struct termNodeType {
    nodeEnum nType;                 /* noOpTermNode, opTermNode */
    struct factorNodeType *factor1; /* used for both */
    struct operNodeType *oper;      /* only for expressions with operators */
    struct factorNodeType *factor2; /* only for expressions with operators */
} termNodeType;

typedef struct factorNodeType {        
    nodeEnum nType;                 
    struct identNodeType *ident;
    struct numNodeType *num;
    struct boollitNodeType *boollit;
    struct exprNodeType *expr;
} factorNodeType;

typedef struct identNodeType { 
    char *value;
} identNodeType;

typedef struct operNodeType {    
    nodeEnum nType;
    char *value;
    int type;
} operNodeType;

typedef struct numNodeType {
    int value;
} numNodeType;

typedef struct boollitNodeType {
    int value;
} boollitNodeType;

typedef struct nodeType {  
    nodeEnum nType; 
    struct progNodeType *prog;
    struct declNodeType *decl;
    struct typeNodeType *type;
    struct stmtSeqNodeType *seq;
    struct stmtNodeType *stmt;
    struct asgnStmtNodeType *asgnStmt;
    struct ifStmtNodeType *ifStmt;
    struct elseStmtNodeType *elseStmt;
    struct whileStmtNodeType *whileStmt;
    struct writeIntNodeType *writeStmt;
    struct exprNodeType *expr;
    struct sExprNodeType *sExpr;
    struct termNodeType *term;
    struct factorNodeType *factor;
    struct identNodeType *ident;
    struct numNodeType *num;
    struct boollitNodeType *boollit;
    struct operNodeType *oper;
} nodeType;

/* Declarations */
void gencode(nodeType*);
void gencodedecl(declNodeType*);
void gencodeseq(stmtSeqNodeType*);
void gencodeasgn(asgnStmtNodeType*);
void gencodeif(ifStmtNodeType*);
void gencodeelse(elseStmtNodeType*);
void gencodewhile(whileStmtNodeType*);
void gencodewrite(writeIntNodeType*);
void gencodeexpr(exprNodeType*);
void gencodesexpr(sExprNodeType*);
void gencodeterm(termNodeType*);
void gencodefactor(factorNodeType*);
void gencodeident(identNodeType*);
void gencodenum(numNodeType*);
void gencodeboollit(boollitNodeType*);
int installId(char*, int);
struct hashnode *findId(char*);
int findType(char*);
void deleteId(struct hashnode*);
void deleteAll();
void printIds();
int validateInt(struct hashnode*);
int validateExprAsgn(struct hashnode*, int);
int validateExpr(exprNodeType*);
int validateStmt(nodeType*);
void error(char*, char*);
int yyerror(char*);
int err(nodeType*, int);

nodeType *create_node(nodeEnum, nodeType*, nodeType*, nodeType*);

nodeType *ident(char*);
nodeType *num(int);
nodeType *boollit(int);
nodeType *oper(char*, int);