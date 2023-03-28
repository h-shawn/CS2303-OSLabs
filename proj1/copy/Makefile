all: Copy
clean:
		rm *.o
Copy: Copy.o
		gcc Copy.o -o Copy
Copy.o: Copy.c header.h
		gcc -c Copy.c -o Copy.o