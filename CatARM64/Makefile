cat: cat.o
	ld cat.o -o cat
 
cat.o: cat.S
	as -g  --64 cat.S -o cat.o

clean:
	rm -f cat.o cat *.stackdump *'~'
