all : memoria.o winsuport2.o multiproc2 mp_car2

multiproc2 : multiproc2.c memoria.o winsuport2.o
	gcc -Wall multiproc2.c memoria.o winsuport2.o -o multiproc2 -lcurses

mp_car2 : mp_car2.c memoria.o winsuport2.o
	gcc -Wall mp_car2.c memoria.o winsuport2.o -o mp_car2 -lcurses

winsuport2.o : winsuport2.c winsuport2.h
	gcc -c -Wall winsuport2.c -o winsuport2.o 

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o 

clean:
	rm multiproc2 mp_car2 winsuport2.o memoria.o
