#include "pelf.h"
#include <stdalign.h>
#include <stddef.h> // For 'NULL'
#include <stdio.h>
#include <stdlib.h> // For exit(), malloc() and free()
#include <string.h> // For strcmp()

elf64_hdr *ELF64_HDR = NULL;

int main(int argc, char *argv[]) {
    char *file_path = NULL;

    // Get file path from command line args
    if (argc < 2) {
        printf("ERROR: Insufficient arguments.\n");
        // usage();
        exit(1);
    } else {
        file_path = argv[1];
    }

    // Check if file exists, is an ELF and is a 64-bit ELF
    if (is_file_valid(file_path)) {
        if (get_elf_class(file_path) != 2) {
            printf("ERROR: This utility can only parse 64-bit ELF files.\n");
            exit(1);
        }
    } else {
        printf("ERROR: Invalid file.\n");
        exit(1);
    }

    // Parse ELF file header
    parse_elf64_hdr(file_path);
    print_elf64_hdr();

    return 0;
}

// Parse the 64-bit ELF file header
void parse_elf64_hdr(char *file_path) {
    FILE *file = fopen(file_path, "r");
    char *file_data = (char *)malloc(sizeof(char) * 65);
    ELF64_HDR = (elf64_hdr *)malloc(sizeof(elf64_hdr));

    fgets(file_data, 65, file);

    for (int i = 0; i < 16; i++) {
        ELF64_HDR->e_ident[i] = *(file_data + i);
    }

    ELF64_HDR->e_type = *(file_data + 16);
    ELF64_HDR->e_machine = *(file_data + 18);
    ELF64_HDR->e_version = *(file_data + 20);
    ELF64_HDR->e_entry = *(file_data + 24);
    ELF64_HDR->e_phoff = *(file_data + 32);
    ELF64_HDR->e_shoff = *(file_data + 40);
    ELF64_HDR->e_flags = *(file_data + 48);
    ELF64_HDR->e_ehsize = *(file_data + 52);
    ELF64_HDR->e_phentsize = *(file_data + 54);
    ELF64_HDR->e_phnum = *(file_data + 56);
    ELF64_HDR->e_shentsize = *(file_data + 58);
    ELF64_HDR->e_shnum = *(file_data + 60);
    ELF64_HDR->e_shstrndx = *(file_data + 62);

    free(file_data);
    fclose(file);
}

// Print the 64-bit ELF file header
void print_elf64_hdr() {
    printf("ELF File Header:\n");

    if (ELF64_HDR == NULL) {
        printf("Empty\n");
        return;
    }

    printf("Magic number: %#02x %#02x %#02x %#02x (%#02x %c %c %c)\n",
           ELF64_HDR->e_ident[0], ELF64_HDR->e_ident[1], ELF64_HDR->e_ident[2],
           ELF64_HDR->e_ident[3], ELF64_HDR->e_ident[0], ELF64_HDR->e_ident[1],
           ELF64_HDR->e_ident[2], ELF64_HDR->e_ident[3]);
    printf("Class: %d\n", ELF64_HDR->e_ident[4]);
    printf("Data (Endianness): %d\n", ELF64_HDR->e_ident[5]);
    printf("Version: %d\n", ELF64_HDR->e_ident[6]);
    printf("OS/ABI: %#02x\n", ELF64_HDR->e_ident[7]);
    printf("ABI version: %#02x\n", ELF64_HDR->e_ident[8]);
    printf("Type: %#04x\n", ELF64_HDR->e_type);
    printf("Machine: %#03x\n", ELF64_HDR->e_machine);
    printf("Version: %d\n", ELF64_HDR->e_version);
    printf("Entry address: %#lx\n", ELF64_HDR->e_entry);
    printf("Program header offset: %#lx\n", ELF64_HDR->e_phoff);
    printf("Section header offset: %#lx\n", ELF64_HDR->e_shoff);
    printf("Flags: %#x\n", ELF64_HDR->e_flags);
    printf("This header's size: %dB\n", ELF64_HDR->e_ehsize);
    printf("Program header entry size: %dB\n", ELF64_HDR->e_phentsize);
    printf("No. of program header entries: %d\n", ELF64_HDR->e_phnum);
    printf("Section header entry size: %dB\n", ELF64_HDR->e_shentsize);
    printf("No. of section header entries: %d\n", ELF64_HDR->e_shnum);
    printf("Section header string index table offset: %d\n",
           ELF64_HDR->e_shstrndx);
    printf("\n");
}

// Get the first four bytes of the file
// If the file is an ELF, this will be the magic number
char *get_magic_num(char *file_path) {
    FILE *file = fopen(file_path, "r");
    char *file_data = (char *)malloc(sizeof(char) * 5);
    fgets(file_data, 5, file);

    return file_data;
}

// Get the class of an ELF: 32-bit or 64-bit
int get_elf_class(char *file_path) {
    FILE *file = fopen(file_path, "r");
    char *file_data = (char *)malloc(sizeof(char) * 6);
    fgets(file_data, 6, file);
    int elf_class = *(file_data + 4);
    free(file_data);

    return elf_class;
}

// Check if the file exists and is an ELF file
bool is_file_valid(char *file_path) {
    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        return false;
    } else {
        fclose(file);

        // Check if file is an ELF
        char *elf_magic_num = "\x7f"
                              "ELF";
        char *file_data = get_magic_num(file_path);
        if (strcmp(file_data, elf_magic_num) == 0) {
            free(file_data);
            return true;
        } else {
            return false;
        }
    }
}

