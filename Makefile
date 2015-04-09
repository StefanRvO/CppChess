#GENERIC RULES FOR BUILDING A PROJECT FOR TI TM4C123G
#May Be buggy
#Losely based on TI StellarisWare examples

include Makefile.common

#Define Compiler, linker, archiver, etc.
COMPILER=g++
CC=${PREFIX}${COMPILER}
