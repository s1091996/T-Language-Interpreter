#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t_program_structures.h"
struct node *currstmts[200];
int curr_stmts_amount = 0;

struct node *currnums[200];
int curr_nums_amount = 0;

int initial_nums_amount = 0;
int ini=0;

struct node *exprs[200];

struct tree *trees[100];
int tree_amount = 0;
int curr_tree_index = 0;

char ifelseop = 'i';
int blocksstart = -1;
int numsstart = -1;

void print_node_type (enum node_type type) {
    switch (type) {
        case NODE_NUM:         printf("Node Type: Number Node\n"); break;
        case NODE_EXPR:        printf("Node Type: Expression Node\n"); break;
        case NODE_IFSTMT:      printf("Node Type: If Statement Node\n"); break;
        case NODE_LOCALVARSTMT:printf("Node Type: Local Variable Declaration Node\n"); break;
        case NODE_ASSIGNSTMT:  printf("Node Type: Assignment Statement Node\n"); break;
        case NODE_RETURNSTMT:  printf("Node Type: Return Statement Node\n"); break;
        case NODE_WRITESTMT:   printf("Node Type: Write Statement Node\n"); break;
        case NODE_READSTMT:    printf("Node Type: Read Statement Node\n"); break;
        case NODE_FUNCSTMT:    printf("Node Type: Function Node\n"); break;
        default:               printf("Node Type: Unknown Node Type\n"); break;
    }
}

struct node* create_inode(int val){
	struct node *new_node = (struct node*)malloc(sizeof(struct node));
	struct num_node *num = (struct num_node*)malloc(sizeof(struct num_node));
	
	if (new_node == NULL || num== NULL) {
        printf("Memory allocation failed");
        abort();
    }
	
	num->type = 'i';
    num->value.ival = val;
	
	new_node->type = NODE_NUM;
	new_node->ndtp.numnd = num;
	
	return new_node;
}

struct node* create_rnode(double val){
	struct node *new_node = (struct node*)malloc(sizeof(struct node));
	struct num_node *num = (struct num_node*)malloc(sizeof(struct num_node));
	
	if (new_node == NULL || num== NULL) {
        printf("Memory allocation failed");
        abort();
    }
   
	num->type = 'r';
    num->value.rval = val;
	
	new_node->type = NODE_NUM;
	new_node->ndtp.numnd = num;
	
	return new_node;
}

struct node* create_exprnode(struct node* a,struct node* b,char op){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct expr_node *expr = (struct expr_node*)malloc(sizeof(struct expr_node));
	
	if (new_node == NULL || expr==NULL) {
		printf("Memory allocation failed");
		abort();
	}
	
	expr->left = a;
	expr->right = b;
	expr->op = op;
	
	new_node->type = NODE_EXPR;
	new_node->ndtp.expnd = expr;
	
	return new_node;
}

struct node* execute_exprnode(struct node* mthexpr){
	if(mthexpr==NULL){
		printf("error: Undefined Behavior\n");
		abort();
	}
	if(mthexpr->type==NODE_NUM){
		if(mthexpr->ndtp.numnd->haveval=='n'){
			printf("error: Undefined Behavior\n");
			abort();
		}
		return mthexpr;
	}
	
	if(mthexpr->type!=NODE_EXPR){
		printf("execute_exprnode error: error type\n");	
		abort();
	}
	
	struct node *a;
	struct node *b;
	if(mthexpr->ndtp.expnd->right != NULL) a = execute_exprnode(mthexpr->ndtp.expnd->right);
	if(mthexpr->ndtp.expnd->left != NULL) b = execute_exprnode(mthexpr->ndtp.expnd->left);
	
	if(a->type!=NODE_NUM || b->type!=NODE_NUM){
		printf("error node\n");
		abort();
	}
	
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	new_node->ndtp.numnd = (struct num_node *)malloc(sizeof(struct num_node));
	
	if (new_node == NULL) {
        printf("Memory allocation failed");
        abort();
    }
	new_node->type = NODE_NUM;
	
