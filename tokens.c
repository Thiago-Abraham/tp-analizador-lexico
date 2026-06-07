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
      //nuevo->contador = 1; 
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
        //Se elimina esta funcionalidad ya que al guardarlos en un mismo nodo, tienen la misma fila y columna
        /*
        if (strcmp(actual->lexema, lexema) == 0 && actual->tipo == tipo) {
            actual->contador++;
            return;
        }*/
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
        printf("Token: %s | Tipo: %d | Linea: %d | Columna: %d\n", actual->lexema, actual->tipo, actual->linea, actual->columna);
        actual = actual->siguiente;
    } 
}

void ordenarAlfabeticamente (NodoToken** lista) {
    
    //iteraciones varia entre 0 y 1 cada vez que se inicia el bucle para saber si se cambio algun nodo de lugar en dicha iteracion del bucle
    int iteraciones;
    NodoToken *actual = NULL;
    NodoToken *ant = NULL;
    NodoToken *ultimo = NULL;
    //primero pongo el bucle despues la condicion, siempre quiero evaluar almenos una vez
    do{
        iteraciones = 0;
        actual = *lista;
        ant = NULL;
    
        while(actual->siguiente != ultimo) {
            NodoToken *sig = actual->siguiente;
            // si el siguiente esta primero alfabeticamente
            if(strcasecmp(actual->lexema, sig->lexema) > 0){

                actual->siguiente = sig->siguiente;
                sig->siguiente = actual;
            
                if(ant == NULL){
                    // actual sigue siendo el primrer nodo de la lista asi que lo cambio por sig
                    *lista = sig;
                }
                else{
                    //Si no, reconecto el anterior al siguiente
                    ant->siguiente = sig;
                }

                ant = sig;
                iteraciones = 1;
            }
            else{
                // no hubo ningun cambio necesario entre actual y sig asi que avanzo en la lista normalmente
                ant = actual;
                actual = actual->siguiente;
            }
        }
        //Cuando termina el while de arriba, el actual siempre queda en el ultimo nodo que va a ser siempre el ultimo valor alfabetico, haciendo ultimo == actual hago que termine el while de arriba en el que ya fue ordenado como ultimo definitivo anteriormente. 
        ultimo = actual;

    }while(iteraciones); //si no hubo ninguna iteracion desde el principio, ya venia ordenada y termina todo el bucle
}

void ordenarPorLongitud (NodoToken** lista) {
    //FUNCION A DESARROLLAR

    
}

//HAY QUE SEGUIR DESARROLLANDO LAS FUNCIONES PARA CADA CASO, LEER CONSIGNA DE TP
void darFormatoALista (NodoToken** lista, TipoDeLista tipo) {
    switch (tipo) {
        case tipoListaIdentificadores:
            ordenarAlfabeticamente(lista);
            break;
        case tipoListaLiteralesCadena:
            ordenarPorLongitud(lista);
            break;
        case tipoListaPalabrasReservadas:
        case tipoListaConstantesHexadecimales:
        case tipoListaConstantesDecimales:
        case tipoListaConstantesOctales:
        case tipoListaConstantesReales:
        case tipoListaConstantesCaracter:
        case tipoListaOperadoresYPuntuadores:
        case tipoListaNoReconocidos:
    }
}