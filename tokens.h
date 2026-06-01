#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    tokenNoReconocido,
    tokenCentinela,

    /* Constantes numéricas */
    tokenDecimal,
    tokenOctal,
    tokenHexadecimal,
    tokenReal,
    tokenNumero,

    /* Literales */
    tokenConstanteCaracter,
    tokenLiteralCadena,

    /* Puntuadores y operadores (categorías generales) */
    tokenPuntuador,
    tokenOperador,

    /* Palabras reservadas por categoría */
    tokenReservadaEspecificadorAlmacenamiento,
    tokenReservadaEspecificadorTipo,
    tokenReservadaCalificadorTipo,
    tokenReservadaStructUnion,
    tokenReservadaEnumeracion,
    tokenReservadaEtiqueta,
    tokenReservadaSeleccion,
    tokenReservadaIteracion,
    tokenReservadaSalto,
    tokenReservadaUnario,

    /* Identificador */
    tokenIdentificador,

    /* Operadores / símbolos específicos (mantener compatibilidad con uso previo) */
    tokenSuma,
    tokenResta,
    tokenMultiplicacion,
    tokenDivision,
    tokenParentesisAbre,
    tokenParentesisCierra
} TipoToken;

typedef struct NodoToken {
    TipoToken tipo;                         
    char* lexema;                         // Puntero al texto del token
    int contador;

    struct NodoToken* siguiente;
} NodoToken;

#endif // TOKENS_H 