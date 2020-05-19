%{
    #include "parse_tree.h"

    struct mytype
    {
        std::string str;
        oper_t* oper;
        expr_t* expr;
        std::list<expr_t*> args;
    };

    cmdlist pcode;
    std::map <std::string, regnum> vars;
    std::map <std::string, short> strings;
    regnum lastreg = 0;

    #define YYSTYPE mytype

    extern int yylineno;
    extern int yylex();

    void yyerror(char *s) 
    {
        std::cerr << s << ", line " << yylineno << std::endl;
        exit(1);
    }

%}

%token IF ELSE WHILE EXIT
%token EQ LE GE NE
%token STRING NUM ID

%type<str> ID NUM STRING
%type<oper> OPS OP1 OP2 OP
%type<expr> EXPR EXPR1 EXPR2 TERM VAL ARG
%type<args> ARGS

%%

PROGRAM: OPS    { 
                    pcode = $1->emit(); 
                    delete $1;
                    pcode.push_back(command(command::hlt, 0, 0));
                }
;

OPS:    OP                              
|       OPS OP                          { $$ = new block($1, $2); }
;

OP1:    '{' OPS '}'                     { $$ = $2; }
|       EXPR ';'                        { $$ = new exprop($1); }
|       IF '(' EXPR ')' OP1 ELSE OP1    { $$ = new ifop($3, $5, $7); }
|       WHILE '(' EXPR ')' OP1          { $$ = new whileop($3, $5); }
|       EXIT ';'                        { $$ = new exitop(); }
;

OP2:    IF '(' EXPR ')' OP              { $$ = new ifop($3, $5, new block()); }
|       IF '(' EXPR ')' OP1 ELSE OP2    { $$ = new ifop($3, $5, $7); }
|       WHILE '(' EXPR ')' OP2          { $$ = new whileop($3, $5); }
;

OP:     OP1 | OP2 ;

EXPR:   EXPR1
|       ID '=' EXPR                     { $$ = new assign($1, $3); }

EXPR1:  EXPR2
|       EXPR1 EQ EXPR2                  { $$ = new binary(command::eq, $1, $3); }
|       EXPR1 LE EXPR2                  { $$ = new binary(command::le, $1, $3); }
|       EXPR1 GE EXPR2                  { $$ = new binary(command::ge, $1, $3); }
|       EXPR1 NE EXPR2                  { $$ = new binary(command::ne, $1, $3); }
|       EXPR1 '>' EXPR2                 { $$ = new binary(command::gt, $1, $3); }
|       EXPR1 '<' EXPR2                 { $$ = new binary(command::lt, $1, $3); }
;

EXPR2:  TERM
|       EXPR2 '+' TERM                  { $$ = new binary(command::add, $1, $3); }
|       EXPR2 '-' TERM                  { $$ = new binary(command::sub, $1, $3); }
;

TERM:   VAL
|       TERM '*' VAL                    { $$ = new binary(command::mul, $1, $3); }
|       TERM '/' VAL                    { $$ = new binary(command::div, $1, $3); }
;

VAL:    NUM                             { $$ = new value($1); }
|       '-' VAL                         { $$ = new unary(command::sub, $2); }
|       '!' VAL                         { $$ = new unary(command::eq, $2); }
|       '(' EXPR ')'                    { $$ = $2; }
|       ID                              { $$ = new varref($1); }
|       ID '(' ARGS ')'                 { $$=new funcall($1, $3); }
;

ARGS:                                   { $$.clear(); }
|       ARG                             { $$.clear(); $$.push_back($1); }
|       ARGS ',' ARG                    { $$ = $1; $$.push_back($3); }
;

ARG:    EXPR
|       STRING                          { $$ = new string($1); }
;

%%

int main() 
{
    yyparse();
    int offset = pcode.size()*sizeof(command);
    std::vector<int> offsets(strings.size());
    for(auto&& item: strings)
    {
            offsets[item.second - 1] = offset;
            offset += item.first.length();
            offset++;
    }

    for (auto&& item: pcode) 
    {
            if (item.opcode == command::echo && !item.dest)
            {
                    item.imm = offsets[item.imm - 1];
            }
            write(1, &item, sizeof(item));
    }

    for(auto&& item: strings)
    {
            write(1, item.first.c_str(), item.first.length()+1);
    }
}