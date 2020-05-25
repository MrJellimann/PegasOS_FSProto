diskMaker : diskMaker.o

diskMaker.o : diskMaker.c
	cc -c diskMaker.c

clean :
	rm diskMaker.o