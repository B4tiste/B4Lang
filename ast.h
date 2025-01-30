#ifndef AST_H
#define AST_H

#include "lexer.h"

// Différents types de noeuds
typedef enum {
    NODE_ASSIGNMENT,
    NODE_RETURN,
    NODE_EXPRESSION,
} NodeType;

// Structure d'un noeud AST
typedef struct ASTNode {
    NodeType type;                      // Type du noeud
    char variable[MAX_TOKEN_LENGTH];    // Nom de la variable
    char value[MAX_TOKEN_LENGTH];       // Valeur
    struct ASTNode *next;               // Prochaine noeud
} ASTNode;

// Fonctions de gestion de l'AST
ASTNode *create_assignment_node(char *var, char *val);
ASTNode *create_return_node(char *var);
void print_ast(ASTNode *node);
void free_ast(ASTNode *node);

#endif