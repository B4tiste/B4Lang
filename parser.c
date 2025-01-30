#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

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
            // int x = 42;
            Token var = get_next_token(file);   // Nom de la variable
            Token eq = get_next_token(file);    // Signe '='
            Token val = get_next_token(file);   // Valeur
            Token semi = get_next_token(file);  // Point-virgule ';'

            if (var.type == TOKEN_IDENTIFIER && eq.type == TOKEN_SYMBOL && strcmp(eq.value, "=") == 0
            && val.type == TOKEN_NUMBER && semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_assignment_node(var.value, val.value);
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
            else
            {
                printf("Erreur de syntaxe\n");
                return NULL;
            }
        }
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "return") == 0)
        {
            // return x;
            Token var = get_next_token(file);
            Token semi = get_next_token(file);

            if (var.type == TOKEN_IDENTIFIER && semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_return_node(var.value);

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
            else
            {
                printf("Erreur de syntaxe\n");
                return NULL;
            }
        }
    }

    return head;
}