	if(a->ndtp.numnd->type=='i'&& b->ndtp.numnd->type=='i'){
		new_node->ndtp.numnd->type = 'i';
		int aval = a->ndtp.numnd->value.ival;
		int bval = b->ndtp.numnd->value.ival;
		int val = 0;
		
		switch (mthexpr->ndtp.expnd->op) {
			case '+':
				val = aval+bval;
				break;
			case '-':
				val = aval-bval;
				break;
			case '*':
				val = aval*bval;
				break;
			case '/':
				val = aval/bval;
				break;
			default:
				printf("execute_exprnode: Unknown operator\n");
				abort();
				free(new_node);
				return NULL;
		}
		new_node->ndtp.numnd->value.ival = val;
	}else {
		new_node->ndtp.numnd->type = 'r';
		
        double aval = (a->ndtp.numnd->type == 'i') ? (double)a->ndtp.numnd->value.ival : a->ndtp.numnd->value.rval;
        double bval = (b->ndtp.numnd->type == 'i') ? (double)b->ndtp.numnd->value.ival : b->ndtp.numnd->value.rval;
		double val = 0.0;

        switch (mthexpr->ndtp.expnd->op) {
			case '+':
				val = aval+bval;
				break;
			case '-':
				val = aval-bval;
				break;
			case '*':
				val = aval*bval;
				break;
			case '/':
				val = aval/bval;
				break;
			default:
				printf("execute_exprnode:Unknown operator\n");
				abort();
				free(new_node);
				return NULL;
		}
	
		new_node->ndtp.numnd->value.rval = val;
    }
	return new_node;
}

struct node* create_localvarnode(const char* id, char type, struct node *expr){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct localvar_stmt *localvar = (struct localvar_stmt *)malloc(sizeof(struct localvar_stmt));
	struct node *num_node = (struct node *)malloc(sizeof(struct node)); 
	struct num_node *num = (struct num_node  *)malloc(sizeof(struct num_node));
	
	if(find_global_numnode(id)!=NULL){
		printf("redefinition\n");
		abort();
	}
	
	if (new_node == NULL || localvar== NULL || num_node==NULL ||num==NULL) {
        printf("Memory allocation failed");
        abort();
    }
	
    localvar->id = (char *)malloc(strlen(id) + 1);
	strncpy(localvar->id, id, strlen(id) + 1);
	
	localvar->expr = expr;
	
	new_node->type = NODE_LOCALVARSTMT;
	new_node->ndtp.lcstmt = localvar;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	num->id = (char *)malloc(strlen(id) + 1);
	strncpy(num->id, id, strlen(id) + 1);
	num->type = type;
	num->haveval = 'n';
	
	num_node->type = NODE_NUM;
	num_node->ndtp.numnd = num;
	
	currnums[curr_nums_amount] = num_node;
	curr_nums_amount++;
	
	return new_node;
}

void execute_localvarnode(struct localvar_stmt* localvar){
	if(localvar==NULL){
		printf("execute_localvarnode error: NULL node\n");
		abort();
	}
	
	if(localvar->expr!=NULL){
		struct node *newnode = find_tree_numnode(trees[curr_tree_index],localvar->id);
		struct node *expr = execute_exprnode(localvar->expr);
		if(newnode->ndtp.numnd->type=='i'){
			newnode->ndtp.numnd->value.ival = (expr->ndtp.numnd->type=='i')? expr->ndtp.numnd->value.ival : (int)expr->ndtp.numnd->value.rval;
		}else{
			newnode->ndtp.numnd->value.rval = (expr->ndtp.numnd->type=='i')? (double)expr->ndtp.numnd->value.ival : expr->ndtp.numnd->value.rval;
		}
		newnode->ndtp.numnd->haveval = 'y';
	}
}

struct node* create_assignnode(struct num_node *assvar, struct node *expr){
	
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct assign_stmt *assign = (struct assign_stmt *)malloc(sizeof(struct assign_stmt));
	if (new_node == NULL || assign== NULL) {
        printf("Memory allocation failed");
        abort();
    }
	
    assign->assvar = assvar;
	assign->expr = expr;
	
