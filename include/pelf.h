#ifndef PELF_H // Include Guard
#define PELF_H

#include <stdbool.h> // For bool
#include <stdint.h>  // For unsigned integer datatypes
#include <stdio.h>   // For FILE

// Constants
#define MAGIC_BYTE_COUNT 4
const char *ELF_MAGIC_BYTES = "\x7F"
                              "ELF";

// Structure definitions
typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf64_hdr;

// Function declarations
elf64_hdr parse_elf64_hdr(FILE *file);
void print_elf64_hdr(const elf64_hdr *header);
void get_magic_bytes(FILE *file, unsigned char *magic_bytes);
uint8_t get_elf_class(FILE *file);
bool is_magic_bytes_elf(const unsigned char *magic_bytes);

#endif // PELF_H
