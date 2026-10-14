// usage    : numbering-file f
// fonction : comme numbering, mais lit le fichier f au lieu de stdin
//            (exercice 3, question 2). Flux libc uniquement.
//   statut 0 : succès
//   statut 1 : mauvais usage, f impossible à ouvrir, ou erreur de lecture
//
// Différences avec numbering.c (marquées "Q2" ci-dessous) :
//   1. main prend argc/argv et vérifie qu'il y a un argument ;
//   2. ouverture de argv[1] avec fopen(..., "r") et test de NULL ;
//   3. fread et feof utilisent ce FILE* au lieu de stdin ;
//   4. fermeture avec fclose à la fin.
//
// compilation : gcc -Wall -Wextra -o numbering-file numbering-file.c

#include <stdio.h>      // FILE, fopen, fclose, fread, printf, feof
#include <string.h>     // strerror
#include <errno.h>      // errno

// Q2 (1) : main reçoit les arguments
int main(int argc, char *argv[])
{
    // Q2 (1) : il faut exactement un argument
    if (argc != 2) {
        fprintf(stderr, "%s: usage: %s file\n", argv[0], argv[0]);
        return 1;
    }
    // Q2 (2) : ouvre le fichier en lecture
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for reading: %s\n",
                argv[0], argv[1], strerror(errno));
        return 1;
    }

    // Numéro de la ligne courante
    int num = 1;
    // Vrai (1) si le prochain caractère est le premier d'une ligne
    int debut = 1;
    // Caractère lu
    char c;

    // Q2 (3) : lit dans f au lieu de stdin
    while (fread(&c, 1, 1, f) == 1) {
        // Premier caractère d'une ligne : écrit d'abord "num<TAB>"
        if (debut) {
            printf("%d\t", num);
            debut = 0;
        }
        // Recopie le caractère
        printf("%c", c);
        // Fin de ligne : la ligne suivante commence au prochain caractère
        if (c == '\n') {
            num += 1;
            debut = 1;
        }
    }

    // Q2 (3) : feof sur f
    if (!feof(f)) {
        fprintf(stderr, "%s: Fatal: read error on '%s'\n", argv[0], argv[1]);
        fclose(f);
        return 1;
    }
    // Q2 (4) : ferme le fichier
    fclose(f);
    return 0;
}
