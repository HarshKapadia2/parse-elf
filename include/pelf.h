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
// 64-bit ELF file header
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

// 64-bit ELF section header
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} elf64_shdr;

// Function declarations
elf64_hdr parse_elf64_hdr(FILE *file);
elf64_shdr *parse_elf64_shdrs(FILE *file, const elf64_hdr *file_hdr);
void print_elf64_hdr(const elf64_hdr *header);
void print_elf64_shdrs(const elf64_shdr *sec_hdr_arr,
                       const elf64_hdr *file_hdr);
void get_magic_bytes(FILE *file, unsigned char *magic_bytes);
uint8_t get_elf_class(FILE *file);
bool is_magic_bytes_elf(const unsigned char *magic_bytes);

#endif // PELF_H

