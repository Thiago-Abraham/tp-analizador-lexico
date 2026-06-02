#include <stdio.h>
#include < stdlib.h>
#include <string.h>
#include "tokens.h"

NodoToken* crearNodo (const char* lexema, TipoToken tipo, int linea, int columna)
{
    NodoToken* nuevo= (NodoToken*) malloc(sizeof(NodoToken));

    if (nuevo == NULL)
    {
        fprintf(stderr, "Error: No fue posible asignar memoria al nodo.\n")
        return NULL;
    }

    else
    {   /* strdup mide el tamaño de la cadena y hace un malloc automatico */
        nuevo->lexema = strdup(lexema); 
        nuevo->tipo = tipo;
        nuevo->contador = 1;
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

void imprimirLista (NodoToken* lista) {
    NodoToken* actual = lista;
    while (actual != NULL) {
        printf("Token: %s | Tipo: %d | Contador: %d | Linea: %d | Columna: %d\n", actual->lexema, actual->tipo, actual->contador, actual->linea, actual->columna);
        actual = actual->siguiente;
    }
}