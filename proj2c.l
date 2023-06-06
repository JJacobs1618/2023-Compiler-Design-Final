/* Definitions */
%{    
    #include <stdio.h>
	#include <stdlib.h>
    #include <string.h>
    #include <ctype.h> 
    #include "proj2c.tab.h"

    int yywrap(void) { return 1; }
%}

%option noinput
%option nounput

white   [[:space:]]|\%.*

num     ([1-9][0-9]*)|0
ident   [A-Z][A-Z0-9]*

T_OP2     (\*)|(div)|(mod)
T_OP3     (\+)|(\-)
T_OP4     (=)|(!=)|(<)|(>)|(<=)|(>=)

/* Rules */
%%

{white}     { ; } 

{num}       {       
                yylval.iVal = atoi(yytext);
                if(yylval.iVal != atol(yytext)) /*catch overflow*/
                    err(NULL, -1);
                return(T_NUM);
}

"false"     {
                yylval.iVal = 0;
                return(T_BOOLLIT);
            }

"true"      {
                yylval.iVal = 1;
                return(T_BOOLLIT);
            }

{ident}     {
                yylval.sVal = strdup(yytext);
                return(T_IDENT);
            }

"("             { return(T_LP); }
")"             { return(T_RP); }
":="            { return(T_ASGN); }
";"             { return(T_SC); }

{T_OP2}     {
                yylval.sVal = strdup(yytext);
                return(T_OP2);
            }

{T_OP3}     {
                yylval.sVal = strdup(yytext);
                return(T_OP3);
            }
{T_OP4}     {
                yylval.sVal = strdup(yytext);
                return(T_OP4);
            }

"if"            { return(T_IF); }
"then"          { return(T_THEN); }
"else"          { return(T_ELSE); }
"begin"         { return(T_BGN); }
"end"           { return(T_END); }
"while"         { return(T_WHILE); }
"do"            { return(T_DO); }
"program"       { return(T_PROGRAM); }
"var"           { return(T_VAR); }
"as"            { return(T_AS); }
"int"           { return(T_INT); }
"bool"          { return(T_BOOL); }

"writeInt"      { return(T_WRITEINT); }
"readInt"       { return(T_READINT); }
%%
