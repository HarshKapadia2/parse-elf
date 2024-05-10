#include "pelf.h"
#include <errno.h>  // For strerr()
#include <stddef.h> // For 'NULL'
#include <stdio.h>  // For file functions, printf()
#include <string.h> // For memcmp()

int main(int argc, char *argv[]) {
    char *file_path = NULL;

    // Get file path from command line args
    if (argc < 2) {
        printf("ERROR: Insufficient arguments.\n");
        // usage();
        return 1;
    } else {
        file_path = argv[1];
    }

    // try to open file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        fclose(file);
        printf("ERROR: Could not open file '%s': %s", file_path,
               strerror(errno));
        return 2;
    }

    unsigned char magic_bytes[MAGIC_BYTE_COUNT];
    get_magic_bytes(file, magic_bytes);
    if (!is_magic_bytes_elf(magic_bytes)) {
        fclose(file);
        printf("ERROR: File at '%s' does not have ELF header, got: %02x %02x "
               "%02x %02x\n",
               file_path, magic_bytes[0], magic_bytes[1], magic_bytes[2],
               magic_bytes[3]);
        return 2;
    }

    // Check if is ELF and is a 64-bit ELF
    if (get_elf_class(file) != 2) {
        fclose(file);
        printf("ERROR: This utility can only parse 64-bit ELF files.\n");
        return 1;
    }

    // Parse ELF file header
    elf64_hdr header = parse_elf64_hdr(file);
    print_elf64_hdr(&header);

    // Cleanup
    fclose(file);

    return 0;
}

// Get the first MAGIC_BYTE_COUNT bytes of the file
// If the file is an ELF, this will be the magic number
// Seeks to beginning of file to read data
void get_magic_bytes(FILE *file, unsigned char *magic_bytes) {
    fseek(file, 0L, SEEK_SET);
    fread(magic_bytes, sizeof(unsigned char), MAGIC_BYTE_COUNT, file);
}

bool is_magic_bytes_elf(const unsigned char *magic_bytes) {
    return memcmp(magic_bytes, ELF_MAGIC_BYTES, MAGIC_BYTE_COUNT) == 0;
}

// Get the class of an ELF: 32-bit or 64-bit
uint8_t get_elf_class(FILE *file) {
    uint8_t elf_class;
    fseek(file, MAGIC_BYTE_COUNT, SEEK_SET);
    fread(&elf_class, sizeof(elf_class), 1, file);
    return elf_class;
}

// Parse the 64-bit ELF file header
elf64_hdr parse_elf64_hdr(FILE *file) {
    fseek(file, 0L, SEEK_SET);
    elf64_hdr header;
    fread(&header, sizeof(header), 1, file);

    return header;
}

// Print the 64-bit ELF file header
void print_elf64_hdr(const elf64_hdr *header) {
    printf("ELF File Header:\n");

    if (header == NULL) {
        printf("Empty\n");
        return;
    }

    printf("Magic number: %#02x %#02x %#02x %#02x (%#02x %c %c %c)\n",
           header->e_ident[0], header->e_ident[1], header->e_ident[2],
           header->e_ident[3], header->e_ident[0], header->e_ident[1],
           header->e_ident[2], header->e_ident[3]);
    printf("Class: %d\n", header->e_ident[4]);
    printf("Data (Endianness): %d\n", header->e_ident[5]);
    printf("Version: %d\n", header->e_ident[6]);
    printf("OS/ABI: %#02x\n", header->e_ident[7]);
    printf("ABI version: %#02x\n", header->e_ident[8]);
    printf("Type: %#04x\n", header->e_type);
    printf("Machine: %#03x\n", header->e_machine);
    printf("Version: %d\n", header->e_version);
    printf("Entry address: %#lx\n", header->e_entry);
    printf("Program header offset: %lu B into the file\n", header->e_phoff);
    printf("Section header offset: %lu B into the file\n", header->e_shoff);
    printf("Flags: %#x\n", header->e_flags);
    printf("This header's size: %d B\n", header->e_ehsize);
    printf("Program header entry size: %d B\n", header->e_phentsize);
    printf("No. of program header entries: %d\n", header->e_phnum);
    printf("Section header entry size: %d B\n", header->e_shentsize);
    printf("No. of section header entries: %d\n", header->e_shnum);
    printf("Section header string index table offset: %d\n",
           header->e_shstrndx);
    printf("\n");
}
