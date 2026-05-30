%{

#include <stdio.h>
#include <stdbool.h>

bool DEBUG_MODE = false;

void log_debug(const char *mensaje, const char *texto) {
    if (DEBUG_MODE) {
        fprintf(stderr, "[DEBUG] %s: '%s'\n", mensaje, texto);
    } else {
        /*esto luego deberia ser la funcion de imprimir reporte que da todo el formato*/
        fprintf(stdout, "%s: %s\n", mensaje, texto);
    }
}

int tokens_reconocidos = 0;
int tokens_no_reconocidos = 0;

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

[1-9][0-9]*             { log_debug("Encontre una constante entera decimal", yytext); tokens_reconocidos++; }
0[0-7]*                 { log_debug("Constante entera octal", yytext); tokens_reconocidos++; }
0[xX][0-9a-fA-F]+       { log_debug("Constante entera hexadecimal", yytext); tokens_reconocidos++; }
{punctuators}           { log_debug("Puntuador", yytext); tokens_reconocidos++; }
{operators}             { log_debug ("Operador", yytext); tokens_reconocidos++; }
{identifier}            { log_debug("Identificador", yytext); tokens_reconocidos++; }
{noReconocido}          { if (DEBUG_MODE) fprintf(stderr, "\n[DEBUG] ERROR: Token no reconocido: %s", yytext);
                            tokens_no_reconocidos++;
                            BEGIN(tokenNoReconocido);  }
{centinela}             { }


<tokenNoReconocido>{
    [^ \t\n]+ {
        if (DEBUG_MODE) fprintf(stderr, "%s", yytext); 
        BEGIN(INITIAL); 
        if (DEBUG_MODE) fprintf(stderr, "'\n");
    }
    [ \t\n] {
        yyless(0); 
        BEGIN(INITIAL); 
        if (DEBUG_MODE) fprintf(stderr, "'\n");
    } 
    <<EOF>> {
        BEGIN(INITIAL); 
        if (DEBUG_MODE) fprintf(stderr, "'\n");
    }
}

%%

// argc cantidad de parametros, argv[] lista con contenido de los parametros
int main(int argc, char *argv[]) {
    // Validacion del parametro obligatorio, con !=2 porque lee nombre y ruta del archivo (./bin/tp2.exe <ruta archivo entrada>)
    if (argc < 2) {
        fprintf(stderr, "Error: Parámetro obligatorio faltante.\n");
        fprintf(stderr, "Uso correcto: %s <ruta archivo entrada> DEBUG(?)\n", argv[0]);
        return 1;  
    } else if (argc > 3){
        fprintf(stderr, "Error: Demasiados parámetros.\n");
        fprintf(stderr, "Uso correcto: %s <ruta archivo entrada> DEBUG(?)\n", argv[0]);
        return 1;
    } else if (argc == 3 && strcmp(argv[2], "DEBUG") == 0) {
        DEBUG_MODE = true;
    } 
 
    
    FILE *archivo = fopen(argv[1], "r");
    if (!archivo) {
        fprintf(stderr, "Error crítico: No se pudo abrir el archivo '%s'\n", argv[1]);
        return 1;
    }
    
    yyin = archivo; 

    yylex(); 

    fclose(yyin); 

    // Reporte final de tokens reconocidos y no reconocidos
    printf("==============================================\n");
    printf("Total de tokens válidos reconocidos: %d\n", tokens_reconocidos);
    printf("Total de secuencias no reconocidas:  %d\n", tokens_no_reconocidos);
    printf("==============================================\n");

    return 0;
}
