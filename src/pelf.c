#include "pelf.h"
#include <errno.h>  // For strerr()
#include <stddef.h> // For 'NULL'
#include <stdio.h>  // For file functions, printf()
#include <stdlib.h> // For malloc(), free()
#include <string.h> // For memcmp(), strcmp()

int main(int argc, char *argv[]) {
    char *file_path = NULL;

    // Get file path from command line args
    if (argc < 2) {
        printf("ERROR: Insufficient arguments. Please provide a path to a "
               "64-bit ELF file.\n\n");
        return 1;
    } else {
        file_path = argv[1];
    }

    // Try to open file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("ERROR: Could not open file '%s': %s\n\n", file_path,
               strerror(errno));
        return 2;
    }

    // Check if file is ELF
    unsigned char magic_bytes[MAGIC_BYTE_COUNT];
    get_magic_bytes(file, magic_bytes);

    if (!is_magic_bytes_elf(magic_bytes)) {
        fclose(file);
        printf("ERROR: File at '%s' does not have ELF header, got: %02x %02x "
               "%02x %02x\n\n",
               file_path, magic_bytes[0], magic_bytes[1], magic_bytes[2],
               magic_bytes[3]);
        return 2;
    }

    // Check if ELF file is a 64-bit ELF
    if (get_elf_class(file) != 2) {
        fclose(file);
        printf("ERROR: This utility can only parse 64-bit ELF files.\n\n");
        return 1;
    }

    printf("64-bit ELF File Parser\n\n\n");
    printf("ELF details and value translations: "
           "https://en.wikipedia.org/wiki/Executable_and_Linkable_Format\n\n");
    printf("ELF file path: %s\n\n\n", file_path);

    // Print ELF file header
    elf64_hdr *file_hdr = parse_elf64_hdr(file);

    if (file_hdr == NULL) {
        fclose(file);
        printf("ERROR: File header could not be parsed.\n\n");
        return 3;
    }

    print_elf64_hdr(file_hdr);

    // Print ELF section headers
    elf64_shdr *sec_hdr_arr = NULL;
    char *shstrtab = NULL;
    if (file_hdr->e_shnum > 0) {
        sec_hdr_arr = parse_elf64_shdrs(file, file_hdr);

        if (sec_hdr_arr == NULL) {
            fclose(file);
            free(file_hdr);
            printf("ERROR: Section headers could not be parsed.\n\n");
            return 3;
        }

        shstrtab = get_shstrtab(file, file_hdr);

        if (shstrtab == NULL) {
            fclose(file);
            free(file_hdr);
            free(sec_hdr_arr);
            printf(
                "ERROR: Section header string table could not be parsed.\n\n");
            return 3;
        }

        print_elf64_shdrs(sec_hdr_arr, file_hdr->e_shnum, shstrtab);
    } else {
        printf("NOTE: No section headers were found.\n\n");
    }

    // Print ELF segment (program) headers
    elf64_phdr *prog_hdr_arr = NULL;
    if (file_hdr->e_phnum > 0) {
        prog_hdr_arr = parse_elf64_phdrs(file, file_hdr);

        if (shstrtab == NULL) {
            fclose(file);
            free(file_hdr);
            free(sec_hdr_arr);
            free(shstrtab);
            printf("ERROR: Program (segment) headers could not be parsed.\n\n");
            return 3;
        }

        print_elf64_phdrs(prog_hdr_arr, file_hdr);
    } else {
        printf("NOTE: No program (segment) headers were found.\n\n");
    }

    // Print dynamic dependencies
    print_dynamic_deps(file, file_hdr, sec_hdr_arr);

    // Cleanup
    free(file_hdr);
    free(sec_hdr_arr);
    free(shstrtab);
    free(prog_hdr_arr);
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
elf64_hdr *parse_elf64_hdr(FILE *file) {
    elf64_hdr *file_hdr = (elf64_hdr *)malloc(sizeof(elf64_hdr));

    fseek(file, 0L, SEEK_SET);
    fread(file_hdr, sizeof(elf64_hdr), 1, file);

    return file_hdr;
}

