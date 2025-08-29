#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "parser.h"
#include "lexer.h"
#include <stdio.h>

// Statement parsing functions
void parse_variable_declaration(const char *line, FILE *out, ParserContext *ctx);
void parse_array_declaration(const char *line, FILE *out, ParserContext *ctx);
void parse_map_declaration(const char *line, FILE *out, ParserContext *ctx);
void parse_print_statement(const char *line, FILE *out, ParserContext *ctx);
void parse_if_statement(const char *line, FILE *out, ParserContext *ctx);
void parse_else_statement(const char *line, FILE *out);
void parse_while_statement(const char *line, FILE *out, ParserContext *ctx);
void parse_for_statement(const char *line, FILE *out, ParserContext *ctx);
void parse_assignment(const char *line, FILE *out, ParserContext *ctx);
void parse_function_definition(const char *line, FILE *out, ParserContext *ctx);
void parse_return_statement(const char *line, FILE *out, ParserContext *ctx);

// Helper functions
bool is_variable_declaration(const char *line);
bool is_array_declaration(const char *line);
bool is_map_declaration(const char *line);
bool is_print_statement(const char *line);
bool is_if_statement(const char *line);
bool is_else_statement(const char *line);
bool is_while_statement(const char *line);
bool is_for_statement(const char *line);
bool is_assignment(const char *line);
bool is_function_definition(const char *line);
bool is_return_statement(const char *line);

#endif // STATEMENTS_H
