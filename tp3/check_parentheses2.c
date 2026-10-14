// usage    : check_parentheses2 file
//            check_parentheses2 -        (lit l'entrée standard)
// fonction : exercice 7, questions 2 à 4 : comme check_parentheses, mais
//            les messages donnent la ligne et la colonne de la
//            parenthèse fautive. Flux libc uniquement.
//   statut 0 : parenthésage correct
//   statut 1 : ')' en trop ou '(' non fermée
//   statut 2 : mauvais usage, file impossible à ouvrir, erreur de lecture
//
// Ajouts par rapport à check_parentheses.c, repérés par Q2 / Q3 / Q4.
//
// compilation : gcc -Wall -Wextra -o check_parentheses2 check_parentheses2.c

#include <stdio.h>      // FILE, fopen, fclose, fread, feof, fprintf, stdin
#include <stdlib.h>     // malloc, free
#include <string.h>     // strcmp, strerror
#include <errno.h>      // errno

int main(int argc, char *argv[])
{
    // Il faut exactement un argument
    if (argc != 2) {
        fprintf(stderr, "%s: usage: %s file\n", argv[0], argv[0]);
        return 2;
    }

    // Q3 : "-" désigne l'entrée standard, déjà ouverte : pas de fopen
    FILE *f;
    if (strcmp(argv[1], "-") == 0) {
        f = stdin;
    } else {
        // Ouvre le fichier en lecture
        f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "%s: Fatal: cannot open '%s' for reading: %s\n",
                    argv[0], argv[1], strerror(errno));
            return 2;
        }
    }

    // Q2 : position du caractère courant (ligne et colonne, à partir de 1)
    int ligne = 1;
    int col = 0;

    // Q4 : pile des positions des '(' non encore fermées.
    // pl[i] et pc[i] = ligne et colonne de la '(' de profondeur i.
    // Le compteur cpt est la hauteur de la pile ; taille = place allouée.
    int taille = 16;
    int *pl = malloc(taille * sizeof(int));
    int *pc = malloc(taille * sizeof(int));
    if (pl == NULL || pc == NULL) {
        fprintf(stderr, "%s: Fatal: out of memory\n", argv[0]);
        return 2;
    }

    // (1) Compteur de parenthèses ouvertes non encore fermées
    int cpt = 0;
    // Caractère lu
    char c;

    // (2) Lit caractère par caractère
    while (fread(&c, 1, 1, f) == 1) {
        // Q2 : met à jour la position AVANT de traiter le caractère
        col += 1;

        if (c == '(') {
            // Q4 : pile pleine ? on alloue 2 fois plus grand, on recopie,
            //      on libère l'ancienne zone
            if (cpt == taille) {
                int *nl = malloc(2 * taille * sizeof(int));
                int *nc = malloc(2 * taille * sizeof(int));
                if (nl == NULL || nc == NULL) {
                    fprintf(stderr, "%s: Fatal: out of memory\n", argv[0]);
                    return 2;
                }
                for (int i = 0; i < taille; i++) {
                    nl[i] = pl[i];
                    nc[i] = pc[i];
                }
                free(pl);
                free(pc);
                pl = nl;
                pc = nc;
                taille = 2 * taille;
            }
            // Q4 : empile la position de cette '('
            pl[cpt] = ligne;
            pc[cpt] = col;
            // Ouvrante : +1
            cpt += 1;
        } else if (c == ')') {
            // Fermante : -1 (Q4 : dépile, la '(' du sommet est fermée)
            cpt -= 1;
            // (3) Négatif : Q2, on connaît la position de cette ')'
            if (cpt < 0) {
                fprintf(stderr, "error: line %d, column %d: ')' is unexpected\n",
                        ligne, col);
                free(pl);
                free(pc);
                // Q3 : on ne ferme pas stdin, on ne l'a pas ouvert
                if (f != stdin)
                    fclose(f);
                return 1;
            }
        } else if (c == '\n') {
            // Q2 : le caractère suivant est en colonne 1 de la ligne suivante
            ligne += 1;
            col = 0;
        }
    }

    // fread a renvoyé 0 : fin de fichier ou erreur ?
    if (!feof(f)) {
        fprintf(stderr, "%s: Fatal: read error on '%s'\n", argv[0], argv[1]);
        free(pl);
        free(pc);
        if (f != stdin)
            fclose(f);
        return 2;
    }
    // Q3 : on ne ferme pas stdin
    if (f != stdin)
        fclose(f);

    // (4) Positif à la fin : Q4, les cpt '(' encore dans la pile ne sont
    //     pas fermées ; on signale la dernière ouverte (sommet de la pile)
    if (cpt > 0) {
        fprintf(stderr, "error: line %d, column %d: '(' is unmatched\n",
                pl[cpt - 1], pc[cpt - 1]);
        free(pl);
        free(pc);
        return 1;
    }
    free(pl);
    free(pc);
    return 0;
}
