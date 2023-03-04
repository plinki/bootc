CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: bootc

bootc: bootc.o pbrfat.o mbr.o read.o bootsector.o tempfile.o
	$(CC) $(CFLAGS) $^ -o $@

bootc.o: bootc.c pbrfat.h read.h bootsector.h tempfile.h
	$(CC) $(CFLAGS) -c $< -o $@

pbrfat.o: pbrfat.c pbrfat.h bootsector.h
	$(CC) $(CFLAGS) -c $< -o $@

mbr.o: mbr.c bootsector.h
	$(CC) $(CFLAGS) -c $< -o $@

read.o: read.c read.h
	$(CC) $(CFLAGS) -c $< -o $@

bootsector.o: bootsector.c bootsector.h
	$(CC) $(CFLAGS) -c $< -o $@

tempfile.o: tempfile.c tempfile.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o bootc
