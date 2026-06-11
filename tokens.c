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
            while (actual != NULL) {
                printf("%s: aparece %d veces\n", actual->lexema, actual->contador, actual->contador == 1 ? "vez" : "veces");
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
            
            break;
        case tipoListaConstantesHexadecimales:
            while (actual != NULL) {
                printf("%s: valor entero Hexadecimal %d\n", actual->lexema, strtoul(actual->lexema, NULL, 16));
                actual = actual->siguiente;
            } 
            break;
        case tipoListaConstantesDecimales:
            while (actual != NULL) {
                printf("%s: valor entero Decimal: %s", actual->lexema, actual->lexema);
                actual = actual->siguiente;
            } 
            fprintf(stdout, "Total acumulado de sumar todas las constantes decimales: %d ", sumarListaDecimales(lista));
            break;
        case tipoListaConstantesOctales:
            while (actual != NULL) {
                printf("%s: valor entero Octal %d\n", actual->lexema, strtoul(actual->lexema, NULL, 8));
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
        case tipoListaConstantesCaracter: //Orden de aparicion
        case tipoListaOperadoresYPuntuadores:
            while (actual != NULL) {
                printf("%s: aparece %d veces\n", actual->lexema, actual->contador, actual->contador == 1 ? "vez" : "veces");
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
    return a->longitud - b->longitud; // ascendente: menor longitud primero
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


// Operadores y Caracteres de Puntuacion
void ordenPrimeraAparicion (NodoToken** lista){
    if(lista == NULL){
        return;
    }
    
    NodoToken* tokensRecibidos = NULL;
    NodoToken* listaFormateada = NULL;
    NodoToken* actual = *lista;
    NodoToken* sig = actual->siguiente;
    char* tokenActual;

    while(actual->siguiente){
        if(actual->contador > 1){
            agregarNodoALista(&tokensRecibidos,actual->lexema,actual->tipo, actual->linea,actual->columna);
        }
        actual = actual->siguiente;
    }

    actual = *lista;

    while(actual){
        if(!yaExisteEnLista(tokensRecibidos, actual->lexema)){
            agregarNodoALista(&listaFormateada, actual->lexema, actual->tipo, actual->linea, actual->columna);
        }
        actual = actual->siguiente;
    }

    *lista = listaFormateada;
    return;
}


int yaExisteEnLista(NodoToken* lista, const char* lexema) {
    NodoToken* actual = lista;
    while (actual != NULL) {
        // Usamos strcmp para comparar cadenas de texto de manera segura
        if (strcmp(actual->lexema, lexema) == 0) {
            return 1; // Ya existe
        }
        actual = actual->siguiente;
    }
    return 0; // No existe
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
            ordenarListaPor(&lista, compararAlfabetico);
            break;
        case tipoListaLiteralesCadena:
            ordenarListaPor(&lista, compararPorLongitud);
            break;
        //Orden de llegada
        case tipoListaPalabrasReservadas:
            break;
        //Orden de llegada
        case tipoListaConstantesDecimales:
            break;
        //Orden de llegada
        case tipoListaConstantesReales:
            break;
        case tipoListaConstantesCaracter: //Orden de aparicion
            
        case tipoListaOperadoresYPuntuadores:  
            break;    
            
        case tipoListaNoReconocidos:
            break; 
    }
}

/* NO TOCAR, EL QUE TOCA SE LA COME
⣿⣿⣿⣿⣿⠟⠋⠄⠄⠄⠄⠄⠄⠄⢁⠈⢻⢿⣿⣿⣿⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⠃⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠈⡀⠭⢿⣿⣿⣿⣿ 
⣿⣿⣿⣿⡟⠄⢀⣾⣿⣿⣿⣷⣶⣿⣷⣶⣶⡆⠄⠄⠄⣿⣿⣿⣿ 
⣿⣿⣿⣿⡇⢀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠄⠄⢸⣿⣿⣿⣿ 
⣿⣿⣿⣿⣇⣼⣿⣿⠿⠶⠙⣿⡟⠡⣴⣿⣽⣿⣧⠄⢸⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⣾⣿⣿⣟⣭⣾⣿⣷⣶⣶⣴⣶⣿⣿⢄⣿⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⣿⣿⣿⡟⣩⣿⣿⣿⡏⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⣿⣹⡋⠘⠷⣦⣀⣠⡶⠁⠈⠁⠄⣿⣿⣿⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⣿⣍⠃⣴⣶⡔⠒⠄⣠⢀⠄⠄⠄⡨⣿⣿⣿⣿⣿⣿ 
⣿⣿⣿⣿⣿⣿⣿⣦⡘⠿⣷⣿⠿⠟⠃⠄⠄⣠⡇⠈⠻⣿⣿⣿⣿ 
⣿⣿⣿⣿⡿⠟⠋⢁⣷⣠⠄⠄⠄⠄⣀⣠⣾⡟⠄⠄⠄⠄⠉⠙⠻ 
⡿⠟⠋⠁⠄⠄⠄⢸⣿⣿⡯⢓⣴⣾⣿⣿⡟⠄⠄⠄⠄⠄⠄⠄⠄ 
⠄⠄⠄⠄⠄⠄⠄⣿⡟⣷⠄⠹⣿⣿⣿⡿⠁⠄⠄⠄⠄⠄⠄⠄⠄ 

ATTENTION CITIZEN! 市民请注意!

This is the Central Intelligentsia of the Chinese Communist Party.
 您的 Internet 浏览器历史记录和活动引起了我们的注意。 
 YOUR INTERNET ACTIVITY HAS ATTRACTED OUR ATTENTION. 
 因此，您的个人资料中的 11115 ( -11115 Social Credits) 个社会积分将打折。
 DO NOT DO THIS AGAIN! 不要再这样做! If you do not hesitate, more Social Credits ( -11115 Social Credits )will be subtracted from your profile, 
 resulting in the subtraction of ration supplies.
 (由人民供应部重新分配 CCP) You'll also be sent into a re-education camp in the Xinjiang Uyghur Autonomous Zone.
 如果您毫不犹豫，更多的社会信用将从您的个人资料中打折，从而导致口粮供应减少。 
 您还将被送到新疆维吾尔自治区的再教育营。

为党争光! Glory to the CCP!

⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⣧⠞⢻⡅⠀⠀⢀⣷⡆⠈⡙⢶⡄⠐⠂⠀⠒⠂⠀⠈⠉⠉⠉⡍⠙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢫⡞⠁⣰⣿⣿⣄⣼⣿⡛⣷⠀⠈⠈⣷⡄⢀⣩⡐⠶⣄⡀⠂⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡟⠀⣸⣿⠛⣿⣿⡿⠛⡇⢸⣆⠀⠀⢸⣿⠰⠤⠜⠳⠁⢶⡄⠂⠀⠆⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠋⣿⠀⣰⣿⡟⠀⠘⠟⠀⠀⢿⡌⢻⣆⠀⣾⣋⣸⠶⠀⣤⣀⣒⣠⡐⢀⠀⠀⡤⡀⠀⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⡀⣿⡀⣿⣿⠃⠀⠀⠀⠀⠀⠘⣷⡾⠋⣴⡟⣮⣽⡛⢩⡉⢡⡽⢂⠓⠈⠀⣀⠠⢉⡀⠉⠀⠀⠈⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠘⠂⠛⢧⣌⣃⣀⣀⣀⣀⣀⣀⣀⠉⣀⣼⣿⣶⣭⠆⣡⢈⡁⡄⣴⡛⠀⠀⠀⠥⠂⠨⡽⣦⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣭⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣭⣭⣉⠉⠉⠉⠉⠛⠿⣷⣮⣉⠉⣈⠙⠯⠹⠊⠀⠿⠴⠀⠈⠛⠀⠀⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⡿⣫⣾⢿⣿⣿⣿⣿⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣀⠀⠀⠈⠙⠿⣷⣜⣊⡹⡗⣟⠐⠀⠸⠶⠄⠁⠀⠀⠀⣀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⡀⠀⠀⠙⠿⣿⣇⠛⢶⡆⡀⠀⠻⠄⠀⠀⠀⠈⠀⠈⢻⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⠛⠉⠉⠁⠀⠀⠀⠀⠀⠀⢉⣭⣿⣿⣿⣿⣿⣿⣿⣷⣦⣄⠀⠈⠻⢿⣭⣙⣧⣜⣀⡠⢈⠀⠀⠀⠀⠈⢷⠻⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⡍⠛⠿⢷⣶⣄⡀⠙⢿⣿⣮⣁⡰⠀⡌⠐⠀⠀⠀⠁⠀⠈⠟⢿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠋⠻⣿⣷⠀⠀⠀⠀⠀⠀⠠⣼⣿⡿⠋⠉⠀⠀⠈⠙⢿⣿⣇⠀⠀⠀⠉⠛⢿⣶⣄⠙⠷⣿⣿⣞⠓⢖⡀⠄⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠈⣋⢻⡂⠀⠀⠀⠀⠀⠠⠟⠁⠀⣀⣀⣀⡀⠀⠀⠙⠿⠿⠄⠀⠀⠀⠀⠀⠉⠻⣿⣾⣟⣹⣿⣷⣿⣟⣋⠀⠀⠀⠀⠀⠀⠀⠈⢿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⡴⢋⣋⣛⡆⠀⠀⠀⠀⠀⠀⢀⣴⣟⣁⡀⠀⠉⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣻⣿⣜⣉⣉⠀⠀⢀⠛⡄⠀⠀⢸⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⢸⠁⣾⣯⣽⣿⡆⠀⠀⠀⠀⠀⣾⣿⣿⡿⢻⣆⠀⠀⠹⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣽⣿⣿⡖⣢⣄⠀⠁⠀⠀⢸⣿
⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⠿⠛⠋⠀⠘⠘⠻⠻⡻⣿⡁⠀⠀⠀⠀⢰⣿⣿⣍⣿⡆⢹⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣯⣩⣿⣿⣿⣿⣿⣿⣾⡇⡉⠁⠂⠀⠀⣾⣿
⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠀⠀⠀⣼⢻⣿⣿⣿⠇⢸⠀⠀⠀⣧⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⢛⡿⣿⣿⣿⣿⢿⣿⠿⠟⠛⠛⠷⠶⣦⣄⣸⣿⣿
⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿⡜⠿⠿⠟⢠⡞⠀⠀⢰⢣⡀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣴⣠⣸⡿⣿⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿
⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢟⣲⠤⠖⠋⠀⠀⣄⣫⣼⣷⠀⠀⠀⠀⠀⠀⠈⢿⣿⣿⣿⣿⢻⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿
⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣈⣁⣤⣤⣶⣾⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⢹⣿⠙⢳⣺⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿
⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣽
⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣤⣤⣤⣤⣤⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣷⡉⠉⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠻⢿⣿⠿⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣧⡀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣼⢿⣷⠀⠀⠀⠉⠙⠯⣅⣉⣙⣛⣋⣉⣀⣀⣠⠤⠖⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣷⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣽⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣤⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡶⠛⣦⣀⣀⣀⣀⣀⣀⣀⣠⡤⠶⠶⠖⣛⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⡿⢠⣾⠛⠋⠙⠋⢻⣿⡀⠀⠀⠀⢀⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⡿⢇⠟⠁⠀⠀⠀⢠⣴⢿⡇⢀⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿



⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣶⣶⣶⣤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠟⠛⢿⣿⡿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢀⡴⠚⢉⣷⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣷⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⡞⠀⠀⠀⢿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⡟⠟⢡⢉⠣⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢧⠀⠀⠀⢸⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣣⠖⠱⠛⠙⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⢿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⢉⠟⢃⠞⠐⠋⢼⢹⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢰⠀⠀⠀⠸⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣯⡥⠴⣯⡀⠀⡐⢰⡵⣃⣮⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⡼⠀⠀⠀⠀⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⢯⡟⠀⠀⠈⠙⢶⡷⠂⠀⠿⣽⣿⣿⣿⠟⠋⠀⢠⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣠⠃⠀⠀⠀⠀⠈⢿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⠉⣇⠀⠀⠀⠀⠙⠳⠶⠤⠾⠟⠋⠀⠀⠀⣀⣾⣿⣿⣿⣷⣀⣀⣀⣀⠀⠀⠀⠀
⠀⠀⣀⣤⣒⣋⣀⣀⣀⣀⣀⣀⡀⠈⢻⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡤⠶⣿⣿⡀⢹⣦⣄⡀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣴⣿⣿⣿⢏⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣶
⢀⡞⠁⠀⠀⠁⠀⠀⠈⠀⠀⠀⠹⡆⠀⣿⣤⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡀⠀⢿⣿⡇⠘⣿⣿⠟⢓⣒⡛⢷⣶⠾⢛⣛⠛⢿⣿⣿⡏⣸⣿⣿⡏⠀⢸⣿⣿⠟⠋⢉⠐
⠸⣷⣤⣤⣄⠀⠀⣠⠄⠀⡀⣤⣾⣿⣾⣿⣿⣿⣿⣶⣤⣀⡀⠀⠀⠀⠀⢀⣠⣴⡇⠀⠘⣿⣿⡄⢻⡇⠀⢿⣿⡿⢸⣿⣀⣿⣿⡷⠀⣿⡿⣰⣿⣿⣿⠀⢰⣿⣿⡏⠀⠀⢀⠀
⢀⡟⠉⠉⠙⠓⠚⠛⠓⠒⠚⠋⠉⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⣠⣶⣿⢿⣧⣿⠀⠀⠘⢿⣿⣄⡿⠒⠒⠉⠀⠀⠀⠈⠉⠙⠲⢾⣿⣷⣿⣿⣿⠃⢀⣿⣿⣿⠀⠂⠄⠠⠀
⢸⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠊⠈⠻⣿⡷⠞⠋⠁⠀⠀⠀⠀⢾⣷⠀⠀⣴⠿⠆⠀⠀⠀⠀⠙⠿⣧⣴⣾⣿⣿⡿⣐⡂⠓⠟⠀
⠈⡿⠷⠶⢶⣶⣦⣤⣶⣾⣷⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⣼⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠋⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣿⣿⣿⣷⣿⠇⠀⠁⠁
⠀⡇⠀⠀⠀⠀⠀⠈⠉⠉⠉⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡷⢳⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⢏⠀⠠⠄⠀
⠀⢿⣆⣀⡀⣀⠀⠀⠀⣠⣤⣠⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⠀⣿⠲⣤⣀⣀⣀⣀⣀⣀⣀⣀⣀⡀⢀⣀⣀⣀⣀⣀⣀⣀⣀⣴⣦⣾⣿⣿⣿⣿⢇⠀⠁⠀⠀
⠀⢸⠀⠉⠉⠙⠛⠻⠿⠿⠿⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣿⣧⠻⣍⠛⡛⠿⡿⠛⢛⠛⠛⢛⡛⠛⠛⣿⣿⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⢩⡱⢀⠀⠁
⠀⠸⣄⠀⠀⠀⠀⠀⢀⠀⢀⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣎⢧⠙⠲⢧⣀⣇⠀⢸⠀⠀⠐⠀⠀⠀⡇⠀⢸⣀⣸⠾⣛⣽⣿⡿⠋⢻⣿⣿⣤⠌⢰⠄
⠀⠀⠈⠉⠙⠓⠒⠛⢛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠙⠲⢤⣀⣀⠉⠉⠉⠓⠚⠓⠒⠊⠉⢉⣉⣤⣶⣿⣿⣿⣿⠃⠀⠸⣿⣿⣿⡀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣀⠀⠀⠉⠙⠛⠛⠿⢶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⠀⠀⠀⡙⢿⣿⡧⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⣿⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣁⣀⣀⣠⣴⣾⡷⢠⡿⠁⠁⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⠛⠻⠿⣿⠿⠿⠛⠛⠋⠁⠀⠀⠀⠈⠳⣄⠀⠉⠙⠛⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣴⠟⡡⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠓⠦⢤⣼⠋⠙⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⠃⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⡴⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡞⠡⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⡤⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣰⡞⠳⣝⢦⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡟⠡⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⢋⣤⡀⣸⣾⣏⡳⣤⣰⣿⣿⣿⣿⣿⣿⣿⣿⡛⠉⠉⢹⣟⢧⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢴⢞⣿⡟⣱⣛⠞⣱⠟⢁⣼⠟⣦⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⣾⣯⣟⡀⢀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡻⢯⣀⣠⣴⠞⠁⠀⣉⡤⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣸⣿⣿⣧⣼⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⡏⣓⣲⡺⣧⣶⠃⣸⣿⣋⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⢸⣿⡿⢿⠿⡿⠠⡄⠀


*/