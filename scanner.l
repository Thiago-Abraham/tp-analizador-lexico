%{

#include <stdio.h>
#include <stdbool.h>

bool DEBUG_MODE = false;

void log_debug(const char *mensaje, const char *texto) {
    if (DEBUG_MODE) {
        fprintf(stderr, "[DEBUG] %s: %s\n", mensaje, texto);
    } else {
        /*esto luego deberia ser la funcion de imprimir reporte que da todo el formato*/
        fprintf(stdout, "%s: %s\n", mensaje, texto);
    }
}

int tokens_reconocidos = 0;
int tokens_no_reconocidos = 0;

int largo = 0;

%}

%pointer
%option noyywrap

/*Condiciones de Arranque*/
%x tokenNoReconocido

/* Declaraciones */
centinela                                    [ \t\n]
nondigit                                     [_a-zA-Z]
constanteCaracter                            \'[a-zA-Z]\'
literalesCadena                              \".*\"
digit                                        [0-9]
decimal                                      [1-9]{digit}*
octalDigit                                   [0-7]
octal                                        0{octalDigit}*
hexaDigit                                    [0-9a-fA-F]
hexadecimal                                  0[xX]{hexaDigit}+
realPuntoInicial                             "."[0-9]+([eE][-+]?[0-9]+)?[dDfFlL]?
realConPunto                                 [0-9]+"."[0-9]*([eE][-+]?[0-9]+)?[dDfFlL]?
realSoloExponencial                          [0-9]+[eE][-+]?[0-9]+[dDfFlL]?
real                                         {realPuntoInicial}|{realConPunto}|{realSoloExponencial}
operators                                    (\[\]|[().*+\-~!&?|/%<>#,:]|->|\+\+|--|<<|>>|&&|\|\||[*/%+!|<=>&^]?=|<<=|>>=|##)
punctuators                                  (\[\]|[(){}*,:=#;]|\.\.\.)
reservadaEspecificadorAlmacenamiento         (auto|register|static|extern|typedef)
reservadaEspecificadorTipo                   (void|char|short|int|long|float|double|signed|unsigned)
reservadaDeCalificadoresDeTipo               (const|volatile)
reservadaDeStructUnion                       (struct|union)
reservadaDeEnumeracion                       (enum)
reservadaEtiqueta                            (case|default)
reservadaSeleccion                           (if|else|switch)
reservadaDeIteracion                         (do|while|for)
reservadaSalto                               (goto|continue|break|return) 
reservadaUnario                              (sizeof)
identifier                                   {nondigit}({nondigit}|{digit})*
noReconocido                                 [^ \t\n]

%%

{decimal}                                    { log_debug("Encontre una constante entera decimal", yytext); tokens_reconocidos++; }
{octal}                                      { log_debug("Constante entera octal", yytext); tokens_reconocidos++; }
{hexadecimal}                                { log_debug("Constante entera hexadecimal", yytext); tokens_reconocidos++; }
{real}                                       { log_debug("Constante real", yytext); tokens_reconocidos++; }
{constanteCaracter}                          { log_debug("Constante de caracter", yytext); tokens_reconocidos++; }
{literalesCadena}                            { log_debug("Literal de cadena o String", yytext); tokens_reconocidos++; }
{punctuators}                                { log_debug("Puntuador", yytext); tokens_reconocidos++; }
{operators}                                  { log_debug("Operador", yytext); tokens_reconocidos++; }
{reservadaEspecificadorAlmacenamiento}       { log_debug("Palabra reservada (Especificadores de Almacenamiento)", yytext); tokens_reconocidos++; }
{reservadaEspecificadorTipo}                 { log_debug("Palabra reservada (Especificadores de Tipo)", yytext); tokens_reconocidos++; }
{reservadaDeCalificadoresDeTipo}             { log_debug("Palabra reservada (Calificadores de Tipo)", yytext); tokens_reconocidos++; }
{reservadaDeStructUnion}                     { log_debug("Palabra reservada (Struct o Union)", yytext); tokens_reconocidos++; }
{reservadaDeEnumeracion}                     { log_debug("Palabra reservada (Enumeración)", yytext); tokens_reconocidos++; }
{reservadaEtiqueta}                          { log_debug("Palabra reservada (Etiqueta)", yytext); tokens_reconocidos++; }
{reservadaSeleccion}                         { log_debug("Palabra reservada (Selección)", yytext); tokens_reconocidos++; }
{reservadaDeIteracion}                       { log_debug("Palabra Reservada (Iteracion)",yytext); tokens_reconocidos++; }
{reservadaSalto}                             { log_debug("Palabra reservada (Salto)", yytext); tokens_reconocidos++; }
{reservadaUnario}                            { log_debug("Palabra reservada (Unario)", yytext); tokens_reconocidos++; }
{identifier}                                 { log_debug("Identificador", yytext); tokens_reconocidos++; }                                                 
{noReconocido}                               { if (DEBUG_MODE) fprintf(stderr, "[DEBUG] ERROR: Token no reconocido: %s", yytext);
                                                 tokens_no_reconocidos++;
                                                 BEGIN(tokenNoReconocido);  }
{centinela}                                  { }


 
 

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
    printf("\n==============================================\n");
    printf("Total de tokens válidos reconocidos: %d\n", tokens_reconocidos);
    printf("Total de secuencias no reconocidas:  %d\n", tokens_no_reconocidos);
    printf("==============================================\n");

    return 0;
}
