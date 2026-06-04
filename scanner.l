%{

#include <stdio.h>
#include <stdbool.h>
#include "tokens.h"

#define COLOR_VERDE  "\033[32m"
#define COLOR_ROJO   "\033[31m"
#define COLOR_RESET  "\033[0m"
#define INICIO_CONTEO_LINEA 1
#define INICIO_CONTEO_COLUMNA 1

int linea_actual = INICIO_CONTEO_LINEA, columna_actual = INICIO_CONTEO_COLUMNA;

bool DEBUG_MODE = false;

NodoToken* listaTokens = NULL; // Lista para almacenar los tokens reconocidos

void imprimirReporte(const char *mensaje, const char *texto, int linea, int columna, char error /*pasar E si es error*/) {
    if (DEBUG_MODE && (error == 'E')) {
        fprintf(stderr, COLOR_ROJO "[DEBUG] ERROR: %s: %s: linea %d, columna %d" COLOR_RESET "\n", mensaje, texto, linea, columna);
    } else if (DEBUG_MODE) {
        fprintf(stderr, COLOR_VERDE "[DEBUG] %s: %s: linea %d, columna %d" COLOR_RESET "\n", mensaje, texto, linea, columna);
    } else {
        /*esto luego deberia ser la funcion de imprimir reporte que da todo el formato*/

        fprintf(stdout, "%s: %s: linea %d, columna %d\n", mensaje, texto, linea, columna);
        imprimirLista(listaTokens); 
        liberarLista(listaTokens);
    }
}

int tokens_reconocidos = 0;
int tokens_no_reconocidos = 0;

int largo = 0;

%}

%pointer
%option noyywrap

/* Declaraciones */
tabulacion                                   [ \t]+
saltoDeLinea                                 \n+
nondigit                                     [_a-zA-Z]
constanteCaracter                            \'([a-zA-Z]|(\\n)|(\\t)|(\\{octal})|(\\{hexadecimal}))\'
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
noReconocido                                 [^ \t\n()\[\]{}.*+\-~!&?|/%<>#,:=;^"']+

%%

{decimal}                                    { imprimirReporte("Encontre una constante entera decimal", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{octal}                                      { imprimirReporte("Constante entera octal", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{hexadecimal}                                { imprimirReporte("Constante entera hexadecimal", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{real}                                       { imprimirReporte("Constante real", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{constanteCaracter}                          { imprimirReporte("Constante de caracter", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{literalesCadena}                            { imprimirReporte("Literal de cadena o String", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{punctuators}                                { imprimirReporte("Puntuador", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{operators}                                  { imprimirReporte("Operador", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEspecificadorAlmacenamiento}       { imprimirReporte("Palabra reservada (Especificadores de Almacenamiento)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEspecificadorTipo}                 { imprimirReporte("Palabra reservada (Especificadores de Tipo)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeCalificadoresDeTipo}             { imprimirReporte("Palabra reservada (Calificadores de Tipo)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeStructUnion}                     { imprimirReporte("Palabra reservada (Struct o Union)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeEnumeracion}                     { imprimirReporte("Palabra reservada (Enumeración)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEtiqueta}                          { imprimirReporte("Palabra reservada (Etiqueta)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaSeleccion}                         { imprimirReporte("Palabra reservada (Selección)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeIteracion}                       { imprimirReporte("Palabra Reservada (Iteracion)",yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaSalto}                             { imprimirReporte("Palabra reservada (Salto)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaUnario}                            { imprimirReporte("Palabra reservada (Unario)", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{identifier}                                 { imprimirReporte("Identificador", yytext, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }                                                 
{noReconocido}                               { imprimirReporte("Token no reconocido", yytext, linea_actual, columna_actual, 'E'); tokens_no_reconocidos++; columna_actual += yyleng; }
{tabulacion}                                 { columna_actual += yyleng; }
{saltoDeLinea}                               { linea_actual += yyleng; columna_actual = INICIO_CONTEO_COLUMNA; }

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