// Parse all the 64-bit ELF section headers
elf64_shdr *parse_elf64_shdrs(FILE *file, const elf64_hdr *file_hdr) {
    elf64_shdr *sec_hdr_arr = malloc(file_hdr->e_shnum * sizeof(elf64_shdr));

    if (sec_hdr_arr == NULL) {
        return NULL;
    }

    fseek(file, file_hdr->e_shoff, SEEK_SET);
    fread(sec_hdr_arr, sizeof(elf64_shdr), file_hdr->e_shnum, file);

    return sec_hdr_arr;
}

// Parse all the 64-bit ELF segment (program) headers
elf64_phdr *parse_elf64_phdrs(FILE *file, const elf64_hdr *file_hdr) {
    elf64_phdr *prog_hdr_arr = malloc(file_hdr->e_phnum * sizeof(elf64_phdr));

    if (prog_hdr_arr == NULL) {
        return NULL;
    }

    fseek(file, file_hdr->e_phoff, SEEK_SET);
    fread(prog_hdr_arr, sizeof(elf64_phdr), file_hdr->e_phnum, file);

    return prog_hdr_arr;
}

// Print the names and locations of dynamically loaded
// libraries/dependencies
void print_dynamic_deps(FILE *file, const elf64_hdr *file_hdr,
                        const elf64_shdr *sec_hdr_arr) {
    int dyn_shdr_idx = -1;
    char *dynstr_sec_data =
        get_sec_data_using_name(file, sec_hdr_arr, file_hdr, ".dynstr");

    // Find '.dynamic' section header
    for (int i = 0; i < file_hdr->e_shnum; i++) {
        const elf64_shdr sec_hdr = sec_hdr_arr[i];

        if (sec_hdr.sh_type == 0x6) {
            dyn_shdr_idx = i;
            break;
        }
    }

    if (dyn_shdr_idx < 0) {
        free(dynstr_sec_data);
        printf("NOTE: No dynamic section was found.\n\n");
        return;
    }

    // Get the 'elf64_dyn' entries in the dynamic section
    const elf64_shdr dyn_shdr = sec_hdr_arr[dyn_shdr_idx];
    uint64_t dyn_sec_offset = dyn_shdr.sh_offset;
    uint64_t dyn_sec_size = dyn_shdr.sh_size;
    uint64_t dyn_ent_size = dyn_shdr.sh_entsize;
    int dyn_ent_num = dyn_sec_size / dyn_ent_size;

    elf64_dyn *dyn_ent_arr = malloc(dyn_ent_num * sizeof(elf64_dyn));

    if (dyn_ent_arr == NULL) {
        free(dynstr_sec_data);
        printf("NOTE: No memory could be allocated for dynamic section "
               "entries.\n\n");
        return;
    }

    fseek(file, dyn_sec_offset, SEEK_SET);
    fread(dyn_ent_arr, sizeof(elf64_dyn), dyn_ent_num, file);

    // Print the library names
    printf("Dynamic dependencies listed in the ELF file:\n");
    for (int i = 0; i < dyn_ent_num; i++) {
        elf64_dyn dyn_ent = dyn_ent_arr[i];

        if (dyn_ent.d_tag == 1) {
            uint64_t str_tab_offset = dyn_ent.d_val;

            printf("-> %s\n", (dynstr_sec_data + str_tab_offset));
        }
    }
    printf("\n");
    printf("NOTE: Each dependency might have its own dependencies.\n");
    printf("\n\n");

    // Cleanup
    free(dyn_ent_arr);
    free(dynstr_sec_data);
}

