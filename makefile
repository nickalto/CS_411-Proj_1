#  Nick Alto - CS411 - Assignment 1
#  Makefile for pMult

 default : begin clean

 begin:
	gcc -Wall -lpthread -lm -o pMult pMult.c  

 clean: 
	rm -f *.o
