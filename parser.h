#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

ASTNode *parse_expression(FILE *file);
ASTNode *parse(FILE *file);

#endif