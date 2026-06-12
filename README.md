# TP2 — Scanner Léxico de C

Trabajo Práctico 2 de Sintaxis y Semántica de los Lenguajes (UTN).  
Implementa un **analizador léxico** para un subconjunto del lenguaje C, usando Flex para el reconocimiento de tokens y listas enlazadas propias para su almacenamiento y presentación.

---

## Estructura del proyecto

```
src/
├── scanner.l           # Definición del scanner (Flex)
├── tokens.h            # Tipos de datos y declaraciones públicas
├── tokens.c            # Implementación de listas y funciones auxiliares
├── compilar-scanner.sh # Script de compilación
└── entrada.txt         # Archivo de prueba
```

---

## Compilación y uso

### Compilar

```bash
cd src
./compilar-scanner.sh
```

El script ejecuta internamente:

```bash
flex scanner.l && gcc lex.yy.c tokens.c -o scanner
```

1. `flex scanner.l` genera `lex.yy.c` a partir de las reglas léxicas.
2. `gcc` compila `lex.yy.c` junto con `tokens.c` y produce el ejecutable `scanner`.

### Ejecutar

```bash
./scanner <ruta_al_archivo>          # Modo normal: imprime el reporte
./scanner <ruta_al_archivo> DEBUG    # Modo debug: imprime cada token al reconocerlo
```

**Modo normal:** procesa el archivo, agrupa los tokens por categoría y genera un reporte formateado.  
**Modo DEBUG:** imprime en `stderr` cada token reconocido con su lexema, línea y columna en tiempo real, sin generar el reporte final. Al terminar muestra el total de tokens válidos y no reconocidos.

### Archivo de entrada de ejemplo (`entrada.txt`)

```c
int main(void) {
    char *cadena = "Hola, Mundo\n";
    char *cadena_2 = "mundo";
    if cadena = "texto"
    @double@ sizeof
    return (20 * 10) + 07 + 0xFF + 017 + 07 + 1.5 * ('a' + '\0');
}
```

---

## Arquitectura general

```
archivo fuente C
       │
       ▼
  [ scanner.l / Flex ]
       │ reconoce tokens con expresiones regulares
       │ llama a imprimirReporte() por cada token
       ▼
  [ agregarNodoALista() ]
       │ inserta el token en la lista correspondiente
       │ (deduplicando o permitiendo repetición según el tipo)
       ▼
  [ listas enlazadas por categoría ]
       │
       ▼
  [ darFormatoALista() ]   →   ordena si corresponde
       │
       ▼
  [ imprimirLista() ]      →   imprime el reporte final
       │
       ▼
  [ liberarLista() ]       →   libera la memoria
```

---

## `tokens.h` — Tipos de datos

### `TipoToken`

Enumera todos los tipos de token que el scanner puede reconocer:

| Valor | Descripción |
|---|---|
| `tokenDecimal` | Constante entera decimal (`42`, `100`) |
| `tokenOctal` | Constante entera octal (`07`, `017`) |
| `tokenHexadecimal` | Constante entera hexadecimal (`0xFF`, `0x1A`) |
| `tokenReal` | Constante de punto flotante (`1.5`, `.3e2`) |
| `tokenConstanteCaracter` | Constante de carácter (`'a'`, `'\n'`) |
| `tokenLiteralesCadena` | Literal de cadena (`"hola"`) |
| `tokenOperators` | Operador (`+`, `*`, `==`, `<<`, etc.) |
| `tokenPunctuators` | Puntuador (`{`, `}`, `;`, `,`, etc.) |
| `tokenReservada*` | Palabras reservadas de C (ver detalle abajo) |
| `tokenIdentifier` | Identificador de usuario (`main`, `cadena`) |
| `tokenNoReconocido` | Secuencia no reconocida por ninguna regla (`@`) |

Las palabras reservadas se subdividen en:

| Subcategoría | Palabras |
|---|---|
| Especificadores de almacenamiento | `auto`, `register`, `static`, `extern`, `typedef` |
| Especificadores de tipo | `void`, `char`, `short`, `int`, `long`, `float`, `double`, `signed`, `unsigned` |
| Calificadores de tipo | `const`, `volatile` |
| Struct / Union | `struct`, `union` |
| Enumeración | `enum` |
| Etiquetas | `case`, `default` |
| Selección | `if`, `else`, `switch` |
| Iteración | `do`, `while`, `for` |
| Salto | `goto`, `continue`, `break`, `return` |
| Operador unario | `sizeof` |

### `TipoDeLista`

Enumera las categorías de listas en las que se almacenan los tokens:

| Valor | Contenido |
|---|---|
| `tipoListaIdentificadores` | Identificadores de usuario |
| `tipoListaLiteralesCadena` | Literales de cadena |
| `tipoListaPalabrasReservadas` | Palabras reservadas (todas las subcategorías) |
| `tipoListaConstantesDecimales` | Enteros decimales |
| `tipoListaConstantesHexadecimales` | Enteros hexadecimales |
| `tipoListaConstantesOctales` | Enteros octales |
| `tipoListaConstantesReales` | Constantes de punto flotante |
| `tipoListaConstantesCaracter` | Constantes de carácter |
| `tipoListaOperadoresYPuntuadores` | Operadores y puntuadores |
| `tipoListaNoReconocidos` | Tokens no reconocidos |

