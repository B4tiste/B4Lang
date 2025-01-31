#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Tableau des strings des types de node
const char node_type_string[4][MAX_TOKEN_LENGTH] = {
    "NODE_DECLARATION",
    "NODE_ASSIGNMENT",
    "NODE_RETURN",
    "NODE_EXPRESSION",
};

// Créer un noeuf de déclaration
ASTNode *create_declaration_node(char *var)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_DECLARATION;
    strcpy(node->variable, var);
    node->left = node->right = node->next = NULL;

    return node;
}

// Créer un noeuf d'affectation (int x = expr;)
ASTNode *create_assignment_node(char *var, ASTNode *expr)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_ASSIGNMENT;
    strcpy(node->variable, var);
    node->left = expr; // L'expression assignée à la var
    node->right = NULL;
    node->next = NULL;

    return node;
}

// Créer un nœud return (ex: return x; ou return (x + 3 * y);)
ASTNode *create_return_node(ASTNode *expr)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_RETURN;
    node->left = expr; // ✅ Stocke l'expression retournée
    node->next = NULL;

    return node;
}
// Créer un noeuf pour une expression (3 + y)
ASTNode *create_expression_node(char *op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_EXPRESSION;
    strcpy(node->variable, op);
    node->left = left;
    node->right = right;
    node->next = NULL;

    return node;
}

// Créer un noeud pour un IF/ELSE
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_IF;
    node->condition = condition;
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    node->next = NULL;

    return node;
}

// Libère la mémoire de l'AST
void free_ast(ASTNode *node)
{
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);

    free(node);
}

// Fonction debuf d'affichage de l'AST
void print_ast(ASTNode *node)
{
    while (node)
    {
        switch (node->type)
        {
        case NODE_DECLARATION:
            printf("Déclaration: %s\n", node->variable);
            break;

        case NODE_ASSIGNMENT:
            printf("Affectation: %s = ", node->variable);
            print_ast(node->left); // ✅ Afficher l'expression assignée
            printf("\n");
            break;

        case NODE_RETURN:
            printf("Return: ");
            print_ast(node->left); // ✅ Afficher l'expression retournée
            printf("\n");
            break;

        case NODE_EXPRESSION:
            if (node->left && node->right)
            { // Si c'est une opération (ex: 3 + y)
                printf("(");
                print_ast(node->left);
                printf(" %s ", node->variable);
                print_ast(node->right);
                printf(")");
            }
            else
            { // Si c'est juste un nombre ou une variable
                printf("%s", node->variable);
            }
            break;

        default:
            printf("Node inconnu\n");
            break;
        }

        node = node->next;
    }
}

// Affichage détaillé
void print_ast_tree(ASTNode *node, int indent, int is_left)
{
    if (!node)
        return;

    for (int i = 0; i < indent - 1; i++)
    {
        printf("  │ ");
    }
    if (indent > 0)
    {
        printf(is_left ? "  ├─ " : "  └─ ");
    }

    // printf("%s ", node_type_string[node->type]);

    switch (node->type)
    {
    case NODE_DECLARATION:
        printf("Déclaration: %s\n", node->variable);
        break;

    case NODE_ASSIGNMENT:
        printf("Affectation: %s\n", node->variable);
        print_ast_tree(node->left, indent + 1, 1);
        break;

    case NODE_RETURN:
        printf("Return\n");
        print_ast_tree(node->left, indent + 1, 0);
        break;

    case NODE_IF:
        printf("Condition `if`\n");
        print_ast_tree(node->condition, indent + 1, 1);
        printf("  ├─ Bloc `if`\n");
        print_ast_tree(node->then_branch, indent + 1, 1);
        if (node->else_branch)
        {
            printf("  └─ Bloc `else`\n");
            print_ast_tree(node->else_branch, indent + 1, 0);
        }
        break;

    case NODE_EXPRESSION:
        if (node->left && node->right)
        { // Opération binaire (ex: 3 + y)
            printf("Opération: %s\n", node->variable);
            print_ast_tree(node->left, indent + 1, 1);
            print_ast_tree(node->right, indent + 1, 0);
        }
        else
        { // Valeur simple (nombre ou variable)
            printf("Valeur: %s\n", node->variable);
        }
        break;

    default:
        printf("Nœud inconnu\n");
        break;
    }

    if (node->next)
    {
        print_ast_tree(node->next, indent, 0);
    }
}
