#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Analyse une expression récursivement (3 + y * 2)
ASTNode *parse_expression(FILE *file)
{
    Token token = get_next_token(file);

    // Si c'est un nombre ou une variable on retourne un noeud de valeur
    if (token.type == TOKEN_NUMBER || token.type == TOKEN_IDENTIFIER)
    {
        ASTNode *node = malloc(sizeof(ASTNode));

        node->type = NODE_EXPRESSION;
        strcpy(node->variable, token.value);
        node->left = node->right = NULL;

        return node;
    }

    // Si c'est une parenthèse ouvrante, on analyse l'expression à l'intérieur
    if (token.type == TOKEN_SYMBOL && strcmp(token.value, "(") == 0)
    {
        ASTNode *left = parse_expression(file);
        Token op = get_next_token(file); // Opérateur
        ASTNode *right = parse_expression(file);
        Token closing = get_next_token(file); // ")"

        if (closing.type != TOKEN_SYMBOL || strcmp(closing.value, ")") != 0)
        {
            printf("Erreur de syntaxte : Parenthèse fermante manquante\n");
        }

        return create_expression_node(op.value, left, right);
    }

    return NULL;
}

// Fonction pour parser le code
ASTNode *parse(FILE *file)
{
    Token token;
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while ((token = get_next_token(file)).type != TOKEN_EOF)
    {
        if (token.type == TOKEN_KEYWORD && strcmp(token.value, "int") == 0)
        {
            // Déclaration de variable (int x;)
            Token var = get_next_token(file);
            Token semi = get_next_token(file);

            if (var.type == TOKEN_IDENTIFIER && semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_declaration_node(var.value);
                if (!head)
                {
                    head = node;
                }
                else
                {
                    current->next = node;
                }
                current = node;
            }
        }
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "return") == 0)
        {
            // return x ou return EXPRESSION
            ASTNode *expr = parse_expression(file); // Lire l'expression
            Token semi = get_next_token(file);

            if (semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_return_node(expr);
                if (!head)
                {
                    head = node;
                }
                else
                {
                    current->next = node;
                }
                current = node;
            }
        }
        else if (token.type == TOKEN_IDENTIFIER)
        {
            // Affectation (x = ...)
            char var[MAX_TOKEN_LENGTH];
            strcpy(var, token.value);

            Token eq = get_next_token(file);
            if (eq.type == TOKEN_SYMBOL && strcmp(eq.value, "=") == 0)
            {
                ASTNode *expr = parse_expression(file);
                Token semi = get_next_token(file);

                if (semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
                {
                    ASTNode *node = create_assignment_node(var, expr);
                    if (!head)
                    {
                        head = node;
                    }
                    else
                    {
                        current->next = node;
                    }
                    current = node;
                }
            }
        }
    }

    return head;
}