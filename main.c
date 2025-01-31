#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

int main(void)
{
    FILE *file = fopen("/home/batiste/Documents/B4lang/test.b4l", "r");
    if (!file)
    {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    printf("Lancement du LEXEUR et du PARSER...\n");
    // Analyse syntaxique
    ASTNode *ast = parse(file);
    if (ast)
    {
        print_ast(ast);
        free_ast(ast);
    }
    else
    {
        printf("Erreur dans l'analyse du programme\n");
    }

    fclose(file);
    return EXIT_SUCCESS;
}
