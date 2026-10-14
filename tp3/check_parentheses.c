// usage    : check_parentheses file
// fonction : exercice 7, question 1 : vérifie le parenthésage de file.
//            Message sur stderr en cas de problème (sans numéro de ligne
//            ni de colonne). Flux libc uniquement.
//   statut 0 : parenthésage correct
//   statut 1 : ')' en trop ou '(' non fermée
//   statut 2 : mauvais usage, file impossible à ouvrir, erreur de lecture
//
// compilation : gcc -Wall -Wextra -o check_parentheses check_parentheses.c

#include <stdio.h>      // FILE, fopen, fclose, fread, feof, fprintf
#include <string.h>     // strerror
#include <errno.h>      // errno

int main(int argc, char *argv[])
{
    // Il faut exactement un argument
    if (argc != 2) {
        fprintf(stderr, "%s: usage: %s file\n", argv[0], argv[0]);
        return 2;
    }
    // Ouvre le fichier en lecture
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for reading: %s\n",
                argv[0], argv[1], strerror(errno));
        return 2;
    }

    // (1) Compteur de parenthèses ouvertes non encore fermées
    int cpt = 0;
    // Caractère lu
    char c;

    // (2) Lit caractère par caractère (fread renvoie 1 si un caractère lu)
    while (fread(&c, 1, 1, f) == 1) {
        // Ouvrante : +1
        if (c == '(')
            cpt += 1;
        // Fermante : -1
        else if (c == ')')
            cpt -= 1;
        // (3) Négatif : cette ')' ne ferme aucune '('
        if (cpt < 0) {
            fprintf(stderr, "error: ')' is unexpected\n");
            fclose(f);
            return 1;
        }
    }

    // fread a renvoyé 0 : fin de fichier ou erreur ? (cours diapo 45)
    if (!feof(f)) {
        fprintf(stderr, "%s: Fatal: read error on '%s'\n", argv[0], argv[1]);
        fclose(f);
        return 2;
    }
    fclose(f);

    // (4) Positif à la fin : au moins une '(' n'a pas été fermée
    if (cpt > 0) {
        fprintf(stderr, "error: '(' is unmatched\n");
        return 1;
    }
    return 0;
}
