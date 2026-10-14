// usage    : numbering
// fonction : recopie stdin sur stdout en faisant précéder chaque ligne
//            de son numéro et d'une tabulation. Flux libc uniquement.
//   statut 0 : succès
//   statut 1 : erreur de lecture
//
// compilation : gcc -Wall -Wextra -o numbering numbering.c
// test        : { echo 'first line' ; echo 'second line' ; } | ./numbering

#include <stdio.h>      // fread, printf, feof, stdin, stderr

int main()
{
    // Numéro de la ligne courante
    int num = 1;
    // Vrai (1) si le prochain caractère est le premier d'une ligne
    int debut = 1;
    // Caractère lu
    char c;

    // fread lit 1 élément de 1 octet ; il renvoie 1 si un caractère
    // a été lu, 0 en fin de fichier ou en cas d'erreur (diapo 44)
    while (fread(&c, 1, 1, stdin) == 1) {
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

    // fread a renvoyé 0 : feof distingue la fin de fichier d'une erreur
    if (!feof(stdin)) {
        fprintf(stderr, "numbering: Fatal: read error\n");
        return 1;
    }
    return 0;
}
