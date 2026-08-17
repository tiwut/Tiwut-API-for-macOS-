#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* out;

typedef struct {
    char name[64];
    int offset;
} Var;

static Var vars[256];
static int var_count = 0;
static int current_stack_offset = 0;
static TargetOS current_os;

static const char* prefix() {
    return current_os == OS_MACOS ? "_" : "";
}

static const char* local_prefix() {
    return current_os == OS_MACOS ? "l_" : ".L";
}

typedef struct {
    char str[256];
    int id;
} StringLiteral;

static StringLiteral string_pool[256];
static int string_count = 0;

static int add_string(const char* text, int length) {
    if (string_count >= 256) {
        fprintf(stderr, "Error: Too many string literals.\n");
        exit(1);
    }
    strncpy(string_pool[string_count].str, text, length);
    string_pool[string_count].str[length] = '\0';
    string_pool[string_count].id = string_count;
    return string_count++;
}

static void push() {
    fprintf(out, "    str x0, [sp, #-16]!\n");
    current_stack_offset -= 16;
}

static void pop(const char* reg) {
    fprintf(out, "    ldr %s, [sp], #16\n", reg);
    current_stack_offset += 16;
}

static int get_var_offset(const char* name, int len) {
    for (int i = 0; i < var_count; i++) {
        if (strncmp(vars[i].name, name, len) == 0 && vars[i].name[len] == '\0') {
            return vars[i].offset;
        }
    }
    fprintf(stderr, "Error: Undefined variable %.*s\n", len, name);
    exit(1);
}

static void add_var(const char* name, int len) {
    if (var_count >= 256) {
        fprintf(stderr, "Error: Too many variables.\n");
        exit(1);
    }
    strncpy(vars[var_count].name, name, len);
    vars[var_count].name[len] = '\0';
    vars[var_count].offset = current_stack_offset;
    var_count++;
}

static void gen_expr(ASTNode* node) {
    if (node->type == AST_NUMBER) {

        char num_str[64];
        strncpy(num_str, node->token.text, node->token.length);
        num_str[node->token.length] = '\0';
        fprintf(out, "    movz x0, #%s\n", num_str);
        push();
        return;
    }
    
    if (node->type == AST_IDENTIFIER) {
        int offset = get_var_offset(node->token.text, node->token.length);
        fprintf(out, "    ldr x0, [x29, #%d]\n", offset);
        push();
        return;
    }
    
    if (node->type == AST_STRING) {
        int id = add_string(node->token.text, node->token.length);
        fprintf(out, "    adrp x0, %sstr_%d@PAGE\n", local_prefix(), id);
        fprintf(out, "    add x0, x0, %sstr_%d@PAGEOFF\n", local_prefix(), id);
        push();
        return;
    }
    
    if (node->type == AST_CALL) {
        for (int i = 0; i < node->children_count; i++) {
            gen_expr(node->children[i]);
        }
        for (int i = node->children_count - 1; i >= 0; i--) {
            char reg[8];
            snprintf(reg, sizeof(reg), "x%d", i);
            pop(reg);
        }
        char func_name[64];
        strncpy(func_name, node->token.text, node->token.length);
        func_name[node->token.length] = '\0';
        fprintf(out, "    bl %s%s\n", prefix(), func_name);
        push(); 
        return;
    }
    
    if (node->type == AST_BINOP) {
        gen_expr(node->left);
        gen_expr(node->right);
        
        pop("x1"); 
        pop("x0"); 
        
        if (node->op.type == TOKEN_PLUS) {
            fprintf(out, "    add x0, x0, x1\n");
        } else if (node->op.type == TOKEN_MINUS) {
            fprintf(out, "    sub x0, x0, x1\n");
        } else if (node->op.type == TOKEN_STAR) {
            fprintf(out, "    mul x0, x0, x1\n");
        } else if (node->op.type == TOKEN_SLASH) {
            fprintf(out, "    sdiv x0, x0, x1\n");
        }
        push();
        return;
    }
}

static void gen_statement(ASTNode* node) {
    if (node->type == AST_VAR_DECL) {
        gen_expr(node->left);
        add_var(node->token.text, node->token.length);
        return;
    }
    
    if (node->type == AST_PRINT) {
        gen_expr(node->left);
        pop("x0");
        

        fprintf(out, "    mov x1, x0\n");
        fprintf(out, "    adrp x0, %sformat_str@PAGE\n", local_prefix());
        fprintf(out, "    add x0, x0, %sformat_str@PAGEOFF\n", local_prefix());
        

        fprintf(out, "    bl _printf\n");
        return;
    }
    
    if (node->type == AST_CALL || node->type == AST_BINOP || node->type == AST_NUMBER || node->type == AST_IDENTIFIER || node->type == AST_STRING) {
        gen_expr(node);
        pop("x0"); 
    }
}

static void gen_function(ASTNode* node) {

    char func_name[64];
    strncpy(func_name, node->token.text, node->token.length);
    func_name[node->token.length] = '\0';
    
    fprintf(out, ".global %s%s\n", prefix(), func_name);
    fprintf(out, ".align 2\n");
    fprintf(out, "%s%s:\n", prefix(), func_name);
    

    fprintf(out, "    stp x29, x30, [sp, #-16]!\n");
    fprintf(out, "    mov x29, sp\n");
    current_stack_offset = 0;
    
    for (int i = 0; i < node->children_count; i++) {
        gen_statement(node->children[i]);
    }
    

    fprintf(out, "    mov sp, x29\n");
    fprintf(out, "    ldp x29, x30, [sp], #16\n");
    fprintf(out, "    mov x0, #0\n"); 
    fprintf(out, "    ret\n");
}

void generate_code_arm64(ASTNode* program, const char* output_filename, TargetOS os) {
    current_os = os;
    out = fopen(output_filename, "w");
    if (!out) {
        fprintf(stderr, "Could not open output file %s\n", output_filename);
        exit(1);
    }
    
    fprintf(out, ".text\n");
    
    for (int i = 0; i < program->children_count; i++) {
        if (program->children[i]->type == AST_FUNCTION) {
            gen_function(program->children[i]);
        }
    }
    

    fprintf(out, ".data\n");
    fprintf(out, "%sformat_str: .asciz \"%%ld\\n\"\n", local_prefix());
    for (int i = 0; i < string_count; i++) {
        fprintf(out, "%sstr_%d: .asciz \"%s\"\n", local_prefix(), string_pool[i].id, string_pool[i].str);
    }
    fprintf(out, "\n");
    
    fclose(out);
}
