%{
	#include <stdlib.h>
	#include <stdio.h>
	#include "t2c.h"
	#include "t_program_parser.h"
	#include <string.h>
	#include "t_program_structures.h"
	extern int yylineno;
	extern char ifelseop;
	extern struct node *currstmts[200];
	extern int curr_stmts_amount;
	extern int curr_nums_amount;
	extern int blocksstart;
	extern int numsstart;
	
%}
%option yylineno
%x C_COMMENT

ID  [A-Za-z][A-Za-z0-9]*
DIG [0-9][0-9]*
RNUM {DIG}"."{DIG}
NQUO [^\"]

%%

WRITE		{return lWRITE;}
READ		{return lREAD;}
IF			{	
				ifelseop = 'i';
				blocksstart = curr_stmts_amount+1;
				numsstart = curr_nums_amount;
				return lIF;
			}
ELSE		{	
				edit_ifelsenode(currstmts[blocksstart-1]);
				ifelseop = 'e';
				blocksstart = curr_stmts_amount;
				numsstart = curr_nums_amount;
				return lELSE;
			}
RETURN		{return lRETURN;}
BEGIN		{return lBEGIN;}
END			{return lEND;}
MAIN		{return lMAIN;}
INT			{return lINT;}
REAL		{return lREAL;}
";"			{return lSEMI;}
","			{return lCOMMA;}
"("			{return lLP;}
")"			{return lRP;}
"+"			{return lADD;}
"-"			{return lMINUS;}
"*"			{return lTIMES;}
"/"			{return lDIVIDE;}
">"			{return lGT;}
"<"			{return lLT;}
":="		{return lASSIGN;}
"=="		{return lEQU;}
"!="		{return lNEQ;}
">="		{return lGE;}
"<="		{return lLE;}

{ID}        { 
                yylval.sval = strdup(yytext); 
                return lID; 
            }
{DIG}       { 
                yylval.ival = atoi(yytext); 
                return lINUM; 
            }
{RNUM}      { 
                yylval.rval = atof(yytext); 
                return lRNUM; 
            }
			
\"[^\"]*\"  {
				int len = yyleng - 2; 
				yylval.sval = (char *)malloc(len + 1);
				memcpy(yylval.sval, yytext + 1, len);
				yylval.sval[len] = '\0'; 
				return lQSTR;
}

"/*"		{ BEGIN(C_COMMENT); }
<C_COMMENT>"*/"	{ BEGIN(INITIAL); }
<C_COMMENT>\n	{ }
<C_COMMENT>.	{ }
[ \t\n]		{}
.		{}

%%

int yywrap() {return 1;}

void print_lex( int t ) {
    switch( t ) {
    case lWRITE: printf("WRITE\n");
        break;
    case lREAD: printf("READ\n");
        break;
    case lIF: printf("IF\n");
        break;
    case lELSE: printf("ELSE\n");
        break;
    case lRETURN: printf("RETURN\n");
        break;
    case lBEGIN: printf("BEGIN\n");
        break;
    case lEND: printf("END\n");
        break;
    case lMAIN: printf("MAIN\n");
        break;
    case lINT: printf("INT\n");
        break;
    case lREAL: printf("REAL\n");
        break;
    case lSEMI: printf("SEMI\n");
        break;
    case lCOMMA: printf("COMMA\n");
        break;
    case lLP: printf("LP\n");
        break;
    case lRP: printf("RP\n");
        break;
    case lADD: printf("ADD\n");
        break;
    case lMINUS: printf("MINUS\n");
        break;
    case lTIMES: printf("TIMES\n");
        break;
    case lDIVIDE: printf("DIVIDE\n");
        break;
    case lASSIGN: printf("ASSIGN\n");
        break;
    case lEQU: printf("EQU\n");
        break;
    case lNEQ: printf("NEQ\n");
        break;
    case lID: printf("ID(%s)\n", name);
        break;
    case lINUM: printf("INUM(%d)\n", ival);
        break;
    case lRNUM: printf("RNUM(%f)\n", rval);
        break;
    case lQSTR: printf("QSTR(%s)\n", qstr);
        break;
    default: printf("******** lexical error!!!");
    }
}