// Get the section header string table contents
char *get_shstrtab(FILE *file, const elf64_hdr *file_hdr) {
    if (file_hdr->e_shstrndx == SHN_UNDEF) {
        printf("NOTE: Empty section name string table.\n\n");
        return NULL;
    }

    uint32_t shstrndx;
    if (file_hdr->e_shstrndx != SHN_XINDEX) {
        shstrndx = file_hdr->e_shstrndx;
    } else {
        // file_hdr->e_shstrndx == SHN_XINDEX implies that the actual index
        // value is stored elsewhere, which in this case is the first section
        // header's sh_link member as per the standard

        elf64_shdr *first_sec_hdr = (elf64_shdr *)malloc(sizeof(elf64_shdr));

        if (first_sec_hdr == NULL) {
            printf("NOTE: Couldn't allocate memory to set 'shstrndx'.\n\n");
            return NULL;
        }

        fseek(file, file_hdr->e_shoff, SEEK_SET);
        fread(first_sec_hdr, sizeof(elf64_shdr), 1, file);

        shstrndx = first_sec_hdr->sh_link;

        free(first_sec_hdr);
    }

    elf64_shdr *shstrtab_sec_hdr = (elf64_shdr *)malloc(sizeof(elf64_shdr));

    if (shstrtab_sec_hdr == NULL) {
        return NULL;
    }

    fseek(file, (file_hdr->e_shoff + (shstrndx * sizeof(elf64_shdr))),
          SEEK_SET);
    fread(shstrtab_sec_hdr, sizeof(elf64_shdr), 1, file);

    char *shstrtab = get_sec_data_using_offset(
        file, shstrtab_sec_hdr->sh_offset, shstrtab_sec_hdr->sh_size);

    return shstrtab;
}

// Get section data using its name
char *get_sec_data_using_name(FILE *file, const elf64_shdr *sec_hdr_arr,
                              const elf64_hdr *file_hdr, char *sec_name) {
    char *shstrtab = get_shstrtab(file, file_hdr);

    for (int i = 0; i < file_hdr->e_shnum; i++) {
        const elf64_shdr sec_hdr = sec_hdr_arr[i];
        char *curr_sec_name = shstrtab + sec_hdr.sh_name;

        if (strcmp(sec_name, curr_sec_name) == 0) {
            free(shstrtab);
            return get_sec_data_using_offset(file, sec_hdr.sh_offset,
                                             sec_hdr.sh_size);
        }
    }

    free(shstrtab);
    return NULL;
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
    printf("ELF File 'File Header':\n\n");

    if (file_hdr == NULL) {
        printf("NOTE: Empty.\n\n");
        return;
    }

    printf("-> Magic number: %#02x %#02x %#02x %#02x (%#02x %c %c %c)\n",
           file_hdr->e_ident[0], file_hdr->e_ident[1], file_hdr->e_ident[2],
           file_hdr->e_ident[3], file_hdr->e_ident[0], file_hdr->e_ident[1],
           file_hdr->e_ident[2], file_hdr->e_ident[3]);
    printf("-> Class: %d\n", file_hdr->e_ident[4]);
    printf("-> Data (Endianness): %d\n", file_hdr->e_ident[5]);
    printf("-> Version: %d\n", file_hdr->e_ident[6]);
    printf("-> OS/ABI: %#02x\n", file_hdr->e_ident[7]);
    printf("-> ABI version: %#02x\n", file_hdr->e_ident[8]);
    printf("-> Type: %#04x\n", file_hdr->e_type);
    printf("-> Machine: %#03x\n", file_hdr->e_machine);
    printf("-> Version: %d\n", file_hdr->e_version);
    printf("-> Entry address: %#lx\n", file_hdr->e_entry);
    printf("-> Program (segment) header table offset: %lu B into the file\n",
           file_hdr->e_phoff);
    printf("-> Section header table offset: %lu B into the file\n",
           file_hdr->e_shoff);
    printf("-> Flags: %#x\n", file_hdr->e_flags);
    printf("-> This header's size: %d B\n", file_hdr->e_ehsize);
    printf("-> Program (segment) header size: %d B\n", file_hdr->e_phentsize);
    printf("-> No. of program (segment) headers: %d\n", file_hdr->e_phnum);
    printf("-> Section header size: %d B\n", file_hdr->e_shentsize);
    printf("-> No. of section headers: %d\n", file_hdr->e_shnum);
    printf("-> Index of the 'section name string table' section header in the "
           "section header table: %d\n",
           file_hdr->e_shstrndx);
    printf("\n\n");
}

