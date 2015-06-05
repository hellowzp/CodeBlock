all:gateway simulator
gateway :gateway.c list.h simulator.h list.c simulator.c
	gcc -Wall gateway.c list.c -o gateway
simulator:simulator.c simulator.h
	gcc -Wall simulator.c -o simulator
