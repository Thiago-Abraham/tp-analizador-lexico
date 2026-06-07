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

// Listas para almacenar los tokens reconocidos
NodoToken* listaIdentificadores = NULL;
NodoToken* listaLiteralesCadena = NULL;
NodoToken* listaReservadaAlmacenamiento = NULL;
NodoToken* listaReservadaTipo = NULL;
NodoToken* listaReservadaCalificadores = NULL;
NodoToken* listaReservadaStructUnion = NULL;
NodoToken* listaReservadaEnumeracion = NULL;
NodoToken* listaReservadaEtiqueta = NULL;
NodoToken* listaReservadaSeleccion = NULL;
NodoToken* listaReservadaIteracion = NULL;
NodoToken* listaReservadaSalto = NULL;
NodoToken* listaReservadaUnario = NULL;
NodoToken* listaConstantesDecimales = NULL;
NodoToken* listaConstantesHexadecimales = NULL;
NodoToken* listaConstantesOctales = NULL;
NodoToken* listaConstantesReales = NULL;
NodoToken* listaConstantesCaracter = NULL;
NodoToken* listaOperadoresYPuntuadores = NULL;
NodoToken* listaNoReconocidos = NULL;

void imprimirReporte(const char *mensaje, const char *lexema, TipoToken tipo, NodoToken** lista, TipoDeLista tipoLista, int linea, int columna, char error /*pasar E si es error A si es acierto (realmente solo importa E)*/) {
    if (DEBUG_MODE && (error == 'E')) {
        fprintf(stderr, COLOR_ROJO "[DEBUG] ERROR: %s: %s: linea %d, columna %d" COLOR_RESET "\n", mensaje, lexema, linea, columna);
    } else if (DEBUG_MODE) {
        fprintf(stderr, COLOR_VERDE "[DEBUG] %s: %s: linea %d, columna %d" COLOR_RESET "\n", mensaje, lexema, linea, columna);
    } else {
        /*esto luego deberia ser la funcion de imprimir reporte que da todo el formato*/
        agregarNodoALista(lista, lexema, tipo, linea, columna);
        //fprintf(stdout, "%s: %s: linea %d, columna %d\n", mensaje, lexema, linea, columna);
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

{decimal}                                    { imprimirReporte("Constante entera decimal", yytext, tokenDecimal, &listaConstantesDecimales, tipoListaConstantesDecimales, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{octal}                                      { imprimirReporte("Constante entera octal", yytext, tokenOctal, &listaConstantesOctales, tipoListaConstantesOctales, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{hexadecimal}                                { imprimirReporte("Constante entera hexadecimal", yytext, tokenHexadecimal, &listaConstantesHexadecimales, tipoListaConstantesHexadecimales, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{real}                                       { imprimirReporte("Constante real", yytext, tokenReal, &listaConstantesReales, tipoListaConstantesReales, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{constanteCaracter}                          { imprimirReporte("Constante de caracter", yytext, tokenConstanteCaracter, &listaConstantesCaracter, tipoListaConstantesCaracter, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{literalesCadena}                            { imprimirReporte("Literal de cadena o String", yytext, tokenLiteralesCadena, &listaLiteralesCadena, tipoListaLiteralesCadena, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{punctuators}                                { imprimirReporte("Puntuador", yytext, tokenPunctuators, &listaOperadoresYPuntuadores, tipoListaOperadoresYPuntuadores, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{operators}                                  { imprimirReporte("Operador", yytext, tokenOperators, &listaOperadoresYPuntuadores, tipoListaOperadoresYPuntuadores, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEspecificadorAlmacenamiento}       { imprimirReporte("Palabra reservada (Especificadores de Almacenamiento)", yytext, tokenReservadaEspecificadorAlmacenamiento, &listaReservadaAlmacenamiento, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEspecificadorTipo}                 { imprimirReporte("Palabra reservada (Especificadores de Tipo)", yytext, tokenReservadaEspecificadorTipo, &listaReservadaTipo, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeCalificadoresDeTipo}             { imprimirReporte("Palabra reservada (Calificadores de Tipo)", yytext, tokenReservadaDeCalificadoresDeTipo, &listaReservadaCalificadores, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeStructUnion}                     { imprimirReporte("Palabra reservada (Struct o Union)", yytext, tokenReservadaDeStructUnion, &listaReservadaStructUnion, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeEnumeracion}                     { imprimirReporte("Palabra reservada (Enumeración)", yytext, tokenReservadaDeEnumeracion, &listaReservadaEnumeracion, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaEtiqueta}                          { imprimirReporte("Palabra reservada (Etiqueta)", yytext, tokenReservadaEtiqueta, &listaReservadaEtiqueta, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaSeleccion}                         { imprimirReporte("Palabra reservada (Selección)", yytext, tokenReservadaSeleccion, &listaReservadaSeleccion, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaDeIteracion}                       { imprimirReporte("Palabra Reservada (Iteracion)",yytext, tokenReservadaDeIteracion, &listaReservadaIteracion, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaSalto}                             { imprimirReporte("Palabra reservada (Salto)", yytext, tokenReservadaSalto, &listaReservadaSalto, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{reservadaUnario}                            { imprimirReporte("Palabra reservada (Unario)", yytext, tokenReservadaUnario, &listaReservadaUnario, tipoListaPalabrasReservadas, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }
{identifier}                                 { imprimirReporte("Identificador", yytext, tokenIdentifier, &listaIdentificadores, tipoListaIdentificadores, linea_actual, columna_actual, 'A'); tokens_reconocidos++; columna_actual += yyleng; }                                                 
{noReconocido}                               { imprimirReporte("Token no reconocido", yytext, tokenNoReconocido, &listaNoReconocidos, tipoListaNoReconocidos, linea_actual, columna_actual, 'E'); tokens_no_reconocidos++; columna_actual += yyleng; }
{tabulacion}                                 { columna_actual += yyleng; }
{saltoDeLinea}                               { linea_actual += yyleng; columna_actual = INICIO_CONTEO_COLUMNA; }

%%

void imprimirListaFormateada () {
    fprintf(stdout, "* Listado de identificadores encontrados:\n");
    if (listaIdentificadores == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaIdentificadores, tipoListaIdentificadores);
        imprimirLista(listaIdentificadores);
        liberarLista(listaIdentificadores);

    }
    
    fprintf(stdout, "\n* Listado de literales cadena encontrados:\n");  
    if (listaLiteralesCadena == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaLiteralesCadena, tipoListaLiteralesCadena);
        imprimirLista(listaLiteralesCadena);
        liberarLista(listaLiteralesCadena);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (clase de almacenamiento):\n");  
    if (listaReservadaAlmacenamiento == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaAlmacenamiento, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaAlmacenamiento);
        liberarLista(listaReservadaAlmacenamiento);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (especificadores de tipo):\n");  
    if (listaReservadaTipo == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaTipo, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaTipo);
        liberarLista(listaReservadaTipo);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (calificadores de tipo):\n");  
    if (listaReservadaCalificadores == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaCalificadores, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaCalificadores);
        liberarLista(listaReservadaCalificadores);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (struct o union):\n");  
    if (listaReservadaStructUnion == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaStructUnion, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaStructUnion);
        liberarLista(listaReservadaStructUnion);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (enumeracion):\n");  
    if (listaReservadaEnumeracion == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaEnumeracion, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaEnumeracion);
        liberarLista(listaReservadaEnumeracion);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (etiqueta):\n");  
    if (listaReservadaEtiqueta == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaEtiqueta, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaEtiqueta);
        liberarLista(listaReservadaEtiqueta);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (seleccion):\n");  
    if (listaReservadaSeleccion == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaSeleccion, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaSeleccion);
        liberarLista(listaReservadaSeleccion);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (iteracion):\n");  
    if (listaReservadaIteracion == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaIteracion, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaIteracion);
        liberarLista(listaReservadaIteracion);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (salto):\n");  
    if (listaReservadaSalto == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaSalto, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaSalto);
        liberarLista(listaReservadaSalto);

    }
    
    fprintf(stdout, "\n* Listado de palabras reservadas (unario):\n");  
    if (listaReservadaUnario == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaReservadaUnario, tipoListaPalabrasReservadas);
        imprimirLista(listaReservadaUnario);
        liberarLista(listaReservadaUnario);

    }
    
    fprintf(stdout, "\n* Listado de constantes enteras decimales:\n");  
    if (listaConstantesDecimales == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaConstantesDecimales, tipoListaConstantesDecimales);
        imprimirLista(listaConstantesDecimales);
        liberarLista(listaConstantesDecimales);

    }
    
    fprintf(stdout, "\n* Listado de constantes enteras hexadecimales:\n");  
    if (listaConstantesHexadecimales == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaConstantesHexadecimales, tipoListaConstantesHexadecimales);
        imprimirLista(listaConstantesHexadecimales);
        liberarLista(listaConstantesHexadecimales);

    }
    
    fprintf(stdout, "\n* Listado de constantes enteras octales:\n");  
    if (listaConstantesOctales == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaConstantesOctales, tipoListaConstantesOctales);
        imprimirLista(listaConstantesOctales);
        liberarLista(listaConstantesOctales);

    }
    
    fprintf(stdout, "\n* Listado de constantes reales:\n");  
    if (listaConstantesReales == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaConstantesReales, tipoListaConstantesReales);
        imprimirLista(listaConstantesReales);
        liberarLista(listaConstantesReales);

    }
    
    fprintf(stdout, "\n* Listado de constantes caracter enumerados:\n");  
    if (listaConstantesCaracter == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaConstantesCaracter, tipoListaConstantesCaracter);
        imprimirLista(listaConstantesCaracter);
        liberarLista(listaConstantesCaracter);

    }
    
    fprintf(stdout, "\n* Listado de operadores/caracteres de puntuación:\n");  
    if (listaOperadoresYPuntuadores == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaOperadoresYPuntuadores, tipoListaOperadoresYPuntuadores);
        imprimirLista(listaOperadoresYPuntuadores);
        liberarLista(listaOperadoresYPuntuadores);

    }
    
    fprintf(stdout, "\n* Listado de cadenas no reconocidas:\n");  
    if (listaNoReconocidos == NULL) {
        fprintf(stdout, "-\n");
    } else {
        //darFormatoALista(&listaNoReconocidos, tipoListaNoReconocidos);
        imprimirLista(listaNoReconocidos);
        liberarLista(listaNoReconocidos);
}
    }
    

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

    // Reporte final de tokens reconocidos y no reconocidos en modo DEBUG
    if (DEBUG_MODE) {
        printf("\n==============================================\n");
        printf("Total de tokens válidos reconocidos: %d\n", tokens_reconocidos);
        printf("Total de tokens error no reconocidos:  %d\n", tokens_no_reconocidos);
        printf("==============================================\n");
    } else {
        imprimirListaFormateada();
    }

    return 0;
}
