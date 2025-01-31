#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Analyse une expression récursivement (3 + y * 2)
ASTNode *parse_expression(FILE *file)
{
    Token token = get_next_token(file);
    ASTNode *left = NULL;

    // ✅ Si c'est un nombre ou une variable, on crée un nœud
    if (token.type == TOKEN_NUMBER || token.type == TOKEN_IDENTIFIER)
    {
        left = malloc(sizeof(ASTNode));
        left->type = NODE_EXPRESSION;
        strcpy(left->variable, token.value);
        left->left = left->right = NULL;
    }
    // ✅ Si c'est une parenthèse ouvrante, traiter l'expression entre parenthèses
    else if (token.type == TOKEN_SYMBOL && strcmp(token.value, "(") == 0)
    {
        left = parse_expression(file);
        Token closing = get_next_token(file);
        if (closing.type != TOKEN_SYMBOL || strcmp(closing.value, ")") != 0)
        {
            printf("Erreur de syntaxe : Parenthèse fermante `)` manquante\n");
            return NULL;
        }
    }
    else
    {
        return NULL; // ❌ Si ce n'est ni une variable, ni un nombre, ni une parenthèse
    }

    // ✅ Lire un opérateur (ex: +, -, *, /, >, <, ==, !=)
    token = get_next_token(file);
    if (token.type == TOKEN_SYMBOL &&
        (strcmp(token.value, "+") == 0 || strcmp(token.value, "-") == 0 ||
         strcmp(token.value, "*") == 0 || strcmp(token.value, "/") == 0 ||
         strcmp(token.value, ">") == 0 || strcmp(token.value, "<") == 0 ||
         strcmp(token.value, "==") == 0 || strcmp(token.value, "!=") == 0))
    {
        ASTNode *right = parse_expression(file); // ✅ Lire la partie droite de l'expression
        return create_expression_node(token.value, left, right);
    }

    // ✅ Si ce n'est pas un opérateur, remettre le token dans le flux
    ungetc(token.value[0], file);
    return left;
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
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "if") == 0)
        {
            // Lire '('
            Token open_paren = get_next_token(file);
            if (strcmp(open_paren.value, "(") != 0)
            {
                printf("Erreur de syntaxe : `if` sans parenthèse ouvrante `(` \n");
                return NULL;
            }

            // Lire toute la condition (ex: x > 4)
            ASTNode *condition = parse_expression(file);

            // Lire ')'
            Token close_paren = get_next_token(file);
            if (strcmp(close_paren.value, ")") != 0)
            {
                printf("Erreur de syntaxe : parenthèse fermante `)` manquante dans `if`\n");
                return NULL;
            }

            // Lire `{`
            Token open_brace = get_next_token(file);
            if (strcmp(open_brace.value, "{") != 0)
            {
                printf("Erreur de syntaxe : accolade ouvrante `{` manquante dans `if`\n");
                return NULL;
            }

            // Lire le bloc `if`
            ASTNode *then_branch = parse(file);

            // Lire `}`
            Token close_brace = get_next_token(file);
            if (strcmp(close_brace.value, "}") != 0)
            {
                printf("Erreur de syntaxe : accolade fermante `}` manquante dans `if`\n");
                return NULL;
            }

            // Vérifier s'il y a un `else`
            ASTNode *else_branch = NULL;
            Token next_token = get_next_token(file);
            if (next_token.type == TOKEN_KEYWORD && strcmp(next_token.value, "else") == 0)
            {
                // Lire `{`
                Token open_brace_else = get_next_token(file);
                if (strcmp(open_brace_else.value, "{") != 0)
                {
                    printf("Erreur de syntaxe : accolade ouvrante `{` manquante dans `else`\n");
                    return NULL;
                }

                // Lire le bloc `else`
                else_branch = parse(file);

                // Lire `}`
                Token close_brace_else = get_next_token(file);
                if (strcmp(close_brace_else.value, "}") != 0)
                {
                    printf("Erreur de syntaxe : accolade fermante `}` manquante dans `else`\n");
                    return NULL;
                }
            }
            else
            {
                // Si ce n'est pas `else`, remettre le token dans le flux
                ungetc(next_token.value[0], file);
            }

            // Créer le nœud `if`
            ASTNode *node = create_if_node(condition, then_branch, else_branch);
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
        else if (token.type == TOKEN_SYMBOL && strcmp(token.value, "}") == 0)
        {
            // ✅ Arrêter le parsing du bloc en remettant `}` dans le flux
            ungetc(token.value[0], file);
            return head;
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
        else if (token.type == TOKEN_KEYWORD && strcmp(token.value, "fn") == 0)
        {
            // Lire le nom de la fonction
            Token fn_name = get_next_token(file);
            if (fn_name.type != TOKEN_IDENTIFIER)
            {
                printf("Erreur de syntaxe : Nom de fonction attendu après `fn`\n");
                return NULL;
            }

            // Lire `(` pour les paramètres
            Token open_paren = get_next_token(file);
            if (strcmp(open_paren.value, "(") != 0)
            {
                printf("Erreur de syntaxe : `(` attendu après `%s`\n", fn_name.value);
                return NULL;
            }

            // Lire les paramètres
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
                }
                else
                {
                    param_current->next = param_node;
                }
                param_current = param_node;

                param = get_next_token(file);
                if (strcmp(param.value, ",") == 0)
                {
                    param = get_next_token(file);
                }
            }

            // Lire `)` et `{`
            if (strcmp(param.value, ")") != 0)
            {
                printf("Erreur de syntaxe : `)` attendu\n");
                return NULL;
            }
            Token open_brace = get_next_token(file);
            if (strcmp(open_brace.value, "{") != 0)
            {
                printf("Erreur de syntaxe : `{` attendu après `)`\n");
                return NULL;
            }

            // Lire le corps de la fonction
            ASTNode *body = parse(file);
            Token close_brace = get_next_token(file);
            if (strcmp(close_brace.value, "}") != 0)
            {
                printf("Erreur de syntaxe : `}` manquant dans la fonction `%s`\n", fn_name.value);
                return NULL;
            }

            // Créer et ajouter la fonction à l'AST
            ASTNode *fn_node = create_function_node(fn_name.value, param_head, body);
            if (!head)
            {
                head = fn_node;
            }
            else
            {
                current->next = fn_node;
            }
            current = fn_node;
        }
        else if (token.type == TOKEN_IDENTIFIER)
        {
            // Vérifier si c'est un appel de fonction
            Token next = get_next_token(file);
            if (next.type == TOKEN_SYMBOL && strcmp(next.value, "(") == 0)
            {
                // C'est un appel de fonction
                ASTNode *args_head = NULL, *args_current = NULL;
                Token arg = get_next_token(file);

                while (arg.type == TOKEN_IDENTIFIER || arg.type == TOKEN_NUMBER)
                {
                    ASTNode *arg_node = malloc(sizeof(ASTNode));
                    arg_node->type = NODE_EXPRESSION;
                    strcpy(arg_node->variable, arg.value);
                    arg_node->next = NULL;

                    if (!args_head)
                    {
                        args_head = arg_node;
                    }
                    else
                    {
                        args_current->next = arg_node;
                    }
                    args_current = arg_node;

                    arg = get_next_token(file);
                    if (strcmp(arg.value, ",") == 0)
                    {
                        arg = get_next_token(file);
                    }
                }

                if (strcmp(arg.value, ")") != 0)
                {
                    printf("Erreur de syntaxe : `)` attendu après appel de fonction `%s`\n", token.value);
                    return NULL;
                }

                ASTNode *fn_call = create_function_call_node(token.value, args_head);
                Token semi = get_next_token(file);
                if (semi.type != TOKEN_SYMBOL || strcmp(semi.value, ";") != 0)
                {
                    printf("Erreur de syntaxe : `;` attendu après appel de fonction `%s`\n", token.value);
                    return NULL;
                }

                if (!head)
                {
                    head = fn_call;
                    current = fn_call;
                }
                else
                {
                    current->next = fn_call;
                    current = fn_call;
                }
            }
            else
            {
                ungetc(next.value[0], file); // Remettre le token si ce n'est pas un appel de fonction
            }
        }
        else if (token.type == TOKEN_IDENTIFIER) // ✅ Détection d'une affectation
        {
            printf("PARSE DEBUG: Détection d'un identifiant -> %s\n", token.value); // Debug

            char var[MAX_TOKEN_LENGTH];
            strcpy(var, token.value);

            Token eq = get_next_token(file);
            if (eq.type == TOKEN_SYMBOL && strcmp(eq.value, "=") == 0) // ✅ Vérifier que c'est une affectation
            {
                printf("PARSE DEBUG: Affectation détectée pour -> %s\n", var); // Debug

                ASTNode *expr = parse_expression(file); // ✅ Lire l'expression
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
                        current->next = node; // ✅ Ajoute à l'AST
                        current = node;
                    }

                    printf("DEBUG: Affectation ajoutée -> %s\n", var);
                }
                else
                {
                    printf("Erreur de syntaxe : `;` manquant après l'affectation de `%s`\n", var);
                }
            }
            else
            {
                printf("Erreur de syntaxe : Symbole `=` attendu après `%s`\n", var);
            }
        }
    }

    return head;
}