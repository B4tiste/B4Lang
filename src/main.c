#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "compiler.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <fichier.b4l>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_path = argv[1];
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    printf("Lancement du LEXEUR et du PARSER...\n");

    // Analyse syntaxique
    ASTNode *ast = parse(file);
    fclose(file); // Plus besoin du fichier après parsing

    if (ast)
    {
        // ✅ Génération du fichier DOT pour la visualisation de l'AST
        print_ast_tree(ast, 0, 0);
        save_ast_to_dot(ast, "ast.dot");
        system("sh /home/batiste/Documents/B4lang/convert_dot_to_png.sh");

        // ✅ Génération du fichier assembleur
        printf("\nGénération du code assembleur...\n");
        char asm_file[256];
        snprintf(asm_file, sizeof(asm_file), "%s.s", file_path);
        save_assembly_to_file(ast, asm_file);

        // ✅ Compilation et création de l'exécutable
        printf("\nCompilation du programme...\n");
        char obj_file[256], exe_file[256];
        snprintf(obj_file, sizeof(obj_file), "%s.o", file_path);
        snprintf(exe_file, sizeof(exe_file), "%s.out", file_path);

        char command[1024];
        snprintf(command, sizeof(command), "nasm -f elf64 %s -o %s", asm_file, obj_file);
        system(command);

        snprintf(command, sizeof(command), "ld %s -o %s -lc --dynamic-linker /lib64/ld-linux-x86-64.so.2", obj_file, exe_file);
        system(command);

        printf("\nCompilation terminée. Exécutez './%s' pour lancer votre programme.\n", exe_file);

        // ✅ Libération de la mémoire
        free_ast(ast);
    }
    else
    {
        printf("Erreur dans l'analyse du programme\n");
    }

    return EXIT_SUCCESS;
}