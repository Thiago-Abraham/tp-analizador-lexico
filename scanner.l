%{

#include <stdio.h>

%}

%pointer
%option noyywrap

/*Condiciones de Arranque*/
%x tokenNoReconocido

/* Declaraciones */
centinela       [ \t\n]
nondigit        [_a-zA-Z]
digit           [0-9]
identifier      {nondigit}({nondigit}|{digit})*
operators       (\[\]|[().*+\-~!&?|/%<>#,:]|->|\+\+|--|sizeof|<<|>>|&&|\|\||[*/%+!|<=>&^]?=|<<=|>>=|##)
punctuators     (\[\]|[(){}*,:=#;]|\.\.\.)
noReconocido    [^ \t\n]


%%

[1-9][0-9]*             { printf("\nEncontre una constante entera decimal: %s\n", yytext); }
0[0-7]*                 { printf("\nEncontre una constante entera octal: %s\n", yytext); }
0[xX][0-9a-fA-F]+       { printf("\nEncontre una constante entera hexadecimal: %s\n", yytext); }
{punctuators}           { printf("\nEncontre un puntuador: %s\n", yytext); }
{operators}             { printf("\nEncontre un operador: %s\n", yytext); }
{identifier}            { printf("\nEncontre un identificador: %s\n", yytext); }
{noReconocido}          { printf("\nEncontre un token no reconocido: %s", yytext); BEGIN(tokenNoReconocido); }
{centinela}             { }

/*
    tokenNoReconocidos es un automata separado que se ejecuta al identificar el primer caracter no reconocido. Este tiene sus propias declaraciones y estados.
*/
<tokenNoReconocido>{
    [^ \t\n]+           { printf("%s", yytext); BEGIN(INITIAL); printf("\n"); }
    [ \t\n]             {yyless(0); printf("\n"); BEGIN(INITIAL); } 
    <<EOF>>             { printf("\n"); BEGIN(INITIAL); }
}

%%

int main(int argc, char *argv[]) {
    yylex();
    return 0;
}