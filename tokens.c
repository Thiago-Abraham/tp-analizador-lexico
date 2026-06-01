#include <stdio.h>
#include < stdlib.h>
#include <string.h>
#include "tokens.h"

NodoToken* CrearNodo (const char* lexema, TipoToken tipo)
{
    NodoToken* nuevo= (NodoToken*) malloc(sizeof(NodoToken));

    if (nuevo == NULL)
    {
        fprintf(stderr, "Error: No fue posible asignar memoria al nodo.\n")
        return 1;
    }

    else
    {   /* strdup mide el tamaño de la cadena y hace un malloc automatico */
        nuevo->lexema = strdup(lexema); 
        nuevo->tipo = tipo;
        nuevo->contador = 1;
        nuevo->siguiente = NULL;
    }
    return nuevo;
}

void agregar_o_actualizar(NodoToken* lista, const char* lexema, TipoToken tipo) {
    if (lista == NULL) 
    {
        lista = crear_nodo(lexema, tipo);
        return;
    }

    NodoToken* actual = lista;
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
    actual->siguiente = crear_nodo(lexema, tipo);
}