### `NodoToken`

Nodo de la lista enlazada. Cada nodo representa un token:

```c
typedef struct NodoToken {
    TipoToken tipo;           // categoría del token
    char*     lexema;         // texto del token (heap, copiado con strdup)
    int       contador;       // cuántas veces apareció (en listas con dedup)
    size_t    longitud;       // longitud del lexema
    int       linea;          // línea donde apareció por primera vez
    int       columna;        // columna donde apareció por primera vez
    struct NodoToken* siguiente;
} NodoToken;
```

---

## `tokens.c` — Implementación

### Funciones públicas

#### `crearNodo(lexema, tipo, linea, columna) → NodoToken*`

Crea un nuevo nodo con `malloc`. Copia el lexema con `strdup` (que hace su propio `malloc` interno). Inicializa `contador = 1`. Devuelve `NULL` si falla la asignación de memoria, imprimiendo un mensaje a `stderr`.

---

#### `agregarNodoALista(lista, lexema, tipo, tipoLista, linea, columna)`

Inserta un token en la lista `*lista`. El comportamiento depende del tipo de lista, determinado por `estaEnListaQueRequiereRepeticion()`:

**Listas con deduplicación** (`tipoListaIdentificadores`, `tipoListaLiteralesCadena`, `tipoListaOperadoresYPuntuadores`):  
Si el lexema ya existe con el mismo tipo, incrementa su `contador` en lugar de crear un nodo nuevo. Esto permite saber cuántas veces apareció cada token único.

**Listas sin deduplicación** (`tipoListaConstantesDecimales`, `tipoListaConstantesHexadecimales`, `tipoListaConstantesOctales`, `tipoListaConstantesReales`, `tipoListaConstantesCaracter`, `tipoListaPalabrasReservadas`, `tipoListaNoReconocidos`):  
Cada aparición genera un nodo nuevo, preservando el orden de llegada y la información de línea/columna de cada ocurrencia.

---

#### `liberarLista(lista)`

Recibe `NodoToken**`. Recorre la lista liberando primero el `lexema` (reservado por `strdup`) y luego el nodo. Al terminar asigna `*lista = NULL` para evitar punteros colgantes (*dangling pointers*).

---

#### `imprimirLista(lista, tipo)`

Imprime el contenido de la lista según su tipo:

| Tipo de lista | Formato de salida |
|---|---|
| Identificadores | `nombre: aparece N vez/veces` |
| Literales cadena | `"texto": longitud N` |
| Palabras reservadas | `palabra: linea N, columna N` |
| Constantes hexadecimales | `0xFF: valor entero decimal 255` |
| Constantes decimales | `42: valor 42` + total acumulado al final |
| Constantes octales | `07: valor entero decimal 7` |
| Constantes reales | `1.5: parte entera 1, mantisa 0.5` |
| Constantes carácter | `1) 'a'`, `2) '\n'` (numeradas por orden de aparición) |
| Operadores/Puntuadores | `+: aparece N vez/veces` |
| No reconocidos | `@: linea N, columna N` |

Para constantes decimales, además imprime al final la suma de todos los valores de la lista.

---

#### `darFormatoALista(lista, tipo)`

Aplica el ordenamiento correspondiente antes de imprimir:

| Tipo de lista | Ordenamiento |
|---|---|
| Identificadores | Alfabético sin distinción de mayúsculas (`strcasecmp`) |
| Literales cadena | Por longitud ascendente |
| Resto | Se mantiene el orden de llegada (sin reordenar) |

El ordenamiento se delega a `ordenarListaPor()`, pasándole la función de comparación adecuada.

---

#### `ordenarListaPor(lista, comparar)`

Bubble sort sobre lista enlazada. Recibe un puntero a función de comparación:

```c
int (*comparar)(NodoToken*, NodoToken*)
```

Esto permite reutilizar el mismo algoritmo para cualquier criterio de orden (estilo funcional). Intercambia nodos reenlazando punteros, sin copiar datos.

---

### Funciones internas (`static`)

Son funciones de uso interno del módulo, no exportadas en `tokens.h`.

#### `estaEnListaQueRequiereRepeticion(tipoLista) → int`

Devuelve `1` si el tipo de lista debe almacenar cada ocurrencia como nodo separado (sin deduplicar), `0` si debe deduplicar y contar. Centraliza esta lógica en un único lugar: si se agrega una nueva categoría, solo hay que modificar esta función.

#### `sumarListaDecimales(lista) → int`

Recorre la lista de constantes decimales y suma todos sus valores enteros. Llamada por `imprimirLista` para mostrar el total acumulado al final del listado decimal.

#### `retornarParteEntera(nodo) → float`

Parsea el lexema de un nodo de constante real con `atof`, trunca a entero y lo devuelve como `float`. Usada por `imprimirLista` en el caso de constantes reales.

