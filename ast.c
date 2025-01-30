#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Créer un noeuf d'affectation (int x = 42;)
ASTNode *create_assignment_node(char *var, char *val)
{
    ASTNode *node = malloc (sizeof(ASTNode));

    node->type = NODE_ASSIGNMENT;
    strcpy(node->variable, var);
    strcpy(node->value, val);
    node->next = NULL;

    return node;
}

// Créer un noeud de return (return x)
ASTNode *create_return_node(char *var)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_RETURN;
    strcpy(node->variable, var);
    node->next = NULL;

    return node;
}

// Libère la mémoire de l'AST
void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *temp = node;
        node = node->next;
        free(temp);
    }
}

// Fonction debuf d'affichage de l'AST
void print_ast(ASTNode *node)
{
    while (node)
    {
        switch (node->type)
        {
        case NODE_ASSIGNMENT:
            printf("Assignation: %s = %s\n", node->variable, node->value);
            break;

        case NODE_RETURN:
            printf("Return: %s\n", node->variable);
            break;

        default:
            printf("Node inexistant");
            break;
        }

        node = node->next;
    }
}