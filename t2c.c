#include <stdio.h>
#include "t2c.h"
#include "t_program_parser.h"

char name[16];
int ival;
float rval;
char qstr[80];

int main(int argc,char *argv[]) {
    int t;

    yyin = fopen(argv[1],"r");
    t = yyparse();
}
