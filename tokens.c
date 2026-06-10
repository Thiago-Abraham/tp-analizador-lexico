#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"


NodoToken* crearNodo (const char* lexema, TipoToken tipo, int linea, int columna)
{
    NodoToken* nuevo= (NodoToken*) malloc(sizeof(NodoToken));

    if (nuevo == NULL)
    {
        fprintf(stderr, "Error: No fue posible asignar memoria al nodo.\n");
        return NULL;
    }

    else
    {   /* strdup mide el tamaño de la cadena y hace un malloc automatico */
        nuevo->tipo = tipo;
        nuevo->lexema = strdup(lexema); 
        nuevo->contador = 1; 
        nuevo->longitud = strlen(lexema);
        nuevo->linea = linea;
        nuevo->columna = columna;
        nuevo->siguiente = NULL;
    }
    return nuevo;
}

void agregarNodoALista(NodoToken** lista, const char* lexema, TipoToken tipo, int linea, int columna) { // Doble puntero en C es pasaje por referencia en C++
    if (*lista == NULL) 
    {
        *lista = crearNodo(lexema, tipo, linea, columna);
        return;
    }

    NodoToken* actual = *lista;
    while (actual != NULL) 
    {
        //Se modifica esta funcionalidad ya que al guardarlos en un mismo nodo, tienen la misma fila y columna, la solucion planteada es aumentar el contador pero dejarlos por separado de todos modos, despues se formatea para eliminar repetidos en caso de que se pida que no salgan las repeticiones por pantalla
        if (strcmp(actual->lexema, lexema) == 0 && actual->tipo == tipo) {
            actual->contador++;
            return;
        }
        if (actual->siguiente == NULL) 
        {
            break;              // Llegamos al final de la lista sin encontrar el token, así que salimos del bucle para agregar uno nuevo, pero sin avanzar a un nodo nulo.
        }
        actual = actual->siguiente;
    }
    actual->siguiente = crearNodo(lexema, tipo, linea, columna);
}

void liberarLista (NodoToken* lista) {
    NodoToken* actual = lista;
    while (actual != NULL) {
        NodoToken* aux = actual->siguiente;
        free(actual->lexema); 
        free(actual); 
        actual = aux; 
    }
}


// imprimirLista segun el tipo de lista
void imprimirLista (NodoToken* lista, TipoDeLista tipo) {
    NodoToken* actual = lista;
    switch (tipo) {
        case tipoListaIdentificadores:
            ordenarListaPor(lista, compararAlfabetico); //cambiar esto
            break;
        case tipoListaLiteralesCadena:
            ordenarListaPor(lista, compararPorLongitud); //cambiar esto
            break;
        case tipoListaPalabrasReservadas:
        case tipoListaConstantesHexadecimales:
            while (actual != NULL) {
                printf("%s: valor entero decimal %d\n", actual->lexema, strtoul(actual->lexema, NULL, 16));
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesDecimales:
            while (actual != NULL) {
                printf("%s: valor: %s", actual->lexema, actual->lexema);
                actual = actual->siguiente;
            } 
            fprintf(stdout, "Total acumulado de sumar todas las constantes decimales: %d ", sumarListaDecimales(lista));
            break;
        case tipoListaConstantesOctales:
            while (actual != NULL) {
                printf("%s: valor entero decimal %d\n", actual->lexema, strtoul(actual->lexema, NULL, 8));
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesReales:
        case tipoListaConstantesCaracter: //Orden de aparicion
        case tipoListaOperadoresYPuntuadores:
        case tipoListaNoReconocidos:
    }
/*     if(lista->tipo == "EnteroDecimal")
    while (actual != NULL) {
        printf("Token: %s | Tipo: %d | Linea: %d | Columna: %d\n", actual->lexema, actual->tipo, actual->linea, actual->columna);
        actual = actual->siguiente;
    }  */
}



//FUNCIONES PARA CONSTANTES ENTERAS
int sumarListaDecimales (NodoToken* lista){
    int suma = 0;
    NodoToken* actual = lista;
    while(actual->siguiente != NULL){
        suma += strtof(lista->lexema, NULL);
        actual = actual->siguiente;
    }
    return suma;
}


//FUNCIONES DE COMPARACION

// Comparación por longitud
int compararPorLongitud(NodoToken *a, NodoToken *b) {
    return b->longitud - a->longitud; // descendente: mayor longitud primero
    // return a->longitud - b->longitud; // ascendente: menor longitud primero
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
    
    if(*lista = NULL){
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


//HAY QUE SEGUIR DESARROLLANDO LAS FUNCIONES PARA CADA CASO, LEER CONSIGNA DE TP
//No hace falta formatear las listas Hexadecimales ni octales
void darFormatoALista (NodoToken** lista, TipoDeLista tipo) {
    switch (tipo) {
        case tipoListaIdentificadores:
            ordenarListaPor(&lista, compararAlfabetico);
            break;
        case tipoListaLiteralesCadena:
            ordenarListaPor(&lista, compararPorLongitud);
            break;
        case tipoListaPalabrasReservadas:
        case tipoListaConstantesDecimales:
        case tipoListaConstantesReales:
        case tipoListaConstantesCaracter: //Orden de aparicion
        case tipoListaOperadoresYPuntuadores:
        case tipoListaNoReconocidos:
    }
}