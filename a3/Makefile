# This makefile is to build pmake. It is not a valid makefile for
# testing pmake since it contains pattern rules and automatic variables
pmake : pmake.o parse_fuyuhan.o run_make.o
	gcc -Wall -g -o pmake pmake.o parse_fuyuhan.o run_make.o

%.o : %.c
	gcc -Wall -g -c $< 

clean :
	rm pmake *.o
