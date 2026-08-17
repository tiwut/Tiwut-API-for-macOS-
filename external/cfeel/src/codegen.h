#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

typedef enum {
    OS_MACOS,
    OS_LINUX
} TargetOS;

typedef enum {
    ARCH_ARM64,
    ARCH_X86_64
} TargetArch;

void generate_code_arm64(ASTNode* program, const char* output_filename, TargetOS os);
void generate_code_x86_64(ASTNode* program, const char* output_filename, TargetOS os);

#endif