	new_node->type = NODE_ASSIGNSTMT;
	new_node->ndtp.asgstmt = assign;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

void execute_assignnode(struct assign_stmt* assign){
	struct num_node *numnode;
	if(assign->expr->type==NODE_NUM){
		numnode = assign->expr->ndtp.numnd;
	}else{
		numnode = execute_exprnode(assign->expr)->ndtp.numnd;
	}
	
	if(assign->assvar->type == 'i'){
		int val = (numnode->type =='i')? numnode->value.ival : (int)numnode->value.rval;
		assign->assvar->value.ival = val;
	}else{
		double val = (numnode->type =='i')? (double)numnode->value.ival : numnode->value.rval;
		assign->assvar->value.rval = val;
	}
	numnode->haveval = 'y';
}

struct node* create_writenode(char *str,struct node *expr){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct write_stmt *write = (struct write_stmt *)malloc(sizeof(struct write_stmt));
	
	if (new_node == NULL || write == NULL) {
		printf("Memory allocation failed");
		abort();
	}
	
	write->sval = str;
	write->expr = expr;
	
	new_node->type = NODE_WRITESTMT;
	new_node->ndtp.wtstmt = write;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

void execute_writenode(struct write_stmt* wrtnode){
	if(wrtnode==NULL){
		printf("execute_write_node error: NULL node\n");
		abort();
	}
	struct node *num;
	num = execute_exprnode(wrtnode->expr);
	struct num_node *numnode = num->ndtp.numnd;
	printf("%s ",wrtnode->sval);
	
	if(numnode->type=='i'){
		printf("%d\n",numnode->value.ival);
	}else{
		printf("%f\n",numnode->value.rval);
	}
}

struct node* create_readnode(char* str,struct node *expr){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct read_stmt *read = (struct read_stmt *)malloc(sizeof(struct read_stmt));

	if (new_node == NULL || read == NULL) {
		printf("Memory allocation failed");
		abort();
	}
	
	read->sval = str;
	read->expr = expr;
	
	new_node->type = NODE_READSTMT;
	new_node->ndtp.rdstmt = read;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

void execute_readnode(struct read_stmt* rdnode){
	if(rdnode==NULL){
		printf("execute_write_node error: NULL node\n");
		abort();
	}
	
	struct node *num;
	struct num_node *numnode = rdnode->expr->ndtp.numnd;
	printf("%s\n",rdnode->sval);
	if(numnode->type=='i'){
		int number;
		scanf("%d", &number);
		numnode->value.ival = number;
	}else{
		double number;
		scanf("%lf", &number);
		numnode->value.rval = number;
	}
	numnode->haveval = 'y';
}

struct node *create_ifelsenode(struct node* expr1, struct node* expr2,int op){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct ifelse_stmt *ifelse = (struct ifelse_stmt *)malloc(sizeof(struct ifelse_stmt));
	
	if (new_node == NULL || ifelse == NULL) {
		printf("Memory allocation failed");
		abort();
	}
	
	switch (op) {
        case 1:
            ifelse->type = bool_GT;
			break;
        case 2:
            ifelse->type = bool_LT;
			break;
        case 3:
            ifelse->type = bool_EQU;
			break;
        case 4:
            ifelse->type = bool_NEQ;
			break;
        case 5:
            ifelse->type = bool_GE;
			break;
        case 6:
            ifelse->type = bool_LE;
			break;
        default:
            printf("Error: Invalid number %d. Returning default bool_GT.\n");
			abort();
            break; 
    }
	ifelse->expr1 = expr1;
	ifelse->expr2 = expr2;
	
	new_node->type = NODE_IFSTMT;
	new_node->ndtp.ifstmt = ifelse;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

struct tree* create_ifelsetree(int blockst,int blockend,int numst,int numend){
	struct tree *new_tree =  (struct tree *)malloc(sizeof(struct tree));
	
	if (new_tree == NULL) {
		printf("Memory allocation failed");
		abort();
	}
	
	new_tree->stmts_amount = blockend - blockst;
	new_tree->stmts = malloc(new_tree->stmts_amount * sizeof(struct node *));
	
