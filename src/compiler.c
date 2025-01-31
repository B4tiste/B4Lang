#include <string.h>
#include <ctype.h>
#include "compiler.h"

void generate_ir_from_ast(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_ASSIGNMENT:
        printf("MOV %s, ", node->variable);
        generate_ir_from_ast(node->left);
        printf("\n");
        break;

    case NODE_EXPRESSION:
        if (node->left && node->right)
        {
            generate_ir_from_ast(node->left);
            printf(" %s ", node->variable);
            generate_ir_from_ast(node->right);
        }
        else
        {
            printf("%s", node->variable);
        }
        break;

    case NODE_RETURN:
        printf("RETURN ");
        generate_ir_from_ast(node->left);
        printf("\n");
        break;

    default:
        break;
    }

    generate_ir_from_ast(node->next);
}

void generate_assembly_from_ast(FILE *file, ASTNode *node)
{
    if (!node)
        return;

    static int label_counter = 0; // Pour générer des labels uniques

    switch (node->type)
    {
    case NODE_DECLARATION:
        // ✅ Ne pas redéfinir la variable dans `.text`
        break;

    case NODE_ASSIGNMENT:
        fprintf(file, "    ; Affectation: %s\n", node->variable);
        generate_assembly_from_ast(file, node->left);
        fprintf(file, "    mov [%s], rax\n", node->variable);
        break;

    case NODE_EXPRESSION:
        if (node->left && node->right)
        {
            // ✅ Comparaison ou opération mathématique
            generate_assembly_from_ast(file, node->left);
            fprintf(file, "    push rax\n");
            generate_assembly_from_ast(file, node->right);
            fprintf(file, "    pop rbx\n");

            if (strcmp(node->variable, "+") == 0)
                fprintf(file, "    add rax, rbx\n");
            else if (strcmp(node->variable, "-") == 0)
                fprintf(file, "    sub rax, rbx\n");
            else if (strcmp(node->variable, "*") == 0)
                fprintf(file, "    imul rax, rbx\n");
            else if (strcmp(node->variable, "/") == 0)
            {
                fprintf(file, "    cqo\n"); // Extension de rax pour division
                fprintf(file, "    idiv rbx\n");
            }
            else if (strcmp(node->variable, ">") == 0)
            {
                fprintf(file, "    cmp rbx, rax\n"); // Vérifier `rbx > rax`
                fprintf(file, "    setg al\n");      // Mettre 1 si vrai
                fprintf(file, "    movzx rax, al\n");
            }
        }
        else
        {
            // ✅ Charger une valeur (nombre ou variable)
            if (isdigit(node->variable[0])) // Nombre
                fprintf(file, "    mov rax, %s\n", node->variable);
            else
                fprintf(file, "    mov rax, [%s]\n", node->variable);
        }
        break;

    case NODE_IF:
    {
        int label_if = label_counter++;
        int label_end = label_counter++;

        fprintf(file, "    ; Condition IF\n");
        generate_assembly_from_ast(file, node->condition);
        fprintf(file, "    cmp rax, 1\n");
        fprintf(file, "    jne else_%d\n", label_if);

        // ✅ Bloc IF
        fprintf(file, "if_%d:\n", label_if);
        generate_assembly_from_ast(file, node->then_branch);
        fprintf(file, "    jmp end_%d\n", label_end);

        // ✅ Bloc ELSE
        fprintf(file, "else_%d:\n", label_if);
        if (node->else_branch)
            generate_assembly_from_ast(file, node->else_branch);

        fprintf(file, "end_%d:\n", label_end);
        break;
    }

    case NODE_RETURN:
        fprintf(file, "    ; Return\n");
        generate_assembly_from_ast(file, node->left);
        fprintf(file, "    mov rsi, rax\n");
        fprintf(file, "    mov rdi, format\n");
        fprintf(file, "    mov rax, 0\n");
        fprintf(file, "    call printf\n");
        fprintf(file, "    mov rax, 60\n");
        fprintf(file, "    xor rdi, rdi\n");
        fprintf(file, "    syscall\n");
        break;

    default:
        fprintf(file, "    ; [INCONNU] Ignoré...\n");
        break;
    }

    generate_assembly_from_ast(file, node->next);
}

void save_assembly_to_file(ASTNode *ast, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Erreur ouverture fichier ASM");
        return;
    }

    // ✅ Ajout du format printf
    fprintf(file, "section .data\n");
    fprintf(file, "    format db \"%%d\", 10, 0\n"); // "%d\n"

    // ✅ Section `.bss` pour les variables non initialisées
    fprintf(file, "section .bss\n");

    // 🔹 Stocker les noms des variables pour éviter les doublons
    char declared_vars[100][MAX_TOKEN_LENGTH];
    int declared_count = 0;

    ASTNode *current = ast;
    while (current)
    {
        if (current->type == NODE_DECLARATION)
        {
            // Vérifier si la variable a déjà été déclarée
            bool already_declared = false;
            for (int i = 0; i < declared_count; i++)
            {
                if (strcmp(declared_vars[i], current->variable) == 0)
                {
                    already_declared = true;
                    break;
                }
            }

            // ✅ Ajouter la variable si elle n'a pas encore été déclarée
            if (!already_declared)
            {
                fprintf(file, "    %s resq 1\n", current->variable);
                strcpy(declared_vars[declared_count++], current->variable);
            }
        }
        current = current->next;
    }

    // ✅ Section `.text` (code exécutable)
    fprintf(file, "section .text\n");
    fprintf(file, "global _start\n");
    fprintf(file, "extern printf\n\n");

    fprintf(file, "_start:\n");
    generate_assembly_from_ast(file, ast);

    fclose(file);
    printf("Assembleur exporté dans '%s'\n", filename);
}
