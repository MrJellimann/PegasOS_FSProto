diskMaker : diskMaker.o

diskMaker.o : diskMaker.c
	cc -c diskMaker.c

fs : fs.o

fs.o : fs.c
	cc -c fs.c

clean :
	rm diskMaker.o
	rm fs.o