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

    // const int sz = 255;

    // char str[sz];

    // while (fgets(str, sz, file) != NULL)
    // {
    //     printf("%s", str);
    // };

    printf("Lancement du LEXEUR et du PARSER...\n");
    // Analyse syntaxique
    ASTNode *ast = parse(file);
    if (ast)
    {
        // print_ast(ast);
        printf("\n=== AST sous forme d'arbre ===\n");
        print_ast_tree(ast, 0, 0); // ✅ Nouvelle version avec des traits de liaison
        save_ast_to_dot(ast, "ast.dot");
        free_ast(ast);
    }
    else
    {
        printf("Erreur dans l'analyse du programme\n");
    }

    fclose(file);
    return EXIT_SUCCESS;
}
