#ifndef PELF_H // Include Guard
#define PELF_H

#include <stdbool.h> // For 'true', 'false' and 'bool'
#include <stdint.h>  // For unsigned integer datatypes

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
void parse_elf64_hdr(char *file_path);
void print_elf64_hdr();
char *get_magic_num(char *file_path);
int get_elf_class(char *file_path);
bool is_file_valid(char *file_path);

#endif // PELF_H

