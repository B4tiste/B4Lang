#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// -----------------------------------------------------------------------------
// Parsing des expressions
// -----------------------------------------------------------------------------

// Parse un facteur : nombre, identifiant ou expression entre parenthèses
ASTNode *parse_factor(FILE *file)
{
    Token token = get_next_token(file);
    ASTNode *node = NULL;

    if (token.type == TOKEN_NUMBER || token.type == TOKEN_IDENTIFIER)
    {
        node = malloc(sizeof(ASTNode));
        node->type = NODE_EXPRESSION;
        strcpy(node->variable, token.value);
        node->left = node->right = NULL;
        return node;
    }
    else if (token.type == TOKEN_SYMBOL && strcmp(token.value, "(") == 0)
    {
        // Dans une parenthèse, on parse une expression complète (y compris les comparaisons)
        node = parse_comparison(file);
        Token closing = get_next_token(file);
        if (closing.type != TOKEN_SYMBOL || strcmp(closing.value, ")") != 0)
        {
            printf("Erreur de syntaxe : parenthèse fermante ')' manquante\n");
            return NULL;
        }
        return node;
    }
    else
    {
        printf("Erreur de syntaxe : facteur inattendu\n");
        return NULL;
    }
}

// Parse un terme : gère les multiplications et divisions
ASTNode *parse_term(FILE *file)
{
    ASTNode *node = parse_factor(file);
    if (!node)
        return NULL;

    while (1)
    {
        Token token = get_next_token(file);
        if (token.type == TOKEN_SYMBOL &&
            (strcmp(token.value, "*") == 0 || strcmp(token.value, "/") == 0))
        {
            ASTNode *right = parse_factor(file);
            if (!right)
                return NULL;
            node = create_expression_node(token.value, node, right);
        }
        else
        {
            // Reposer le token non consommé (attention : pour des tokens multi-caractères, il faudrait un mécanisme plus élaboré)
            ungetc(token.value[0], file);
            break;
        }
    }
    return node;
}

// Parse une expression arithmétique : gère les additions et soustractions
ASTNode *parse_arithmetic_expr(FILE *file)
{
    ASTNode *node = parse_term(file);
    if (!node)
        return NULL;

    while (1)
    {
        Token token = get_next_token(file);
        if (token.type == TOKEN_SYMBOL &&
            (strcmp(token.value, "+") == 0 || strcmp(token.value, "-") == 0))
        {
            ASTNode *right = parse_term(file);
            if (!right)
                return NULL;
            node = create_expression_node(token.value, node, right);
        }
        else
        {
            ungetc(token.value[0], file);
            break;
        }
    }
    return node;
}

// Parse une comparaison : gère les opérateurs relationnels
ASTNode *parse_comparison(FILE *file)
{
    ASTNode *node = parse_arithmetic_expr(file);
    if (!node)
        return NULL;

    while (1)
    {
        Token token = get_next_token(file);
        if (token.type == TOKEN_SYMBOL &&
            (strcmp(token.value, ">") == 0 || strcmp(token.value, "<") == 0 ||
             strcmp(token.value, ">=") == 0 || strcmp(token.value, "<=") == 0 ||
             strcmp(token.value, "==") == 0 || strcmp(token.value, "!=") == 0))
        {
            ASTNode *right = parse_arithmetic_expr(file);
            if (!right)
                return NULL;
            node = create_expression_node(token.value, node, right);
        }
        else
        {
            ungetc(token.value[0], file);
            break;
        }
    }
    return node;
}

// -----------------------------------------------------------------------------
// Fonction principale de parsing
// -----------------------------------------------------------------------------

