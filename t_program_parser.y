%{
	#include <string.h>	
	#include <stdio.h>
	#include <stdlib.h>
	#include "t2c.h"
	#include "t_program_parser.h"
	#include "t_program_structures.h"
	extern int yylineno;
	extern struct node *currstmts[200];
	extern int curr_stmts_amount;

	extern struct node *currnums[200];
	extern int curr_nums_amount;
	extern int initial_nums_amount;
	extern int ini;
	
	extern struct node *exprs[200];

	extern struct tree *trees[100];
	extern int tree_amount;
	
	extern char ifelseop;
	extern int blockstart;
	extern int numsstart;
%}
%union {
    struct node *nd;
	int ival;
    double rval;
	char tp;
	char *sval;
}
%token lINT lREAL
%token lWRITE lREAD lIF lASSIGN
%token lRETURN lBEGIN lEND
%left  lEQU lNEQ lGT lLT lGE lLE
%left  lADD lMINUS
%left  lTIMES lDIVIDE
%token lLP lRP
%token lELSE
%token lMAIN
%token lSEMI lCOMMA
%token<sval> lID lQSTR
%token<ival> lINUM 
%token<rval> lRNUM
%type <nd> expr primexpr multexpr boolexpr ifstmt stmts stmt actuals oactuals returnstmt writestmt
%type <tp> type

%expect 1

%%
prog	:	mthdcls
		{	printf("Program -> MethodDecls\n");
			printf("Parsed OK!\n");
			parse();
		}
	|
		{ printf("****** Parsing failed!\n"); }	
	;

mthdcls	:	mthdcl mthdcls
		{ printf("MethodDecls -> MethodDecl MethodDecls\n"); }	
	|	mthdcl
		{ printf("MethodDecls -> MethodDecl\n"); }	
	;

type	:	lINT
		{ 
			printf("Type -> INT\n"); 
			$$ = 'i';
		}	
	|	lREAL
		{ 
			printf("Type -> REAL\n"); 
			$$ = 'r';
		}	
	;

mthdcl	:	type lMAIN lID lLP formals lRP block
		{ 
			printf("MethodDecl -> Type MAIN ID LP Formals RP Block\n"); 
			create_tree("MAIN");
		}	
	|	type lID lLP formals lRP block
		{ 
			printf("MethodDecl -> Type ID LP Formals RP Block\n"); 
			create_tree($2);
		}	
	;

formals	:	formal oformal
		{ printf("Formals -> Formal OtherFormals\n"); }	
	|
		{ printf("Formals -> \n"); }	
	;

formal	:	type lID
		{ 
			printf("Formal -> Type ID\n"); 
			create_localvarnode($2,$1,NULL);
			initial_nums_amount++;
		}	
	;

oformal	:	lCOMMA formal oformal
		{ printf("OtherFormals -> COMMA Formal OtherFormals\n"); }	
	|
		{ printf("OtherFormals -> \n"); }	
	;
block   : lBEGIN stmts lEND
        { 
			printf("Block -> BEGIN Statements END\n"); 
		}
        ;
		
stmts   : stmt stmts
        { printf("Statements -> Statement Statements\n"); }
        | 
        { printf("Statements -> Statement\n"); }
        ;
		
stmt    : block
        { printf("Statement -> Block\n"); }
        | localvar
        { printf("Statement -> LocalVarDecl\n"); }
        | assignstmt
        { printf("Statement -> AssignStmt\n"); }
        | returnstmt
        { printf("Statement -> ReturnStmt\n"); }
        | ifstmt
        { printf("Statement -> IfStmt\n"); }
        | writestmt
        { printf("Statement -> WriteStmt\n"); }
        | readstmt
        { printf("Statement -> ReadStmt\n"); }
        ;

localvar: type lID lSEMI
        { printf("LocalVarDecl -> Type ID SEMI\n"); 
			create_localvarnode($2,$1,NULL);
		}
        | type lID lASSIGN expr lSEMI
        { 
			printf("LocalVarDecl -> lASSIGN\n");
			create_localvarnode($2,$1,$4);
			printf("local: curr_nums_amount:%d\n",curr_nums_amount);
		}
        ;
assignstmt : lID lASSIGN expr lSEMI
        { 
			printf("AssignStmt -> ID ASSIGN Expression SEMI\n");
			create_assignnode(find_global_numnode($1)->ndtp.numnd,$3);
		}
        ;

