# Parse ELF

A simple 64-bit [**E**xecutable and **L**inkable **F**ormat](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)
file parser.

This utility parses and prints the ELF (File) Header, the Section Headers, the
Segment (Program) Headers and the dynamic dependencies in the ELF file.

This utility is essentially a rudimentary clone of the `readelf` and `ldd` Linux
utilities.

This utility is a part of [my talk on ELFs](https://talks.harshkapadia.me/elf).

## Compilation and Execution Instructions

-	Install GCC and Make

	```shell
	$ sudo apt-get update
	$ sudo apt-get install gcc make
	```

-	Build the parser

	```shell
	$ make
	```

-	Run the parser

	```shell
	$ ./pelf "/path/to/elf/file"
	```

-	To delete build files, run

	```shell
	$ make clean
	```

-	Optional: Formatting files

	```shell
	$ sudo apt-get install clang-format
	$ make format
	```

## Sample Output

```shell
$ make
gcc -Wall -pedantic -I include src/pelf.c -o pelf
chmod +x pelf

$ ./pelf "/usr/bin/vim"
64-bit ELF File Parser


ELF details and value translations:
https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

ELF file path: /usr/bin/vim


ELF File 'File Header':

-> Magic number: 0x7f 0x45 0x4c 0x46 (0x7f E L F)
-> Class: 2
-> Data (Endianness): 1
-> Version: 1
-> OS/ABI: 00
-> ABI version: 00
-> Type: 0x03
-> Machine: 0x3e
-> Version: 1
-> Entry address: 0x4ead0
-> Program (segment) header table offset: 64 B into the file
-> Section header table offset: 3785904 B into the file
-> Flags: 0
-> This header\'s size: 64 B
-> Program (segment) header size: 56 B
-> No. of program (segment) headers: 13
-> Section header size: 64 B
-> No. of section headers: 30
-> Index of the 'section name string table' section header in the section header
table: 29


ELF File Section Headers:

[No.]   Name
        Type            Address         Offset
        Size            EntSize         Flags  Link     Info  Align
---------------------------------------------------------------------
[0]
        NULL            0               0
        0               0                    0          0     0
---------------------------------------------------------------------
[1]     .interp
        PROGBITS                0x318           792
        28              0               A     0         0     1
---------------------------------------------------------------------
[2]     .note.gnu.property
        NOTE            0x338           824
        48              0               A     0         0     8
---------------------------------------------------------------------
[3]     .note.gnu.build-id
        NOTE            0x368           872
        36              0               A     0         0     4
---------------------------------------------------------------------
[4]     .note.ABI-tag
        NOTE            0x38c           908
        32              0               A     0         0     4
---------------------------------------------------------------------
[5]     .gnu.hash
        0x6ffffff6              0x3b0           944
        240             0               A     6         0     8
---------------------------------------------------------------------
[6]     .dynsym
        DYNSYM          0x4a0           1184
        8568            24              A     7         1     8
---------------------------------------------------------------------
[7]     .dynstr
        STRTAB          0x2618          9752
        4267            0               A     0         0     1
---------------------------------------------------------------------
[8]     .gnu.version
        0x6fffffff              0x36c4          14020
        714             2               A     6         0     2
---------------------------------------------------------------------
[9]     .gnu.version_r
        0x6ffffffe              0x3990          14736
        320             0               A     7         5     8
---------------------------------------------------------------------
[10]    .rela.dyn
        RELA            0x3ad0          15056
        202464          24              A     6         0     8
---------------------------------------------------------------------
[11]    .rela.plt
        RELA            0x351b0         217520
        7704            24              IA     6        25     8
---------------------------------------------------------------------
[12]    .init
        PROGBITS                0x37000         225280
        27              0               XA     0        0     4
---------------------------------------------------------------------
[13]    .plt
        PROGBITS                0x37020         225312
        5152            16              XA     0        0     16
---------------------------------------------------------------------
[14]    .plt.got
        PROGBITS                0x38440         230464
        48              16              XA     0        0     16
---------------------------------------------------------------------
[15]    .plt.sec
        PROGBITS                0x38470         230512
        5136            16              XA     0        0     16
---------------------------------------------------------------------
[16]    .text
        PROGBITS                0x39880         235648
        2845383         0               XA     0        0     16
---------------------------------------------------------------------
[17]    .fini
        PROGBITS                0x2f0348                3081032
        13              0               XA     0        0     4
---------------------------------------------------------------------
[18]    .rodata
        PROGBITS                0x2f1000                3084288
        166822          0               A     0         0     32
---------------------------------------------------------------------
[19]    .eh_frame_hdr
        PROGBITS                0x319ba8                3251112
        37684           0               A     0         0     4
---------------------------------------------------------------------
[20]    .eh_frame
        PROGBITS                0x322ee0                3288800
        265108          0               A     0         0     8
---------------------------------------------------------------------
[21]    .init_array
        INIT_ARRAY              0x365230                3555888
        8               8               AW     0        0     8
---------------------------------------------------------------------
[22]    .fini_array
        FINI_ARRAY              0x365238                3555896
        8               8               AW     0        0     8
---------------------------------------------------------------------
[23]    .data.rel.ro
        PROGBITS                0x365240                3555904
        65792           0               AW     0        0     32
---------------------------------------------------------------------
[24]    .dynamic
        DYNAMIC         0x375340                3621696
        608             16              AW     7        0     8
---------------------------------------------------------------------
[25]    .got
        PROGBITS                0x3755a0                3622304
        2648            8               AW     0        0     8
---------------------------------------------------------------------
[26]    .data
        PROGBITS                0x376000                3624960
        160601          0               AW     0        0     32
---------------------------------------------------------------------
[27]    .bss
        NOBITS          0x39d360                3785561
        58176           0               AW     0        0     32
---------------------------------------------------------------------
[28]    .gnu_debuglink
        PROGBITS                0               3785564
        52              0                    0          0     4
---------------------------------------------------------------------
[29]    .shstrtab
        STRTAB          0               3785616
        285             0                    0          0     1
---------------------------------------------------------------------

Section Header flag legend:
W (write), A (alloc), X (execute), M (merge), S (strings),
I (info), L (link order), O (extra OS processing required),
G (group), T (TLS), o (OS specific), P (processor specific),
R (ordered), E (exclude)


ELF File Segment (Program) Headers:

Type            Offset          VirtAddr        PhysAddr
                FileSiz         MemSiz          Flags  Align
---------------------------------------------------------------------
PHDR            0x40            0x40            0x40
                0x2d8           0x2d8           R     0x8
---------------------------------------------------------------------
INTERP          0x318           0x318           0x318
                0x1c            0x1c            R     0x1
---------------------------------------------------------------------
LOAD            0               0               0
                0x36fc8         0x36fc8         R     0x1000
---------------------------------------------------------------------
LOAD            0x37000         0x37000         0x37000
                0x2b9355                0x2b9355                RX     0x1000
---------------------------------------------------------------------
LOAD            0x2f1000                0x2f1000                0x2f1000
                0x72a74         0x72a74         R     0x1000
---------------------------------------------------------------------
LOAD            0x364230                0x365230                0x365230
                0x38129         0x46470         RW     0x1000
---------------------------------------------------------------------
DYNAMIC         0x374340                0x375340                0x375340
                0x260           0x260           RW     0x8
---------------------------------------------------------------------
NOTE            0x338           0x338           0x338
                0x30            0x30            R     0x8
---------------------------------------------------------------------
NOTE            0x368           0x368           0x368
                0x44            0x44            R     0x4
---------------------------------------------------------------------
0x6474e553              0x338           0x338           0x338
                0x30            0x30            R     0x8
---------------------------------------------------------------------
0x6474e550              0x319ba8                0x319ba8                0x319ba8
                0x9334          0x9334          R     0x4
---------------------------------------------------------------------
0x6474e551              0               0               0
                0               0               RW     0x10
---------------------------------------------------------------------
0x6474e552              0x364230                0x365230                0x365230
                0x10dd0         0x10dd0         R     0x1
---------------------------------------------------------------------

Program (Segment) Header flag legend:
X (execute), W (write), R (read)


Dynamic dependencies listed in the ELF file:
-> libm.so.6
-> libtinfo.so.6
-> libselinux.so.1
-> libsodium.so.23
-> libacl.so.1
-> libgpm.so.2
-> libpython3.10.so.1.0
-> libc.so.6

NOTE: Each dependency might have its own dependencies.


```

## Resources

[talks.harshkapadia.me/elf](https://talks.harshkapadia.me/elf)

