#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 100 // Longueur maximale d'un token

// Liste des mots clés disponibles
extern const char *keywords[]; // ✅ Declare it as extern

// Tableau des strings des types de token
extern const char token_type_string[6][MAX_TOKEN_LENGTH];

// Définition des types
typedef enum
{
    TOKEN_INT,        // Nombre entier
    TOKEN_IDENTIFIER, // Identifiant (nom de variable)
    TOKEN_NUMBER,     // Nombre (ex: 42)
    TOKEN_SYMBOL,     // Opérateur, ponctuation (ex: =, +, ;)
    TOKEN_KEYWORD,    // Mot-clé (ex: int, return)
    TOKEN_EOF         // Fin du fichier
} TokenType;

typedef struct
{
    TokenType type;               // Type du token
    char value[MAX_TOKEN_LENGTH]; // Valeur du token
} Token;

bool is_keyword(const char *str);
Token get_next_token(FILE *file);
void print_token(Token token);

#endif