returnstmt : lRETURN expr lSEMI
        { 
			printf("ReturnStmt -> RETURN Expression SEMI\n");
			create_returnnode($2);
		}
        ;
ifstmt  : lIF lLP boolexpr lRP stmt lELSE stmt
		{	printf("IfStmt -> IF LP BoolExpr RP Statement ELSE Statement\n"); 
			edit_ifelsenode($3);
		}
        | lIF lLP boolexpr lRP stmt
        { 
			printf("IfStmt -> IF LP BoolExpr RP Statement\n");
			edit_ifelsenode($3);	
		}
        ;

writestmt : lWRITE lLP expr lCOMMA lQSTR lRP lSEMI
        { 
			printf("writestmt ->  WRITE LP expr COMMA QSTR RP SEMI\n"); 
			$$ = create_writenode($5,$3);
		}
        ;

readstmt : lREAD lLP lID lCOMMA lQSTR lRP lSEMI
        { 
			printf("ReadStmt -> READ LP ID COMMA QString RP SEMI\n"); 
			create_readnode($5,find_global_numnode($3));
		}
        ;

expr    :   multexpr
        { 
			printf("Expression -> MultExpr\n"); 
			$$ = $1;
		}
    |   expr lADD multexpr
        { 
			printf("Expression -> Expression ADD MultExpr\n"); 
			$$ = create_exprnode($1,$3,'+');
		}
    |   expr lMINUS multexpr
        { 	printf("Expression -> Expression MINUS MultExpr\n"); 
			$$ = create_exprnode($1,$3,'-');
		}
    ;
multexpr:   primexpr
        { 
			printf("MultExpr -> PrimExpr\n"); 
			$$ = $1;
		}
    |   multexpr lTIMES primexpr
        { 
			printf("MultExpr -> MultExpr TIMES PrimExpr\n");
			$$ = create_exprnode($1,$3,'*');			
		}
    |   multexpr lDIVIDE primexpr
        { 
			printf("MultExpr -> MultExpr DIVIDE PrimExpr\n"); 
			$$ = create_exprnode($1,$3,'/');
		}
    ;
primexpr:lINUM
        { 
			printf("PrimExpr -> INUM\n"); 
			$$ = create_inode($1);	
		}
    |   lRNUM
        { 
			printf("PrimExpr -> RNUM\n"); 
			$$ = create_rnode($1);
		}
    |   lID  
        { 	
			printf("PrimExpr -> ID\n"); 
			$$ = find_global_numnode($1);
		}
    |   lLP expr lRP
        { 
			printf("PrimExpr -> LP Expression RP\n"); 
			$$ = $2;
		}
    |   lID lLP actuals lRP
        { 
			printf("PrimExpr -> ID LP Actuals RP\n");
			create_funcnode($1);
			$$ = find_tree($1)->value;
		}
    ;
	
actuals :   expr oactuals
        { 
			printf("Actuals -> Expression OtherActuals\n"); 
			exprs[ini] = $1;
			ini++;
		}
    |
        { printf("Actuals -> \n"); }
    ;
oactuals:   lCOMMA expr oactuals
        { 
			printf("OtherActuals -> COMMA Expression OtherActuals\n"); 
			exprs[ini] = $2;
			ini++;
		}
    |
        { printf("OtherActuals -> \n"); }
	;
boolexpr: expr lEQU expr //
        { 
			printf("BoolExpr -> Expression EQU Expression\n"); 
			$$ = create_ifelsenode($1,$3,3);
		}
    |   expr lNEQ expr
        {	
			printf("BoolExpr -> Expression NEQ Expression\n"); 
			$$ = create_ifelsenode($1,$3,4);
		}
    |   expr lGT expr
        { 
			printf("BoolExpr -> Expression GT Expression\n"); 
			$$ = create_ifelsenode($1,$3,1);
		}
    |   expr lGE expr
        { 
			printf("BoolExpr -> Expression GE Expression\n"); 
			$$ = create_ifelsenode($1,$3,5);
		}
    |   expr lLT expr
        { 
			printf("BoolExpr -> Expression LT Expression\n"); 
			$$ = create_ifelsenode($1,$3,2);
		}
    |   expr lLE expr
        { 
			printf("BoolExpr -> Expression LE Expression\n"); 
			$$ = create_ifelsenode($1,$3,6);
		}
    ;

%%

int yyerror(char *s)
{
	fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
	return 1;
}

