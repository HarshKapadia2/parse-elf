#include "pelf.h"
#include <errno.h>  // For strerr()
#include <stddef.h> // For 'NULL'
#include <stdio.h>  // For file functions, printf()
#include <stdlib.h> // For malloc(), free()
#include <string.h> // For memcmp()

FILE *file_temp;

int main(int argc, char *argv[]) {
    char *file_path = NULL;

    // Get file path from command line args
    if (argc < 2) {
        printf("ERROR: Insufficient arguments.\n");
        // TODO: usage();
        return 1;
    } else {
        file_path = argv[1];
    }

    // Try to open file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("ERROR: Could not open file '%s': %s", file_path,
               strerror(errno));
        return 2;
    }

    // Check if file is ELF
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

    // Check if ELF file is a 64-bit ELF
    if (get_elf_class(file) != 2) {
        fclose(file);
        printf("ERROR: This utility can only parse 64-bit ELF files.\n");
        return 1;
    }

    printf("64-bit ELF File Parser\n\n");
    printf("ELF details and value translations: "
           "https://en.wikipedia.org/wiki/Executable_and_Linkable_Format\n\n");
    printf("ELF file path: %s\n\n", file_path);

    file_temp = file;

    // Parse ELF file header
    elf64_hdr file_hdr = parse_elf64_hdr(file);
    print_elf64_hdr(&file_hdr);

    // Parse ELF section headers
    // TODO: Check if section headers exist
    elf64_shdr *sec_hdr_arr = parse_elf64_shdrs(file, &file_hdr);
    char *shstrtab = get_shstrtab(file, &file_hdr);
    print_elf64_shdrs(sec_hdr_arr, file_hdr.e_shnum, shstrtab);

    // Cleanup
    free(sec_hdr_arr);
    free(shstrtab);
    fclose(file);

    return 0;
}

// Get the first MAGIC_BYTE_COUNT bytes of the file
// If the file is an ELF, this will be the magic number
void get_magic_bytes(FILE *file, unsigned char *magic_bytes) {
    fseek(file, 0L, SEEK_SET);
    fread(magic_bytes, sizeof(unsigned char), MAGIC_BYTE_COUNT, file);
}

// Check if file's magic bytes match an ELF's magic bytes
bool is_magic_bytes_elf(const unsigned char *magic_bytes) {
    return memcmp(magic_bytes, ELF_MAGIC_BYTES, MAGIC_BYTE_COUNT) == 0;
}

// Get the class of an ELF: 1 (32-bit) or 2 (64-bit)
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

// Parse all the 64-bit ELF section headers
elf64_shdr *parse_elf64_shdrs(FILE *file, const elf64_hdr *file_hdr) {
    elf64_shdr *sec_hdr_arr = malloc(file_hdr->e_shnum * sizeof(elf64_shdr));

    fseek(file, file_hdr->e_shoff, SEEK_SET);
    fread(sec_hdr_arr, sizeof(elf64_shdr), file_hdr->e_shnum, file);

    return sec_hdr_arr;

    // TODO: Figure out why the code block below doesn't work
    //    elf64_shdr **sec_hdr_arr =
    //        (elf64_shdr **)malloc(sizeof(elf64_shdr *) * file_hdr->e_shnum);
    //    // uint64_t shdr_offset = file_hdr->e_shoff;
    //
    //    fseek(file, file_hdr->e_shoff, SEEK_SET);
    //
    //    for (int i = 0; i < file_hdr->e_shnum; i++) {
    //        elf64_shdr *sec_hdr = (elf64_shdr *)malloc(sizeof(elf64_shdr));
    //
    //        // fseek(file, shdr_offset, SEEK_SET);
    //        fread(sec_hdr, sizeof(file_hdr->e_shentsize), 1, file);
    //        sec_hdr_arr[i] = sec_hdr;
    //
    //        // printf("Size: %lu\n", sec_hdr->sh_size);
    //
    //        // shdr_offset = shdr_offset + file_hdr->e_shentsize;
    //    }
    //
    //    return sec_hdr_arr;
}

