#!/bin/bash

read -p "Introduce el nombre del archivo scanner (con .l): " nombreScanner
read -p "Introduce el nombre del archivo de entrada (sin .txt): " nombreEntrada

nombreSinExtension="${nombreScanner%.l}"
nombreEntradaConExtension="${nombreEntrada}.txt"

flex $nombreScanner && gcc lex.yy.c -o $nombreSinExtension || { echo -e "\nERROR AL COMPILAR\n"; exit 1; }

./$nombreSinExtension < $nombreEntradaConExtension || { echo -e "\nERROR AL ABRIR ARCHIVO DE ENTRADA\n"; exit 1; }