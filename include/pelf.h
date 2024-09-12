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
// 64-bit ELF (file) header
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

// 64-bit ELF segment (program) header
typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} elf64_phdr;

// 64-bit ELF dynamic section entry
typedef struct {
    uint64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    };
} elf64_dyn;

// Function declarations
elf64_hdr *parse_elf64_hdr(FILE *file);
elf64_shdr *parse_elf64_shdrs(FILE *file, const elf64_hdr *file_hdr);
elf64_phdr *parse_elf64_phdrs(FILE *file, const elf64_hdr *file_hdr);
void print_dynamic_deps(FILE *file, const elf64_hdr *file_hdr,
                        const elf64_shdr *sec_hdr_arr);
char *get_shstrtab(FILE *file, const elf64_hdr *file_hdr);
char *get_sec_data_using_name(FILE *file, const elf64_shdr *sec_hdr_arr,
                              const elf64_hdr *file_hdr, char *sec_name);
char *get_sec_data_using_offset(FILE *file, uint64_t file_offset,
                                uint64_t sec_data_size);
void print_elf64_hdr(const elf64_hdr *file_hdr);
void print_elf64_shdrs(const elf64_shdr *sec_hdr_arr, uint16_t num_sec,
                       char *shstrtab);
void print_elf64_phdrs(const elf64_phdr *prog_hdr_arr,
                       const elf64_hdr *file_hdr);
void get_magic_bytes(FILE *file, unsigned char *magic_bytes);
uint8_t get_elf_class(FILE *file);
bool is_magic_bytes_elf(const unsigned char *magic_bytes);

#endif // PELF_H