// Get the section header string table contents
char *get_shstrtab(FILE *file, const elf64_hdr *file_hdr) {
    elf64_shdr *shstrtab_sec_hdr = (elf64_shdr *)malloc(sizeof(elf64_shdr));

    fseek(file,
          (file_hdr->e_shoff + (file_hdr->e_shstrndx * sizeof(elf64_shdr))),
          SEEK_SET);
    fread(shstrtab_sec_hdr, sizeof(elf64_shdr), 1, file);

    char *shstrtab = get_sec_data_using_offset(
        file, shstrtab_sec_hdr->sh_offset, shstrtab_sec_hdr->sh_size);

    for (int i = 0; i < 10; i++) {
        printf("%s\n", shstrtab + i);
    }

    return shstrtab;
}

// Get section data using its size and an offset into the file
char *get_sec_data_using_offset(FILE *file, uint64_t file_offset,
                                uint64_t sec_data_size) {
    char *sec_data = (char *)malloc(sec_data_size);

    if (sec_data == NULL) {
        return NULL;
    }

    fseek(file, file_offset, SEEK_SET);
    fread(sec_data, sec_data_size, 1, file);

    return sec_data;
}

// Print the 64-bit ELF file header
void print_elf64_hdr(const elf64_hdr *file_hdr) {
    printf("ELF File file_header:\n");

    if (file_hdr == NULL) {
        printf("Empty\n");
        return;
    }

    printf("Magic number: %#02x %#02x %#02x %#02x (%#02x %c %c %c)\n",
           file_hdr->e_ident[0], file_hdr->e_ident[1], file_hdr->e_ident[2],
           file_hdr->e_ident[3], file_hdr->e_ident[0], file_hdr->e_ident[1],
           file_hdr->e_ident[2], file_hdr->e_ident[3]);
    printf("Class: %d\n", file_hdr->e_ident[4]);
    printf("Data (Endianness): %d\n", file_hdr->e_ident[5]);
    printf("Version: %d\n", file_hdr->e_ident[6]);
    printf("OS/ABI: %#02x\n", file_hdr->e_ident[7]);
    printf("ABI version: %#02x\n", file_hdr->e_ident[8]);
    printf("Type: %#04x\n", file_hdr->e_type);
    printf("Machine: %#03x\n", file_hdr->e_machine);
    printf("Version: %d\n", file_hdr->e_version);
    printf("Entry address: %#lx\n", file_hdr->e_entry);
    printf("Program file_hdr offset: %lu B into the file\n", file_hdr->e_phoff);
    printf("Section file_hdr offset: %lu B into the file\n", file_hdr->e_shoff);
    printf("Flags: %#x\n", file_hdr->e_flags);
    printf("This file_hdr's size: %d B\n", file_hdr->e_ehsize);
    printf("Program file_hdr entry size: %d B\n", file_hdr->e_phentsize);
    printf("No. of program file_hdr entries: %d\n", file_hdr->e_phnum);
    printf("Section file_hdr entry size: %d B\n", file_hdr->e_shentsize);
    printf("No. of section file_hdr entries: %d\n", file_hdr->e_shnum);
    printf("Section file_hdr string index table offset: %d\n",
           file_hdr->e_shstrndx);
    printf("\n");
}

// Print all the 64-bit ELF section headers
void print_elf64_shdrs(const elf64_shdr *sec_hdr_arr, uint16_t num_sec,
                       char *shstrtab) {
    printf("ELF File Section Headers:\n");
    printf("[No.]\tName\n");
    printf("\tType\t\tAddress\t\tOffset\n");
    printf("\tSize\t\tEntSize\t\tFlags  Link  \tInfo  Align\n");
    printf("-------------------------------------------------------------------"
           "--\n");

    for (int i = 0; i < num_sec; i++) {
        const elf64_shdr sec_hdr = sec_hdr_arr[i];

        printf("[%d]\t", i);
        printf("%s", (shstrtab + sec_hdr.sh_name));

        printf("\n\t");

        printf("%#x\t\t", sec_hdr.sh_type);
        printf("%#lx\t\t", sec_hdr.sh_addr);
        printf("%lu", sec_hdr.sh_offset);

        printf("\n\t");

        printf("%lu\t\t", sec_hdr.sh_size);
        printf("%lu\t\t", sec_hdr.sh_entsize);
        printf("%#lx    ", sec_hdr.sh_flags);
        printf("%d  \t", sec_hdr.sh_link);
        printf("%d     ", sec_hdr.sh_info);
        printf("%lu", sec_hdr.sh_addralign);

        printf("\n-------------------------------------------------------------"
               "--------\n");
    }

    printf("\n");
}

