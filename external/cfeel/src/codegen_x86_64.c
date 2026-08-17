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
    return current_os == OS_MACOS ? "L" : ".L"; 
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

static void push(const char* reg) {
    fprintf(out, "    push %s\n", reg);
    current_stack_offset -= 8;
}

static void pop(const char* reg) {
    fprintf(out, "    pop %s\n", reg);
    current_stack_offset += 8;
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
        fprintf(out, "    mov rax, %s\n", num_str);
        push("rax");
        return;
    }
    
    if (node->type == AST_IDENTIFIER) {
        int offset = get_var_offset(node->token.text, node->token.length);
        fprintf(out, "    mov rax, qword ptr [rbp %d]\n", offset);
        push("rax");
        return;
    }
    
    if (node->type == AST_STRING) {
        int id = add_string(node->token.text, node->token.length);
        fprintf(out, "    lea rax, [rip + %sstr_%d]\n", local_prefix(), id);
        push("rax");
        return;
    }
    
    if (node->type == AST_CALL) {
        for (int i = 0; i < node->children_count; i++) {
            gen_expr(node->children[i]);
        }
        
        const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        for (int i = node->children_count - 1; i >= 0; i--) {
            if (i < 6) {
                pop(arg_regs[i]);
            } else {

                fprintf(stderr, "Warning: More than 6 arguments not fully supported in x86_64 backend yet.\n");
                pop("r10");
            }
        }
        
        char func_name[64];
        strncpy(func_name, node->token.text, node->token.length);
        func_name[node->token.length] = '\0';
        

        fprintf(out, "    mov rax, rsp\n");
        fprintf(out, "    and rax, 15\n");
        fprintf(out, "    sub rsp, rax\n");
        fprintf(out, "    push rax\n"); 
        

        fprintf(out, "    xor eax, eax\n"); 
        
        fprintf(out, "    call %s%s\n", prefix(), func_name);
        
        fprintf(out, "    pop rcx\n");
        fprintf(out, "    add rsp, rcx\n"); 
        
        push("rax"); 
        return;
    }
    
    if (node->type == AST_BINOP) {
        gen_expr(node->left);
        gen_expr(node->right);
        
        pop("rbx"); 
        pop("rax"); 
        
        if (node->op.type == TOKEN_PLUS) {
            fprintf(out, "    add rax, rbx\n");
        } else if (node->op.type == TOKEN_MINUS) {
            fprintf(out, "    sub rax, rbx\n");
        } else if (node->op.type == TOKEN_STAR) {
            fprintf(out, "    imul rax, rbx\n");
        } else if (node->op.type == TOKEN_SLASH) {
            fprintf(out, "    cqo\n");
            fprintf(out, "    idiv rbx\n");
        }
        push("rax");
        return;
    }
}

static void gen_statement(ASTNode* node) {
    if (node->type == AST_VAR_DECL) {
        gen_expr(node->left);
        add_var(node->token.text, node->token.length);
        return;
    }
    
    if (node->type == AST_CALL || node->type == AST_BINOP || node->type == AST_NUMBER || node->type == AST_IDENTIFIER || node->type == AST_STRING) {
        gen_expr(node);
        pop("rax"); 
    }
}

static void gen_function(ASTNode* node) {
    char func_name[64];
    strncpy(func_name, node->token.text, node->token.length);
    func_name[node->token.length] = '\0';
    
    fprintf(out, ".global %s%s\n", prefix(), func_name);
    fprintf(out, "%s%s:\n", prefix(), func_name);
    

    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    current_stack_offset = 0;
    
    for (int i = 0; i < node->children_count; i++) {
        gen_statement(node->children[i]);
    }
    

    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    xor eax, eax\n"); 
    fprintf(out, "    ret\n");
}

void generate_code_x86_64(ASTNode* program, const char* output_filename, TargetOS os) {
    current_os = os;
    out = fopen(output_filename, "w");
    if (!out) {
        fprintf(stderr, "Could not open output file %s\n", output_filename);
        exit(1);
    }
    
    fprintf(out, ".intel_syntax noprefix\n");
    fprintf(out, ".text\n");
    
    for (int i = 0; i < program->children_count; i++) {
        if (program->children[i]->type == AST_FUNCTION) {
            gen_function(program->children[i]);
        }
    }
    
    fprintf(out, ".data\n");
    for (int i = 0; i < string_count; i++) {
        fprintf(out, "%sstr_%d: .asciz \"%s\"\n", local_prefix(), string_pool[i].id, string_pool[i].str);
    }
    fprintf(out, "\n");
    
    fclose(out);
}
