all: single multi
clean:
		rm *.o
single: single.o
		gcc single.o -o single
multi: multi.o
		gcc multi.o -o multi
single.o: single.c header.h
		gcc -c single.c -o single.o
multi.o: multi.c header.h
		gcc -c multi.c -o multi.o