	for(int i=blockst; i<blockend; i++){
		new_tree->stmts[i-blockst] = currstmts[i];
		currstmts[i] = NULL;
	}
	
	new_tree->nums_amount = numend - numst;
	new_tree->nums = malloc(new_tree->nums_amount * sizeof(struct num_node *));
	
	for(int i=numst; i<numend; i++){
		new_tree->nums[i-numst] = currnums[i];
		currnums[i] = NULL;
	}
	
	curr_nums_amount -= new_tree->nums_amount;
	curr_stmts_amount -= new_tree->stmts_amount;
	
	return new_tree;	
}

void edit_ifelsenode(struct node *ifelse){
	if(ifelse->type!=NODE_IFSTMT){
		printf("ifelsenode error: error type\n");	
		abort();
	}
	struct ifelse_stmt *ie = ifelse->ndtp.ifstmt;	

	if(ifelseop=='i'){
		ie->iftree = create_ifelsetree(blocksstart,curr_stmts_amount,numsstart,curr_nums_amount);
	}
	if(ifelseop=='e'){
		ie->elsetree = create_ifelsetree(blocksstart,curr_stmts_amount,numsstart,curr_nums_amount);
	}
	
	
}

void execute_ifelsenode(struct ifelse_stmt *ifelse){
	struct node *e1 = execute_exprnode(ifelse->expr1);
	struct node *e2 = execute_exprnode(ifelse->expr2);
	struct num_node *n1 = e1->ndtp.numnd;
	struct num_node *n2 = e2->ndtp.numnd;
	int val1 = (n1->type=='i')? n1->value.ival : (int)n1->value.rval+1;
	int val2 = (n2->type=='i')? n2->value.ival : (int)n2->value.rval+1;
	
	switch (ifelse->type) {
        case bool_GT:
            ifelse->val = (val1 > val2) ? 1 : 0;
            break;
        case bool_LT:
            ifelse->val = (val1 < val2) ? 1 : 0;
            break;
        case bool_EQU:
            ifelse->val = (val1 == val2) ? 1 : 0;
            break;
        case bool_NEQ:
            ifelse->val = (val1 != val2) ? 1 : 0;
            break;
        case bool_GE:
            ifelse->val = (val1 >= val2) ? 1 : 0;
            break;
        case bool_LE:
            ifelse->val = (val1 <= val2) ? 1 : 0;
            break;
        default:
            printf("Error: Invalid bool_type.\n");
			abort();
            break;
    }
	(ifelse->val==1)? execute_tree(ifelse->iftree) : execute_tree(ifelse->elsetree);
	free(e1);
	free(e2);
}

struct node* create_returnnode(struct node *expr){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct return_stmt *rt = (struct return_stmt *)malloc(sizeof(struct return_stmt));
	
	rt->expr = expr;
	
	new_node->type = NODE_RETURNSTMT;
	new_node->ndtp.rtstmt = rt;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

struct node* create_funcnode(const char *id){
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	struct func_stmt *func = (struct func_stmt *)malloc(sizeof(struct func_stmt));
	struct tree *tr = find_tree(id);
	func->exprs = malloc(tr->initial_amount * sizeof(struct node *));;
	func->tree = tr;
	
	
	if (new_node == NULL || func->exprs == NULL) {
		printf("Memory allocation failed\n");
		abort();
	}
	
	for(int i=0,j = ini-1; i<ini; i++,j--){
		func->exprs[i] = exprs[j];
		exprs[j] = NULL;
	}
	ini = 0;
	
	
	
	new_node->type = NODE_FUNCSTMT;
	new_node->ndtp.fc = func;
	
	currstmts[curr_stmts_amount] = new_node;
	curr_stmts_amount++;
	
	return new_node;
}

struct tree* create_tree(const char *id){//func
	
	struct tree *new_tree =  (struct tree *)malloc(sizeof(struct tree));
	
	if (new_tree == NULL) {
		printf("Memory allocation failed");
		abort();
	}
	new_tree->initial_amount = initial_nums_amount;
	new_tree->initialnums = malloc(new_tree->initial_amount * sizeof(struct node *));
	initial_nums_amount = 0;
	