ASTNode *parse(FILE *file)
{
    Token token;
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while ((token = get_next_token(file)).type != TOKEN_EOF)
    {

        // --- Déclaration de variable : "int x;"
        if (token.type == TOKEN_KEYWORD && strcmp(token.value, "int") == 0)
        {
            Token var = get_next_token(file);
            Token semi = get_next_token(file);

            if (var.type == TOKEN_IDENTIFIER &&
                semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_declaration_node(var.value);
                if (!head)
                {
                    head = node;
                    current = node;
                }
                else
                {
                    current->next = node;
                    current = node;
                }
            }
        }
        // --- Affectation : "x = expression;"
        else if (token.type == TOKEN_IDENTIFIER)
        {
            char var[MAX_TOKEN_LENGTH];
            strcpy(var, token.value);

            Token eq = get_next_token(file);
            if (eq.type == TOKEN_SYMBOL && strcmp(eq.value, "=") == 0)
            {
                ASTNode *expr = parse_comparison(file);
                Token semi = get_next_token(file);

                if (semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
                {
                    ASTNode *node = create_assignment_node(var, expr);
                    if (!head)
                    {
                        head = node;
                        current = node;
                    }
                    else
                    {
                        current->next = node;
                        current = node;
                    }
                }
                else
                {
                    printf("Erreur de syntaxe : ';' manquant après l'affectation de %s\n", var);
                }
            }
            else
            {
                printf("Erreur de syntaxe : '=' attendu après %s\n", var);
            }
        }
        // --- Structure conditionnelle : "if (condition) { ... } [else { ... }]"
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "if") == 0)
        {
            Token open_paren = get_next_token(file);
            if (strcmp(open_paren.value, "(") != 0)
            {
                printf("Erreur de syntaxe : 'if' sans parenthèse ouvrante '('\n");
                return NULL;
            }

            // La condition (comparaison entre variable et nombre ou entre deux nombres)
            ASTNode *condition = parse_comparison(file);

            Token close_paren = get_next_token(file);
            if (strcmp(close_paren.value, ")") != 0)
            {
                printf("Erreur de syntaxe : parenthèse fermante ')' manquante dans 'if'\n");
                return NULL;
            }

            // Bloc "then" : { ... }
            Token open_brace = get_next_token(file);
            if (strcmp(open_brace.value, "{") != 0)
            {
                printf("Erreur de syntaxe : accolade ouvrante '{' manquante dans 'if'\n");
                return NULL;
            }
            ASTNode *then_branch = parse(file);
            Token close_brace = get_next_token(file);
            if (strcmp(close_brace.value, "}") != 0)
            {
                printf("Erreur de syntaxe : accolade fermante '}' manquante dans 'if'\n");
                return NULL;
            }

            // Bloc "else" éventuel
            ASTNode *else_branch = NULL;
            Token next_token = get_next_token(file);
            if (next_token.type == TOKEN_KEYWORD && strcmp(next_token.value, "else") == 0)
            {
                Token open_brace_else = get_next_token(file);
                if (strcmp(open_brace_else.value, "{") != 0)
                {
                    printf("Erreur de syntaxe : accolade ouvrante '{' manquante dans 'else'\n");
                    return NULL;
                }
                else_branch = parse(file);
                Token close_brace_else = get_next_token(file);
                if (strcmp(close_brace_else.value, "}") != 0)
                {
                    printf("Erreur de syntaxe : accolade fermante '}' manquante dans 'else'\n");
                    return NULL;
                }
            }
            else
            {
                ungetc(next_token.value[0], file);
            }

            ASTNode *node = create_if_node(condition, then_branch, else_branch);
            if (!head)
            {
                head = node;
                current = node;
            }
            else
            {
                current->next = node;
                current = node;
            }
        }
        // --- Instruction return : "return expression;"
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "return") == 0)
        {
            ASTNode *expr = parse_comparison(file);
            Token semi = get_next_token(file);

            if (semi.type == TOKEN_SYMBOL && strcmp(semi.value, ";") == 0)
            {
                ASTNode *node = create_return_node(expr);
                if (!head)
                {
                    head = node;
                    current = node;
                }
                else
                {
                    current->next = node;
                    current = node;
                }
            }
        }
        // --- Définition de fonction : "fn nom(params) { ... }"
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "fn") == 0)
        {
            Token fn_name = get_next_token(file);
            if (fn_name.type != TOKEN_IDENTIFIER)
            {
                printf("Erreur de syntaxe : Nom de fonction attendu après 'fn'\n");
                return NULL;
            }

            Token open_paren = get_next_token(file);
            if (strcmp(open_paren.value, "(") != 0)
            {
                printf("Erreur de syntaxe : '(' attendu après %s\n", fn_name.value);
                return NULL;
            }

            ASTNode *param_head = NULL, *param_current = NULL;
            Token param = get_next_token(file);
            while (param.type == TOKEN_IDENTIFIER)
            {
                ASTNode *param_node = malloc(sizeof(ASTNode));
                param_node->type = NODE_DECLARATION;
                strcpy(param_node->variable, param.value);
                param_node->next = NULL;

                if (!param_head)
                {
                    param_head = param_node;
                    param_current = param_node;
                }
                else
                {
                    param_current->next = param_node;
                    param_current = param_node;
                }

                param = get_next_token(file);
                if (strcmp(param.value, ",") == 0)
                    param = get_next_token(file);
            }

            if (strcmp(param.value, ")") != 0)
            {
                printf("Erreur de syntaxe : ')' attendu\n");
                return NULL;
            }

            Token open_brace = get_next_token(file);
            if (strcmp(open_brace.value, "{") != 0)
            {
                printf("Erreur de syntaxe : '{' attendu après ')'\n");
                return NULL;
            }

            ASTNode *body = parse(file);
            Token close_brace = get_next_token(file);
            if (strcmp(close_brace.value, "}") != 0)
            {
                printf("Erreur de syntaxe : '}' manquant dans la fonction %s\n", fn_name.value);
                return NULL;
            }

            ASTNode *fn_node = create_function_node(fn_name.value, param_head, body);
            if (!head)
            {
                head = fn_node;
                current = fn_node;
            }
            else
            {
                current->next = fn_node;
                current = fn_node;
            }
        }
        // --- Fin d'un bloc (rencontre d'une accolade fermante)
        else if (token.type == TOKEN_SYMBOL && strcmp(token.value, "}") == 0)
        {
            ungetc(token.value[0], file);
            return head;
        }
    }

    return head;
}
