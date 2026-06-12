# UTN - FRBA
## Sintaxis y Semántica de los Lenguajes
### Trabajo Práctico n° 2 - Flex para reconocimiento de categorías léxicas de C

## Enunciado

Implementar en lenguaje C un programa que permita reconocer desde un archivo `.i` (archivo `.c` preprocesado) de entrada todas las categorías léxicas de ANSI C (C89/C90).
El mismo deberá producir como salida por defecto por pantalla (stdout) un reporte.
Para desarrollar dicho programa se deberá utilizar Flex para la generación del código C que implemente un analizador léxico (scanner) que cumpla con lo requerido.

## Categorías léxicas de ANSI C a reconocer

- **Constantes**
  - Enteras (decimales, octales, hexadecimales): con y sin sufijo.
  - Reales: con y sin sufijo.
  - Caracter: que a su vez pueden ser de tipo:
    - Simple (ej. `'a'`, `'0'`)
    - Con secuencia de escape
      - Simple (ej. `'\n'`, `'\t'`)
      - Octal (ej. `'\0'`, `'\07'`, `'\007'`)
      - Hexadecimal (ej. `'\xF'`, `'\x0F'`)
- **Literales cadena**
- **Palabras reservadas** (agrupar por categoría sintáctica: ver abajo)
- **Identificadores**
- **Caracteres de puntuación/operadores**

## Reporte

El reporte deberá consistir en un conjunto de listados que deberá seguir el siguiente orden:

1. **Identificadores** encontrados indicando la cantidad de veces que aparece cada uno de ellos. El listado debe estar ordenado alfabéticamente.

2. **Literales cadena** encontrados indicando la longitud de los mismos y ordenados por longitud ascendente. En caso de igual longitud se desempatan por orden de aparición.

3. **Palabras reservadas** en el orden en el que han aparecido en el archivo, indicando el número de línea y columna de cada una de ellas. Se debe indicar todas las apariciones de cada palabra reservada y separar el listado por: especificadores de clase de almacenamiento, especificadores de tipo, calificadores de tipo, struct o union, enumeración, etiqueta, selección, iteración, salto y unario.

4. **Constantes** indicando según su tipo:
   - Para las constantes decimales: indicar el valor de cada una y el total acumulado de sumar todas ellas.
   - Para las constantes hexadecimales: indicar su valor entero decimal.
   - Para las constantes octales: indicar su valor entero decimal.
   - Para las constantes reales: indicar el valor de su mantisa y parte entera.
   - Para las constantes caracter: enumerarlas según orden de aparición.
   - Las constantes se deben listar en el orden de aparición y cada vez que se repitan por más que sea la misma.

5. **Operadores/caracteres de puntuación** indicando cantidad de veces que aparecen. Listarlos ordenados según su orden de primera aparición.

6. **Cadenas no reconocidas** indicando el número de línea y el de columna donde se encontraron.

Cuando no hay elemento de un listado se deberá imprimir en el reporte con un `-`. Ver ejemplos de test incluídos en la carpeta correspondiente al trabajo práctico.

## Uso

```
./bin/tp2.exe <ruta archivo entrada> [parámetros adicionales opcionales]
```

La ruta al archivo de texto de entrada es el único parámetro obligatorio.
Adicionalmente, pueden agregar parámetros opcionales que consideren útiles o convenientes (ej. activar logs verbosos para facilitar el debug, etc.).
