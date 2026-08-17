#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Token current;
static Token previous;

static void advance_token() {
    previous = current;
    current = next_token();
}

static void consume(TokenType type, const char* message) {
    if (current.type == type) {
        advance_token();
        return;
    }
    fprintf(stderr, "Error at line %d: %s\n", current.line, message);
    exit(1);
}

static ASTNode* new_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->children = NULL;
    node->children_count = 0;
    node->children_capacity = 0;
    return node;
}

static void add_child(ASTNode* parent, ASTNode* child) {
    if (parent->children_capacity == 0) {
        parent->children_capacity = 8;
        parent->children = (ASTNode**)malloc(sizeof(ASTNode*) * parent->children_capacity);
    } else if (parent->children_count >= parent->children_capacity) {
        parent->children_capacity *= 2;
        parent->children = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * parent->children_capacity);
    }
    parent->children[parent->children_count++] = child;
}

void init_parser(void) {
    advance_token();
}

static ASTNode* parse_expression();

static ASTNode* parse_primary() {
    if (current.type == TOKEN_NUMBER) {
        ASTNode* node = new_node(AST_NUMBER);
        node->token = current;
        advance_token();
        return node;
    }
    if (current.type == TOKEN_STRING) {
        ASTNode* node = new_node(AST_STRING);
        node->token = current;
        advance_token();
        return node;
    }
    if (current.type == TOKEN_IDENTIFIER) {
        Token name = current;
        advance_token();
        if (current.type == TOKEN_LPAREN) {
            advance_token();
            ASTNode* call = new_node(AST_CALL);
            call->token = name;
            if (current.type != TOKEN_RPAREN) {
                add_child(call, parse_expression());
                while (current.type == TOKEN_COMMA) {
                    advance_token();
                    add_child(call, parse_expression());
                }
            }
            consume(TOKEN_RPAREN, "Expect ')' after arguments.");
            return call;
        }
        ASTNode* node = new_node(AST_IDENTIFIER);
        node->token = name;
        return node;
    }
    if (current.type == TOKEN_LPAREN) {
        advance_token();
        ASTNode* expr = parse_expression();
        consume(TOKEN_RPAREN, "Expect ')' after expression.");
        return expr;
    }
    fprintf(stderr, "Error at line %d: Expect expression.\n", current.line);
    exit(1);
}

static ASTNode* parse_term() {
    ASTNode* left = parse_primary();
    
    while (current.type == TOKEN_STAR || current.type == TOKEN_SLASH) {
        Token op = current;
        advance_token();
        ASTNode* right = parse_primary();
        
        ASTNode* node = new_node(AST_BINOP);
        node->op = op;
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

static ASTNode* parse_expression() {
    ASTNode* left = parse_term();
    
    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS) {
        Token op = current;
        advance_token();
        ASTNode* right = parse_term();
        
        ASTNode* node = new_node(AST_BINOP);
        node->op = op;
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

static ASTNode* parse_statement() {
    if (current.type == TOKEN_LET) {
        advance_token();
        consume(TOKEN_IDENTIFIER, "Expect variable name.");
        Token name = previous;
        consume(TOKEN_EQUALS, "Expect '=' after variable name.");
        ASTNode* initializer = parse_expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
        
        ASTNode* node = new_node(AST_VAR_DECL);
        node->token = name;
        node->left = initializer;
        return node;
    }
    
    if (current.type == TOKEN_PRINT) {
        advance_token();
        consume(TOKEN_LPAREN, "Expect '(' after print.");
        ASTNode* expr = parse_expression();
        consume(TOKEN_RPAREN, "Expect ')' after print expression.");
        consume(TOKEN_SEMICOLON, "Expect ';' after print statement.");
        
        ASTNode* node = new_node(AST_PRINT);
        node->left = expr;
        return node;
    }
    

    ASTNode* expr = parse_expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    return expr;
}

static ASTNode* parse_function() {
    consume(TOKEN_FN, "Expect 'fn' to declare a function.");
    consume(TOKEN_IDENTIFIER, "Expect function name.");
    Token name = previous;
    
    consume(TOKEN_LPAREN, "Expect '(' after function name.");
    consume(TOKEN_RPAREN, "Expect ')' after parameters.");
    
    consume(TOKEN_LBRACE, "Expect '{' before function body.");
    
    ASTNode* func = new_node(AST_FUNCTION);
    func->token = name;
    
    while (current.type != TOKEN_RBRACE && current.type != TOKEN_EOF) {
        add_child(func, parse_statement());
    }
    
    consume(TOKEN_RBRACE, "Expect '}' after function body.");
    return func;
}

static ASTNode* parse_extern() {
    consume(TOKEN_EXTERN, "Expect 'extern'.");
    consume(TOKEN_FN, "Expect 'fn' after extern.");
    consume(TOKEN_IDENTIFIER, "Expect function name.");
    Token name = previous;
    consume(TOKEN_LPAREN, "Expect '(' after function name.");

    while (current.type != TOKEN_RPAREN && current.type != TOKEN_EOF) advance_token();
    consume(TOKEN_RPAREN, "Expect ')'.");
    consume(TOKEN_SEMICOLON, "Expect ';' after extern declaration.");
    
    ASTNode* ext = new_node(AST_EXTERN_DECL);
    ext->token = name;
    return ext;
}

ASTNode* parse_program() {
    ASTNode* program = new_node(AST_PROGRAM);
    while (current.type != TOKEN_EOF) {
        if (current.type == TOKEN_EXTERN) {
            add_child(program, parse_extern());
        } else {
            add_child(program, parse_function());
        }
    }
    return program;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    for (int i = 0; i < node->children_count; i++) {
        free_ast(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node);
}
