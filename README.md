# Parse ELF

A simple 64-bit ELF file parser.

## Compilation and Execution Instructions

-	Install GCC and Make.

	```shell
	$ sudo apt-get update
	$ sudo apt-get install gcc make
	```

-	Build and run the parser

	```shell
	$ make # Builds project
	$ ./pelf /path/to/elf/file
	```

-	To delete build files, run

	```shell
	$ make clean
	```

## Resources

-	[Becoming an Elf-Lord](https://cpu.land/becoming-an-elf-lord)
-	[In-depth: ELF - The Extensible & Linkable Format](https://www.youtube.com/watch?v=nC1U1LJQL8o)
-	[Wikipedia - ELF, its format and more](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)
-	[64-bit ELF format](https://uclibc.org/docs/elf-64-gen.pdf)
-	[32-bit ELF format](https://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf)

---

## Ideas/To Do

-	Include one structure that stores all parsed file data.
-	Add a usage function.
-	Include Wikipedia link while printing.

## Goals

-	Parse ELF header, segment header and section header (if present and also add
	names of sections.
-	Maybe also showcase which libraries does the ELF depend on.
	-	Esssentially the output of `ldd`.

