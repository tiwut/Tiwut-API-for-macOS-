#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VAR_DECL,
    AST_EXTERN_DECL,
    AST_CALL,
    AST_PRINT,
    AST_BINOP,
    AST_NUMBER,
    AST_STRING,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    

    Token token; 
    

    Token op;
    struct ASTNode* left;
    struct ASTNode* right;
    

    struct ASTNode** children;
    int children_count;
    int children_capacity;

} ASTNode;

void init_parser(void);
ASTNode* parse_program(void);
void free_ast(ASTNode* node);

#endif
