#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
tokenTabulacion,
tokenSaltoDeLinea,                          
tokenNondigit,
tokenConstanteCaracter,
tokenLiteralesCadena,
tokenDigit,
tokenDecimal,
tokenOctalDigit,
tokenOctal,
tokenHexaDigit,
tokenHexadecimal,
tokenRealPuntoInicial,
tokenRealConPunto,
tokenRealSoloExponencial,
tokenReal,
tokenOperators,
tokenPunctuators,
tokenReservadaEspecificadorAlmacenamiento,
tokenReservadaEspecificadorTipo,
tokenReservadaDeCalificadoresDeTipo,
tokenReservadaDeStructUnion,
tokenReservadaDeEnumeracion,
tokenReservadaEtiqueta,
tokenReservadaSeleccion,
tokenReservadaDeIteracion,
tokenReservadaSalto,
tokenReservadaUnario,
tokenIdentifier,
tokenNoReconocido   
} TipoToken;

typedef struct NodoToken {
    TipoToken tipo;                         
    char* lexema;                         // Puntero al texto del token
    int contador;
    int linea;
    int columna;

    struct NodoToken* siguiente;
} NodoToken;

NodoToken* crearNodo(const char* lexema, TipoToken tipo, int linea, int columna);
void agregarNodoALista(NodoToken** lista, const char* lexema, TipoToken tipo, int linea, int columna);
void liberarLista(NodoToken* lista);
void imprimirLista(NodoToken* lista);

#endif // TOKENS_H 