#ifndef T_PROGRAM_STRUCTURES_H
#define T_PROGRAM_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern struct node *currstmts[200];
extern int curr_stmts_amount;

extern struct node *currnums[200];
extern int curr_nums_amount;
extern int initial_nums_amount;
extern int ini;

extern struct tree *trees[100];
extern int tree_amount;
extern int curr_tree_index;

extern char ifelseop;
extern int blocksstart;
extern int numsstart;



enum node_type {
    NODE_NUM,        	
	NODE_EXPR,		 	
    NODE_IFSTMT,        
	NODE_LOCALVARSTMT,
    NODE_ASSIGNSTMT,     
    NODE_RETURNSTMT,     
    NODE_WRITESTMT,      
    NODE_READSTMT,      	
	NODE_FUNCSTMT, 
};

struct node{
	enum node_type type;
	union{
		struct num_node *numnd;
		struct expr_node *expnd;
		struct ifelse_stmt *ifstmt;
		struct localvar_stmt *lcstmt;
		struct assign_stmt *asgstmt;
		struct return_stmt *rtstmt;
		struct write_stmt *wtstmt;
		struct read_stmt *rdstmt;
		struct func_stmt *fc;
	}ndtp;
};

struct num_node{
	char *id;
	char type;    			
	char haveval; 			
	union {
        int ival;          
        double rval;       
    }value;
};

struct expr_node{
	char op;
	struct node *left;
	struct node *right;
};

enum bool_type {
    bool_GT,        
	bool_LT,		
    bool_EQU,       
	bool_NEQ,		
    bool_GE,    	 
    bool_LE,     
};
struct ifelse_stmt{
	enum bool_type type;
	int val; 
	struct node* expr1;
	struct node* expr2;
	struct tree* iftree;
	struct tree* elsetree;
};


struct localvar_stmt{
	char* id;
	struct node* expr;
};

struct assign_stmt{
	struct num_node *assvar;
	struct node *expr;
};

struct return_stmt{
	struct node *expr;
};

struct write_stmt{
	char *sval;
	struct node *expr;
};

struct read_stmt{
	char *sval;
	struct node *expr;
};

struct func_stmt{
	char *sval;
	struct node **exprs;
	struct tree *tree;
};

struct tree{
	char* id;
	struct node *value;
	
	struct node **stmts;
	int stmts_amount;
	
	struct node **initialnums;
	int initial_amount;
	
	struct node **nums;
	int nums_amount;
};



struct node* create_inode(int val);
struct node* create_rnode(double val);

struct node* create_exprnode(struct node* a,struct node* b,char op);
struct node* execute_exprnode(struct node* mthexpr);

struct node* create_localvarnode(const char* id, char type, struct node *expr);
void execute_localvarnode(struct localvar_stmt* localvar);

struct node* create_assignnode(struct num_node *assvar, struct node *expr);
void execute_assignnode(struct assign_stmt* assign);

struct node* create_writenode(char *str,struct node *expr);
void execute_writenode(struct write_stmt* wrtnode);

struct node* create_readnode(char* str,struct node *expr);
void execute_readnode(struct read_stmt* rdnode);

struct node *create_ifelsenode(struct node* expr1, struct node* expr2,int op);
struct tree* create_ifelsetree(int blockst,int blockend,int numst,int numend);
void edit_ifelsenode(struct node *ifelse);
void execute_ifelsenode(struct ifelse_stmt *ifelse);

struct node * create_returnnode(struct node *expr);
void execute_returnnode(struct return_stmt *rt);

struct node* create_funcnode(const char* id);
struct tree* create_tree(const char *id);
struct node* execute_tree(struct tree* currtree);
struct tree* find_tree(const char* id);

struct node* find_global_numnode(const char* id);
struct node* find_tree_numnode(struct tree* tr,char* id);

void parse();

#endif