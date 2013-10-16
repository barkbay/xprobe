all: myprobe

myprobe: net.o disk.o lparstat.o opentsdbc.o uname.o mem.o
	cc -lperfstat -o myprobe -g disk.o net.o lparstat.o opentsdbc.o uname.o mem.o

lparstat.o: opentsdbc.o lparstat.c uname.o 
	cc -o lparstat.o -g -c lparstat.c

opentsdbc.o: opentsdbc.c
	cc -o opentsdbc.o -g -c opentsdbc.c

uname.o: uname.c
	cc -o uname.o -g -c uname.c

mem.o: mem.c
	cc -o mem.o -g -c mem.c

net.o: net.c
	cc -o net.o -g -c net.c

disk.o: disk.c
	cc -o disk.o -g -c disk.c

clean:
	rm -rf *.o myprobe
