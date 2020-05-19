%{
    #include "parse_tree.h"

    struct mytype
    {
        std::string str;
        oper_t* oper;
        expr_t* expr;
        std::list<expr_t*> args;
    };
    #define YYSTYPE mytype

    #include "b.tab.hpp"

    void yyerror(char *s);
%}

%option yylineno
%option noyywrap

%x STR

%%

[/][/].*\n      ;
if              return IF;
else            return ELSE;
while           return WHILE;
exit            return EXIT;
==              return EQ;
[<]=            return LE;
>=              return GE;
!=              return NE;


[0-9]+          { 
                    yylval.str = yytext;
                    return NUM;
                }


[a-zA-Z_][a-zA-Z0-9_]*  { 
                        yylval.str = yytext;
                        return ID;
                        }


["]             { yylval.str = ""; BEGIN(STR); }
<STR>[^\\\n"]+  yylval.str += yytext;
<STR>\\n        yylval.str += '\n';
<STR>\\["]      yylval.str += '"';
<STR>\\         yyerror("Invalid escape sequence");
<STR>\n         yyerror("Newline in string literal");
<STR>["]        { BEGIN(INITIAL); return STRING; }
[ \t\r\n]       ;
[-{};()=<>+*/!,] { return *yytext; }
.               yyerror("Invalid character");

%%