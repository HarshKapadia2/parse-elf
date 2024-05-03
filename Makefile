all: pelf

pelf: pelf.o
	gcc pelf.o -o pelf

pelf.o: pelf.c
	gcc -c -I. -MP -MD pelf.c

clean:
	rm -f pelf.d pelf.o pelf

# Make should compile pelf.c even if pelf.h is changed, which explains the -MD
# and -MP GCC flags. They generate a pelf.d file for Make to keep a track of the
# dependencies, i.e., the includes.
-include pelf.d

.PHONY: all clean

