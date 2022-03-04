all: sanitycheck shellgame.exe

shellgame.exe: shellgame.o sem.o spinlock.o tas64.o
	gcc --std=gnu11 -g -o shellgame.exe shellgame.o sem.o spinlock.o tas64.o

shellgame.o: shellgame.c
	gcc --std=gnu11 -g -c shellgame.c

sem.o: sem.c sem.h
	gcc --std=gnu11 -g -c sem.c

sanitycheck: sanitycheck.c spinlock.h spinlock.c tas64.S
	gcc --std=gnu11 -g -o sanitycheck spinlock.c sanitycheck.c tas64.S

spinlock.o: spinlock.c spinlock.h
	gcc --std=gnu11 -g -c spinlock.c

tas64.o: tas64.S
	gcc --std=gnu11 -g -c tas64.S

clean:
	rm -f sanitycheck shellgame.exe *.o *.stackdump
