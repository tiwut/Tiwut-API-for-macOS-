#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static const char* source_code;
static int current_pos;
static int line_number;

void init_lexer(const char* source) {
    source_code = source;
    current_pos = 0;
    line_number = 1;
}

static Token make_token(TokenType type, int start, int length) {
    Token token;
    token.type = type;
    token.text = (char*)(source_code + start);
    token.length = length;
    token.line = line_number;
    return token;
}

static Token error_token(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.text = (char*)message;
    token.length = strlen(message);
    token.line = line_number;
    return token;
}

static char advance() {
    current_pos++;
    return source_code[current_pos - 1];
}

static char peek() {
    return source_code[current_pos];
}

static void skip_whitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line_number++;
                advance();
                break;
            default:
                return;
        }
    }
}

static int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static Token identifier_or_keyword() {
    int start = current_pos - 1;
    while (is_alpha(peek()) || is_digit(peek())) {
        advance();
    }
    
    int length = current_pos - start;
    const char* text = source_code + start;
    

    if (length == 2 && strncmp(text, "fn", 2) == 0) return make_token(TOKEN_FN, start, length);
    if (length == 3 && strncmp(text, "let", 3) == 0) return make_token(TOKEN_LET, start, length);
    if (length == 5 && strncmp(text, "print", 5) == 0) return make_token(TOKEN_PRINT, start, length);
    if (length == 6 && strncmp(text, "extern", 6) == 0) return make_token(TOKEN_EXTERN, start, length);
    
    return make_token(TOKEN_IDENTIFIER, start, length);
}

static Token number() {
    int start = current_pos - 1;
    while (is_digit(peek())) {
        advance();
    }
    return make_token(TOKEN_NUMBER, start, current_pos - start);
}

static Token string() {
    int start = current_pos;
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\n') line_number++;
        advance();
    }
    
    if (peek() == '\0') return error_token("Unterminated string.");
    
    int length = current_pos - start;
    advance();
    
    return make_token(TOKEN_STRING, start, length);
}

Token next_token(void) {
    skip_whitespace();
    
    if (peek() == '\0') return make_token(TOKEN_EOF, current_pos, 0);
    
    char c = advance();
    
    if (is_alpha(c)) return identifier_or_keyword();
    if (is_digit(c)) return number();
    if (c == '"') return string();
    
    switch (c) {
        case '=': return make_token(TOKEN_EQUALS, current_pos - 1, 1);
        case '+': return make_token(TOKEN_PLUS, current_pos - 1, 1);
        case '-': return make_token(TOKEN_MINUS, current_pos - 1, 1);
        case '*': return make_token(TOKEN_STAR, current_pos - 1, 1);
        case '/': return make_token(TOKEN_SLASH, current_pos - 1, 1);
        case '(': return make_token(TOKEN_LPAREN, current_pos - 1, 1);
        case ')': return make_token(TOKEN_RPAREN, current_pos - 1, 1);
        case '{': return make_token(TOKEN_LBRACE, current_pos - 1, 1);
        case '}': return make_token(TOKEN_RBRACE, current_pos - 1, 1);
        case ';': return make_token(TOKEN_SEMICOLON, current_pos - 1, 1);
        case ',': return make_token(TOKEN_COMMA, current_pos - 1, 1);
    }
    
    return error_token("Unexpected character.");
}
