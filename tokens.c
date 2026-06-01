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