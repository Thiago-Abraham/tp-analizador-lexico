#!/bin/bash

flex scanner.l && gcc lex.yy.c tokens.c -o scanner || { echo -e "\nERROR AL COMPILAR\n"; exit 1; }