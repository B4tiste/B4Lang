#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

ASTNode *parse_factor(FILE *file);
ASTNode *parse_term(FILE *file);
ASTNode *parse_expression(FILE *file);
ASTNode *parse_comparison(FILE *file);
ASTNode *parse_arithmetic_expr(FILE *file);
ASTNode *parse(FILE *file);

#endif