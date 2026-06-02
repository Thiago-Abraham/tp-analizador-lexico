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

#endif // TOKENS_H 