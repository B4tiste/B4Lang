#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Liste des mots clés disponibles
const char *keywords[] = {"int", "return", "print", NULL};  // ✅ Define it here only

bool is_keyword(const char *str)
{
    for (int i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return true;
        }
    }

    return false;
}

// Fonction qui lit le prochain token dans un fichier
Token get_next_token(FILE *file)
{
    int c;
    Token token;

    // Ignorer les espaces et \n
    while ((c = fgetc(file)) != EOF && isspace(c));

    // Token de fin de fichier
    if (c == EOF)
    {
        token.type = TOKEN_EOF;
        strcpy(token.value, "EOF");
        return token;
    }

    // Token keyword ou identifiant (int/return VS var_a/var_b)
    if (isalpha(c) || c =='_')
    {
        char buffer[MAX_TOKEN_LENGTH] = {c};
        int i = 1;
        while ((c = fgetc(file)) != EOF && (isalnum(c) || c == '_'))
        {
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        ungetc(c, file); // Remettre le prochain char dans le flux

        token.type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        strcpy(token.value, buffer);
        return token;
    }

    // Token nombre
    if (isdigit(c))
    {
        char buffer[MAX_TOKEN_LENGTH] = {c};
        int i = 1;
        while ((c = fgetc(file)) != EOF && isdigit(c))
        {
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        ungetc(c, file); // Remettre le prochain char dans le flux

        token.type = TOKEN_NUMBER;
        strcpy(token.value, buffer);
        return token;
    }

    // Token symbole
    token.type = TOKEN_SYMBOL;
    token.value[0] = c;
    token.value[1] = '\0';

    return token;
}