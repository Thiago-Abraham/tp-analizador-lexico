#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "tokens.h"

static int sumarListaDecimales(NodoToken* lista);
static float retornarParteEntera(NodoToken* Lista);
static float retornarMantisa(NodoToken* Lista);
static int estaEnListaQueRequiereRepeticion(TipoDeLista tipoLista);

NodoToken* crearNodo (const char* lexema, TipoToken tipo, int linea, int columna)
{
    NodoToken* nuevo= (NodoToken*) malloc(sizeof(NodoToken));

    if (nuevo == NULL)
    {
        fprintf(stderr, "Error: No fue posible asignar memoria al nodo.\n");
        return NULL;
    }
    
    nuevo->tipo = tipo;
    nuevo->lexema = strdup(lexema); 
    nuevo->contador = 1; 
    nuevo->longitud = strlen(lexema);
    nuevo->linea = linea;
    nuevo->columna = columna;
    nuevo->siguiente = NULL;
    
    return nuevo;
}

void agregarNodoALista(NodoToken** lista, const char* lexema, TipoToken tipo, TipoDeLista tipoLista, int linea, int columna) { // Doble puntero en C es pasaje por referencia en C++
    if (*lista == NULL) 
    {
        *lista = crearNodo(lexema, tipo, linea, columna);
        return;
    }

    NodoToken* actual = *lista;
    int requiereRepeticion = estaEnListaQueRequiereRepeticion(tipoLista);
    while (actual != NULL) 
    {
        if(requiereRepeticion){
            if (actual->siguiente == NULL) break;
            actual = actual->siguiente;
        } else {
            if (strcmp(actual->lexema, lexema) == 0 && actual->tipo == tipo) {
                actual->contador++;
                return;
            }
            if (actual->siguiente == NULL) break;
            actual = actual->siguiente;
        }
    }
    NodoToken* nuevo = crearNodo(lexema, tipo, linea, columna);
    if (nuevo == NULL) return;
    actual->siguiente = nuevo;

}

void liberarLista (NodoToken** lista) {
    NodoToken* actual = *lista;
    while (actual != NULL) {
        NodoToken* aux = actual->siguiente;
        free(actual->lexema); 
        free(actual); 
        actual = aux; 
    }
    *lista = NULL;
}


