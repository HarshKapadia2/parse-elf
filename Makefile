all: pelf

pelf: include src/pelf.c
	gcc -Wall -pedantic -I include src/pelf.c -o pelf
	chmod +x pelf

clean:
	rm -f pelf

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

.PHONY: all clean format