#### `retornarMantisa(nodo) → float`

Devuelve la parte decimal de una constante real: `atof(lexema) - retornarParteEntera(nodo)`. Usada por `imprimirLista` junto con `retornarParteEntera`.

---

## `scanner.l` — Reglas léxicas (Flex)

### Variables globales

| Variable | Tipo | Descripción |
|---|---|---|
| `linea_actual` | `int` | Línea actual en el archivo fuente |
| `columna_actual` | `int` | Columna actual en el archivo fuente |
| `DEBUG_MODE` | `bool` | Activa el modo de impresión en tiempo real |
| `tokens_reconocidos` | `int` | Contador de tokens válidos (modo DEBUG) |
| `tokens_no_reconocidos` | `int` | Contador de tokens inválidos (modo DEBUG) |
| `lista*` | `NodoToken*` | Una variable por cada categoría de lista (19 en total) |

### Expresiones regulares definidas

| Nombre | Descripción | Ejemplos |
|---|---|---|
| `decimal` | `[1-9][0-9]*` | `42`, `100` |
| `octal` | `0[0-7]*` | `0`, `07`, `017` |
| `hexadecimal` | `0[xX][0-9a-fA-F]+` | `0xFF`, `0x1A` |
| `realPuntoInicial` | Empieza con punto | `.5`, `.3e2` |
| `realConPunto` | Tiene parte entera y punto | `1.5`, `3.14` |
| `realSoloExponencial` | Solo exponente, sin punto | `2e10`, `1E-3` |
| `real` | Unión de los tres anteriores | — |
| `constanteCaracter` | `'x'` o secuencias de escape | `'a'`, `'\n'`, `'\t'` |
| `literalesCadena` | Texto entre comillas dobles | `"hola"`, `"mundo\n"` |
| `operators` | Operadores del lenguaje C | `+`, `==`, `<<`, `->`, `&&` |
| `punctuators` | Puntuadores del lenguaje C | `{`, `}`, `;`, `,`, `...` |
| `reservada*` | Palabras clave C (10 subcategorías) | `int`, `if`, `return` |
| `identifier` | `[_a-zA-Z][_a-zA-Z0-9]*` | `main`, `cadena_2` |
| `noReconocido` | Cualquier otro carácter | `@` |

> **Nota sobre `octal`:** la regla `0[0-7]*` también reconoce el cero solo (`0`), ya que los corchetes con `*` permiten cero dígitos adicionales. Por eso el literal `0` se clasifica como octal y no como decimal.

### Función `imprimirReporte()`

Llamada desde cada regla del scanner. Recibe el mensaje descriptivo, el lexema (`yytext`), el tipo de token, el puntero a la lista, el tipo de lista, la posición y un indicador de error (`'A'` para acierto, `'E'` para error).

- **Modo DEBUG + error:** imprime en rojo a `stderr`.
- **Modo DEBUG + acierto:** imprime en verde a `stderr`.
- **Modo normal:** llama a `agregarNodoALista()` para acumular el token.

### Función `imprimirListaFormateada()`

Recorre las 19 listas en orden y por cada una:

1. Imprime el encabezado de sección.
2. Si la lista está vacía, imprime `-`.
3. Si tiene nodos: aplica `darFormatoALista()`, imprime con `imprimirLista()` y libera con `liberarLista()`.

### `main()`

1. Valida los argumentos: requiere exactamente 1 o 2 parámetros (ruta del archivo y opcionalmente `DEBUG`).
2. Abre el archivo de entrada con `fopen`.
3. Asigna `yyin` al archivo y llama a `yylex()` (el scanner generado por Flex).
4. Cierra el archivo.
5. Según el modo, imprime el resumen DEBUG o llama a `imprimirListaFormateada()`.

---

## Decisiones de diseño

**Doble puntero (`NodoToken**`) en `agregarNodoALista` y `liberarLista`**  
Permite modificar el puntero original del caller, equivalente al pasaje por referencia de C++. `liberarLista` además asigna `*lista = NULL` al terminar para evitar *use-after-free*.

**`estaEnListaQueRequiereRepeticion`**  
Centraliza en un único lugar la decisión de si una lista deduplica o no. Si en el futuro se agrega una nueva categoría, solo hay que modificar esta función y no `agregarNodoALista`.

**`ordenarListaPor` con puntero a función**  
Permite reutilizar el mismo bubble sort para distintos criterios de orden. Refleja un estilo de programación funcional dentro de C, similar a `qsort` de la librería estándar.

**Forward declarations `static`**  
Las funciones internas se declaran al inicio de `tokens.c` con `static` para que puedan ser llamadas por funciones definidas antes que ellas en el archivo, sin necesidad de reordenar el código ni exponerlas en el header.

**`strdup` en `crearNodo`**  
Cada nodo tiene su propia copia del lexema en el heap. Esto es necesario porque Flex reutiliza el buffer `yytext` en cada regla: sin `strdup`, todos los nodos apuntarían al mismo texto y quedarían sobreescritos.
