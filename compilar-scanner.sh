#!/bin/bash

read -p "Introduce el nombre del archivo scanner (con .l): " nombreScanner

flex $nombreScanner && gcc lex.yy.c -o $nombreSinExtension || { echo -e "\nERROR AL COMPILAR\n"; exit 1; }