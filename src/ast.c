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

// Création d'un nœud de fonction
ASTNode *create_function_node(char *name, ASTNode *parameters, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_FUNCTION_DEF;
    strcpy(node->variable, name);
    node->parameters = parameters;
    node->body = body;
    node->next = NULL;

    return node;
}

// Création d'un nœud d'appel de fonction
ASTNode *create_function_call_node(char *name, ASTNode *arguments)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = NODE_FUNCTION_CALL;
    strcpy(node->variable, name);
    node->parameters = arguments;
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

    case NODE_FUNCTION_DEF:
        printf("Déclaration de fonction: %s\n", node->variable);
        printf("  ├─ Paramètres\n");
        print_ast_tree(node->parameters, indent + 1, 1);
        printf("  └─ Corps\n");
        print_ast_tree(node->body, indent + 1, 0);
        break;

    case NODE_FUNCTION_CALL:
        printf("Appel de fonction: %s\n", node->variable);
        printf("  ├─ Arguments\n");
        print_ast_tree(node->parameters, indent + 1, 0);
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

void export_ast_to_dot(FILE *file, ASTNode *node, int *node_count)
{
    if (!node)
        return;

    int current_id = (*node_count)++;

    // Définir l'étiquette du nœud en fonction de son type
    switch (node->type)
    {
    case NODE_DECLARATION:
        fprintf(file, "    node%d [label=\"Déclaration: %s\", shape=box];\n", current_id, node->variable);
        break;
    case NODE_ASSIGNMENT:
        fprintf(file, "    node%d [label=\"Affectation: %s\", shape=box];\n", current_id, node->variable);
        break;
    case NODE_RETURN:
        fprintf(file, "    node%d [label=\"Return\", shape=box];\n", current_id);
        break;
    case NODE_IF:
        fprintf(file, "    node%d [label=\"Condition IF\", shape=diamond];\n", current_id);
        break;
    case NODE_EXPRESSION:
        fprintf(file, "    node%d [label=\"%s\", shape=circle];\n", current_id, node->variable);
        break;
    case NODE_FUNCTION_DEF:
        fprintf(file, "    node%d [label=\"Fonction: %s\", shape=hexagon];\n", current_id, node->variable);
        break;
    case NODE_FUNCTION_CALL:
        fprintf(file, "    node%d [label=\"Appel de fonction: %s\", shape=ellipse];\n", current_id, node->variable);
        break;
    default:
        fprintf(file, "    node%d [label=\"Inconnu\", shape=octagon];\n", current_id);
        break;
    }

    // Exporter les relations
    if (node->parameters)
    {
        int param_id = *node_count;
        export_ast_to_dot(file, node->parameters, node_count);
        fprintf(file, "    node%d -> node%d [label=\"Paramètres\"];\n", current_id, param_id);
    }

    if (node->body)
    {
        int body_id = *node_count;
        export_ast_to_dot(file, node->body, node_count);
        fprintf(file, "    node%d -> node%d [label=\"Corps\"];\n", current_id, body_id);
    }

    if (node->left)
    {
        int left_id = *node_count;
        export_ast_to_dot(file, node->left, node_count);
        fprintf(file, "    node%d -> node%d;\n", current_id, left_id);
    }

    if (node->right)
    {
        int right_id = *node_count;
        export_ast_to_dot(file, node->right, node_count);
        fprintf(file, "    node%d -> node%d;\n", current_id, right_id);
    }

    if (node->then_branch)
    {
        int then_id = *node_count;
        export_ast_to_dot(file, node->then_branch, node_count);
        fprintf(file, "    node%d -> node%d [label=\"IF\"];\n", current_id, then_id);
    }

    if (node->else_branch)
    {
        int else_id = *node_count;
        export_ast_to_dot(file, node->else_branch, node_count);
        fprintf(file, "    node%d -> node%d [label=\"ELSE\"];\n", current_id, else_id);
    }

    if (node->next)
    {
        int next_id = *node_count;
        export_ast_to_dot(file, node->next, node_count);
        fprintf(file, "    node%d -> node%d [style=dashed];\n", current_id, next_id);
    }
}

void save_ast_to_dot(ASTNode *root, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Erreur ouverture fichier DOT");
        return;
    }

    fprintf(file, "digraph AST {\n");
    fprintf(file, "    rankdir=TB;\n"); // Orientation de haut en bas
    fprintf(file, "    node [style=filled, fontname=\"Arial\"];\n");

    int node_count = 0;
    export_ast_to_dot(file, root, &node_count);

    fprintf(file, "}\n");
    fclose(file);

    printf("AST exporté dans '%s'\n", filename);
}
