DEBUG?=0
ifeq ($(DEBUG),1)
        CFLAGS =-DDEBUG
else
        CFLAGS =-DNDEBUG
endif
islip: my402list.o islip.o
	gcc $(CFLAGS) -o islip -g -Wall my402list.o islip.o 
islip.o: islip.c islip.h iSLIP_scheduler.c islipLib.c RRM_scheduler.c
	gcc $(CFLAGS) -g -c -Wall islip.c
my402list.o: my402list.c my402list.h
	gcc $(CFLAGS) -g -c -Wall my402list.c
clean:
	rm -f *.o maxWeight wtNorm.dat wt.dat Simulation.log islip 
