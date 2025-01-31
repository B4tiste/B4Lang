#ifndef AST_H
#define AST_H

#include "lexer.h"

// Tableau des strings des types de node
extern const char node_type_string[4][MAX_TOKEN_LENGTH];

// Différents types de noeuds
typedef enum
{
    NODE_DECLARATION, // int x;
    NODE_ASSIGNMENT,  // x = 42;
    NODE_RETURN,      // return x;
    NODE_EXPRESSION,  // x + y * 2;
    NODE_IF,          // Condition IF
    NODE_ELSE,        // Condition ELSE
} NodeType;

// Structure d'un noeud AST
typedef struct ASTNode
{
    NodeType type;                   // Type du noeud
    char variable[MAX_TOKEN_LENGTH]; // Nom de la variable
    char value[MAX_TOKEN_LENGTH];    // Valeur
    struct ASTNode *left;            // Pour stocker les expressions
    struct ASTNode *right;           // x = (left + right)
    struct ASTNode *next;            // Prochaine noeud
    struct ASTNode *condition;       // Condition
    struct ASTNode *then_branch;     // Bloc IF
    struct ASTNode *else_branch;     // Bloc ELSE
} ASTNode;

// Fonctions de gestion de l'AST
ASTNode *create_declaration_node(char *var);
ASTNode *create_assignment_node(char *var, ASTNode *expr);
ASTNode *create_return_node(ASTNode *expr);
ASTNode *create_expression_node(char *op, ASTNode *left, ASTNode *right);
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

void free_ast(ASTNode *node);
void print_ast(ASTNode *node);
void print_ast_tree(ASTNode *node, int indent, int is_left);

#endif