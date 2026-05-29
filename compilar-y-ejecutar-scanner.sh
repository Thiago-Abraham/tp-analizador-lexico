#!/bin/bash

read -p "Introduce el nombre del archivo scanner (con .l): " nombreScanner

set -e && echo "\nERROR AL COMPILAR\n"

nombreSinExtension="${nombreScanner%.l}"

flex $nombreScanner
gcc lex.yy.c -o $nombreSinExtension
./$nombreSinExtension < entrada.txt  