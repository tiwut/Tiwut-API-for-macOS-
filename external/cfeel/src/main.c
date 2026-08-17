#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

char* read_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        exit(1);
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    
    fclose(f);
    return buffer;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-c] [-S] [-o output] [--target os-arch] [linker flags...] <file.cfeel>\n", argv[0]);
        return 1;
    }
    
    TargetOS target_os = OS_MACOS;
    TargetArch target_arch = ARCH_ARM64;
    
#if defined(__linux__)
    target_os = OS_LINUX;
#endif

#if defined(__x86_64__) || defined(_M_X64)
    target_arch = ARCH_X86_64;
#endif
    
    int flag_c = 0;
    int flag_S = 0;
    const char* out_filename = NULL;
    const char* in_filename = NULL;
    char linker_flags[2048] = "";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            flag_c = 1;
        } else if (strcmp(argv[i], "-S") == 0) {
            flag_S = 1;
        } else if (strcmp(argv[i], "--target") == 0) {
            if (i + 1 < argc) {
                const char* t = argv[++i];
                if (strcmp(t, "macos-arm64") == 0) {
                    target_os = OS_MACOS;
                    target_arch = ARCH_ARM64;
                } else if (strcmp(t, "macos-x86_64") == 0) {
                    target_os = OS_MACOS;
                    target_arch = ARCH_X86_64;
                } else if (strcmp(t, "linux-arm64") == 0) {
                    target_os = OS_LINUX;
                    target_arch = ARCH_ARM64;
                } else if (strcmp(t, "linux-x86_64") == 0) {
                    target_os = OS_LINUX;
                    target_arch = ARCH_X86_64;
                } else {
                    fprintf(stderr, "Error: Unknown target %s\n", t);
                    return 1;
                }
            } else {
                fprintf(stderr, "Error: Missing argument for --target\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                out_filename = argv[++i];
            } else {
                fprintf(stderr, "Error: Missing argument for -o\n");
                return 1;
            }
        } else if (strncmp(argv[i], "-l", 2) == 0 || strncmp(argv[i], "-L", 2) == 0 || strncmp(argv[i], "-F", 2) == 0) {
            strncat(linker_flags, " ", sizeof(linker_flags) - strlen(linker_flags) - 1);
            strncat(linker_flags, argv[i], sizeof(linker_flags) - strlen(linker_flags) - 1);
        } else if (strcmp(argv[i], "-framework") == 0) {
            if (i + 1 < argc) {
                strncat(linker_flags, " -framework ", sizeof(linker_flags) - strlen(linker_flags) - 1);
                strncat(linker_flags, argv[++i], sizeof(linker_flags) - strlen(linker_flags) - 1);
            } else {
                fprintf(stderr, "Error: Missing argument for -framework\n");
                return 1;
            }
        } else {
            in_filename = argv[i];
        }
    }
    
    if (!in_filename) {
        fprintf(stderr, "Error: No input file specified.\n");
        return 1;
    }
    
    char* source = read_file(in_filename);
    
    init_lexer(source);
    init_parser();
    ASTNode* program = parse_program();
    
    char base_name[256];
    strncpy(base_name, in_filename, sizeof(base_name));
    char* dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';
    
    char asm_name[256];
    snprintf(asm_name, sizeof(asm_name), "%s.s", base_name);
    
    if (target_arch == ARCH_ARM64) {
        generate_code_arm64(program, asm_name, target_os);
    } else {
        generate_code_x86_64(program, asm_name, target_os);
    }
    
    free_ast(program);
    free(source);
    
    if (flag_S) {

        if (out_filename) {
            rename(asm_name, out_filename);
        }
        return 0;
    }
    
    char cmd[2048];
    if (flag_c) {
        const char* final_out = out_filename ? out_filename : "a.o";
        snprintf(cmd, sizeof(cmd), "clang -c -O2 -o %s %s", final_out, asm_name);
    } else {
        const char* final_out = out_filename ? out_filename : "a.out";
        snprintf(cmd, sizeof(cmd), "clang -O2 -o %s %s %s", final_out, asm_name, linker_flags);
    }
    
    int ret = system(cmd);
    

    remove(asm_name);
    
    return ret;
}