	new_tree->stmts_amount = curr_stmts_amount;
	new_tree->stmts = malloc(new_tree->stmts_amount * sizeof(struct node *));
	
	new_tree->id = (char *)malloc(strlen(id) + 1);
	strncpy(new_tree->id, id, strlen(id) + 1);
	
	for(int i=0; i < new_tree->stmts_amount; i++){
		new_tree->stmts[i] = currstmts[i];
		currstmts[i] = NULL;
	}
	
	new_tree->nums_amount = curr_nums_amount;
	new_tree->nums = malloc(new_tree->nums_amount * sizeof(struct num_node *));
	
	for(int i=0; i<new_tree->nums_amount; i++){
		new_tree->nums[i] = currnums[i];
		currnums[i] = NULL;
	}
	
	curr_nums_amount = 0;
	curr_stmts_amount = 0;
	
	trees[tree_amount] = new_tree;
	tree_amount++;
	
	return new_tree;
}

struct node* execute_tree(struct tree* currtree){
	if(currtree==NULL)return NULL;
	find_tree(currtree->id);
	trees[curr_tree_index]->value = NULL;
	int stmtsamount =  currtree->stmts_amount;
	struct tree *tr;
	for(int i=0; i<stmtsamount; i++){
		
		switch (currtree->stmts[i]->type) {
            
			case NODE_LOCALVARSTMT:
				execute_localvarnode(currtree->stmts[i]->ndtp.lcstmt);
				currtree->nums_amount++;
                break;
				
            case NODE_ASSIGNSTMT:
				execute_assignnode(currtree->stmts[i]->ndtp.asgstmt);
                break;
			
			case NODE_IFSTMT:
				execute_ifelsenode(currtree->stmts[i]->ndtp.ifstmt);
                break;
            
			case NODE_RETURNSTMT:
				trees[curr_tree_index]->value = currtree->stmts[i]->ndtp.rtstmt->expr;
				return currtree->value;
                break;
			
			case NODE_WRITESTMT:
				execute_writenode(currtree->stmts[i]->ndtp.wtstmt);
                break;
			
			case NODE_READSTMT:
				execute_readnode(currtree->stmts[i]->ndtp.rdstmt);
                break;
				
			case NODE_FUNCSTMT:
				tr = currtree->stmts[i]->ndtp.fc->tree;
				for(int i=0; i<tr->initial_amount; i++){
					if (currtree->stmts[i]->ndtp.fc->exprs[i] == NULL) {
						
						abort();
					}
					print_node_type(currtree->stmts[i]->ndtp.fc->exprs[i]->type);
					tr->stmts[i]->ndtp.lcstmt->expr = currtree->stmts[i]->ndtp.fc->exprs[i];
				}
				execute_tree(tr);
                break;
				
			default:
                printf("execute_tree:Unknown operator\n");
				abort();
                break;
        }
	}
	return NULL;
}	

struct tree* find_tree(const char* id){
	 for(int i = 0; i < tree_amount; i++){
        char *cid = trees[i]->id;
		if(strcmp(cid, id) == 0){
			curr_tree_index = i;
			return trees[i];  
        }
    }
	return NULL;
}

struct node* find_global_numnode(const char* id){	 
    for(int i = 0; i < curr_nums_amount; i++){
        char *cid = currnums[i]->ndtp.numnd->id;
		if(strcmp(cid, id) == 0){
			return currnums[i];  
        }
    }
    return NULL;
}

struct node* find_tree_numnode(struct tree* tr,char* id){	 
	for(int i = 0; i < tr->nums_amount; i++){
        char *cid = tr->nums[i]->ndtp.numnd->id;
		if(strcmp(cid, id) == 0){
			return tr->nums[i];  
        }
    }
    return NULL;
}

void parse(){
	struct tree *tr = find_tree("MAIN");
	if(tr==NULL){
		printf("Program does not contain a static 'MAIN' method suitable for an entry point.");
		abort();
		return;
	}
	execute_tree(tr);
}
