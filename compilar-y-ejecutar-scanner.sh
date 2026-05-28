#!/bin/bash

read -p "Introduce el nombre del archivo scanner: " nombreScanner

set -e && echo "\nERROR AL COMPILAR\n"

flex $nombreScanner
gcc lex.yy.c -o scanner
./scanner < entrada.txt  