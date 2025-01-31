#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"

void generate_ir_from_ast(ASTNode *node);
void generate_assembly_from_ast(FILE *file, ASTNode *node);
void save_assembly_to_file(ASTNode *ast, const char *filename);

#endif