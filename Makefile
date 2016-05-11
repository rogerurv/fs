all : memoria.o winsuport2.o missatge.o semafor.o tennis4 pal_ord4

tennis4 : tennis4.c memoria.o winsuport2.o semafor.o missatge.o
	gcc -Wall tennis4.c memoria.o winsuport2.o missatge.o semafor.o -o tennis4 -lcurses -lpthread

pal_ord4 : pal_ord4.c memoria.o winsuport2.o semafor.o missatge.o
	gcc -Wall pal_ord4.c memoria.o winsuport2.o missatge.o semafor.o -o pal_ord4 -lcurses -lpthread

winsuport2.o : winsuport2.c winsuport2.h
	gcc -c -Wall winsuport2.c -o winsuport2.o 

memoria.o : memoria.c memoria.h
	gcc -c -Wall memoria.c -o memoria.o 

semafor.o : semafor.c semafor.h
	gcc -c -Wall semafor.c -o semafor.o 

missatge.o : missatge.c missatge.h
	gcc -c -Wall missatge.c -o missatge.o 

clean:
	rm tennis4 pal_ord4 winsuport2.o memoria.o missatge.o semafor.o