// Print all the 64-bit ELF section headers
void print_elf64_shdrs(const elf64_shdr *sec_hdr_arr, uint16_t num_sec,
                       char *shstrtab) {
    printf("ELF File Section Headers:\n\n");

    if (sec_hdr_arr == NULL) {
        printf("NOTE: Empty.\n\n");
        return;
    }

    printf("[No.]\tName\n");
    printf("\tType\t\tAddress\t\tOffset\n");
    printf("\tSize\t\tEntSize\t\tFlags  Link  \tInfo  Align\n");
    printf("---------------------------------------------------------------"
           "------\n");

    for (int i = 0; i < num_sec; i++) {
        const elf64_shdr sec_hdr = sec_hdr_arr[i];

        printf("[%d]\t", i);
        printf("%s", (shstrtab + sec_hdr.sh_name));

        printf("\n\t");

        printf("%s\t\t", get_sec_type_name(sec_hdr.sh_type));
        printf("%#lx\t\t", sec_hdr.sh_addr);
        printf("%lu", sec_hdr.sh_offset);

        printf("\n\t");

        printf("%lu\t\t", sec_hdr.sh_size);
        printf("%lu\t\t", sec_hdr.sh_entsize);
        printf("%#lx    ", sec_hdr.sh_flags);
        printf("%d  \t", sec_hdr.sh_link);
        printf("%d     ", sec_hdr.sh_info);
        printf("%lu", sec_hdr.sh_addralign);

        printf("\n---------------------------------------------------------"
               "------------\n");
    }

    printf("\n\n");
}

// Print all the 64-bit ELF segment (program) headers
void print_elf64_phdrs(const elf64_phdr *prog_hdr_arr,
                       const elf64_hdr *file_hdr) {
    printf("ELF File Segment (Program) Headers:\n\n");

    if (prog_hdr_arr == NULL) {
        printf("NOTE: Empty.\n\n");
        return;
    }

    printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\n");
    printf("\t\tFileSiz\t\tMemSiz\t\tFlags  Align\n");
    printf("---------------------------------------------------------------"
           "------\n");

    for (int i = 0; i < file_hdr->e_phnum; i++) {
        const elf64_phdr prog_hdr = prog_hdr_arr[i];

        printf("%#x\t\t", prog_hdr.p_type);
        printf("%#lx\t\t", prog_hdr.p_offset);
        printf("%#lx\t\t", prog_hdr.p_vaddr);
        printf("%#lx", prog_hdr.p_paddr);

        printf("\n\t\t");

        printf("%#lx\t\t", prog_hdr.p_filesz);
        printf("%#lx\t\t", prog_hdr.p_memsz);
        printf("%#x    ", prog_hdr.p_flags);
        printf("%#lx", prog_hdr.p_align);

        printf("\n---------------------------------------------------------"
               "------------\n");
    }

    printf("\n\n");
}

char *get_sec_type_name(uint32_t sec_type) {
    switch (sec_type) {
    case 0x0:
        return "NULL";
        break;
    case 0x1:
        return "PROGBITS";
        break;
    case 0x2:
        return "SYMTAB";
        break;
    case 0x3:
        return "STRTAB";
        break;
    case 0x4:
        return "RELA";
        break;
    case 0x5:
        return "HASH";
        break;
    case 0x6:
        return "DYNAMIC";
        break;
    case 0x7:
        return "NOTE";
        break;
    case 0x8:
        return "NOBITS";
        break;
    case 0x9:
        return "REL";
        break;
    case 0x0A:
        return "SHLIB";
        break;
    case 0x0B:
        return "DYNSYM";
        break;
    case 0x0E:
        return "INIT_ARRAY";
        break;
    case 0x0F:
        return "FINI_ARRAY";
        break;
    case 0x10:
        return "PREINIT_ARRAY";
        break;
    case 0x11:
        return "GROUP";
        break;
    case 0x12:
        return "SYMTAB_SHNDX";
        break;
    case 0x13:
        return "NUM";
        break;
    default:
        return "*****";
        break;
    }
}

