all: LCM faneuil
clean:
		rm *.o
LCM: LarryCurlyMoe.o
		gcc LarryCurlyMoe.o -o LCM
faneuil: faneuil.o
		gcc faneuil.o -o faneuil
LarryCurlyMoe.o: LarryCurlyMoe.c
		gcc -c LarryCurlyMoe.c -o LarryCurlyMoe.o
faneuil.o: faneuil.c 
		gcc -c faneuil.c -o faneuil.o