// imprimirLista segun el tipo de lista
void imprimirLista (NodoToken* lista, TipoDeLista tipo) {
    NodoToken* actual = lista;
    switch (tipo) {
        case tipoListaIdentificadores:
            while (actual != NULL) {
                printf("%s: aparece %d %s\n", actual->lexema, actual->contador, actual->contador == 1 ? "vez" : "veces");
                actual = actual->siguiente;
            } 
            break;
        case tipoListaLiteralesCadena:
            while (actual != NULL) {
                printf("%s: longitud %zu\n", actual->lexema, actual->longitud);
                actual = actual->siguiente;
            } 
            break;
        case tipoListaPalabrasReservadas:
            while (actual != NULL) {
                printf("%s: linea %d, columna %d\n", actual->lexema, actual->linea, actual->columna);
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesHexadecimales:
            while (actual != NULL) {
                printf("%s: valor entero decimal %lu\n", actual->lexema, strtoul(actual->lexema, NULL, 16));
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesDecimales:
            while (actual != NULL) {
                printf("%s: valor %d\n", actual->lexema, (int)strtol(actual->lexema, NULL, 10));
                actual = actual->siguiente;
            } 
            fprintf(stdout, "Total acumulado de sumar todas las constantes decimales: %d\n", sumarListaDecimales(lista));
            break;
        case tipoListaConstantesOctales:
            while (actual != NULL) {
                printf("%s: valor entero decimal %lu\n", actual->lexema, strtoul(actual->lexema, NULL, 8));
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesReales:
            while (actual != NULL) {
                float entera = retornarParteEntera (actual);
                float mantisa = retornarMantisa (actual);
                printf("%s: parte entera %g, mantisa %g\n", actual->lexema, entera, mantisa);
                actual = actual->siguiente;
            }
            break; 
        case tipoListaConstantesCaracter: {//Orden de aparicion
            int contador = 1;
            while (actual != NULL) {
                printf("%d) %s\n", contador, actual->lexema);
                actual = actual->siguiente;
                contador++;
            }
            break;
        }
        case tipoListaOperadoresYPuntuadores:
        while (actual != NULL) {
            printf("%s: aparece %d %s\n", actual->lexema, actual->contador, actual->contador == 1 ? "vez" : "veces");
            actual = actual->siguiente;
        } 
        break;
        case tipoListaNoReconocidos: 
            while (actual != NULL) {
                printf("%s: linea %d, columna %d\n", actual->lexema, actual->linea, actual->columna);
                actual = actual->siguiente;
            }
            break; 
    }
}



//FUNCIONES PARA CONSTANTES ENTERAS
int sumarListaDecimales (NodoToken* lista){
    if (lista == NULL) return 0;
    int suma = 0;
    NodoToken* actual = lista;
    while(actual != NULL){
        suma += (int)strtol(actual->lexema, NULL, 10);
        actual = actual->siguiente;
    }
    return suma;
}


//FUNCIONES DE COMPARACION

// Comparación por longitud
int compararPorLongitud(NodoToken *a, NodoToken *b) {
    return (int)a->longitud - (int)b->longitud; // ascendente: menor longitud primero
    // return b->longitud - a->longitud; // descendente: mayor longitud primero
}

// Comparación alfabética
int compararAlfabetico(NodoToken *a, NodoToken *b) {
    return strcasecmp(a->lexema, b->lexema); //si b va antes alfabeticamente que a devuelve un num > 0
}


//OPTIMIZAMOS PARA QUE ORDENAR UNA LISTA RECIBA LA COMPARACION COMO PARAMETRO (corte paradigma funcional)
void ordenarListaPor(NodoToken **lista, int (*comparar)(NodoToken*, NodoToken*)) {
    int swapped;
    NodoToken *actual = NULL;
    NodoToken *ant = NULL;
    NodoToken *ultimo = NULL;
    
    if(*lista == NULL){
        return;
    }
    do {
        swapped = 0;
        actual = *lista;
        ant = NULL;

        while (actual->siguiente != ultimo) {
            NodoToken *sig = actual->siguiente;

            // en vez de la comparación hardcodeada, llamamos al puntero funcion de comparacion
            if (comparar(actual, sig) > 0) {
                actual->siguiente = sig->siguiente;
                sig->siguiente = actual;
                if (ant == NULL) {
                    *lista = sig;
                } else {
                    ant->siguiente = sig;
                }
                ant = sig;
                swapped = 1;
            } else {
                ant = actual;
                actual = actual->siguiente;
            }
        }
        ultimo = actual;

    } while (swapped);
}

static int estaEnListaQueRequiereRepeticion(TipoDeLista tipoLista) {
    switch (tipoLista) {
        case tipoListaConstantesCaracter:
        case tipoListaConstantesDecimales:
        case tipoListaConstantesHexadecimales:
        case tipoListaConstantesOctales:
        case tipoListaConstantesReales:
        case tipoListaNoReconocidos:
        case tipoListaPalabrasReservadas:
            return 1;
        default:
            return 0;
    }
}

//Retornar parte entera
float retornarParteEntera (NodoToken* Lista){
    int entero = (int) atof(Lista->lexema); // hace atof al lista lexema lo que hace que funcione como numero (float en este caso) y lo convierte a int para quedarse solo con la parte entera
    return (float)entero; // returnea l parte entera ya obtenida trasnformada en float,es decir entero.0000..... 
}

//Retornar mantisa
float retornarMantisa (NodoToken* Lista){
    float mantisa = atof(Lista->lexema) - retornarParteEntera(Lista); // hace atof al lexema para que deje de ser char y leresta la parte entera transformada en float usando la funcion anterior
    return mantisa; //returnea 
}
//HAY QUE SEGUIR DESARROLLANDO LAS FUNCIONES PARA CADA CASO, LEER CONSIGNA DE TP
//No hace falta formatear las listas Hexadecimales ni octales
void darFormatoALista (NodoToken** lista, TipoDeLista tipo){
    switch (tipo) {
        case tipoListaIdentificadores:
            ordenarListaPor(lista, compararAlfabetico);
            break;
        case tipoListaLiteralesCadena:
            ordenarListaPor(lista, compararPorLongitud);
            break;
        case tipoListaPalabrasReservadas: //Orden de llegada
        case tipoListaConstantesDecimales: //Orden de llegada
        case tipoListaConstantesReales: //Orden de llegada
        case tipoListaConstantesCaracter: //Orden de llegada
        case tipoListaOperadoresYPuntuadores: //Orden de lleagda    
        case tipoListaNoReconocidos:  //Sin orden especifico
        case tipoListaConstantesHexadecimales: //Orden de llegada
        case tipoListaConstantesOctales: //Orden de llegada
        break;
    }
}