// usage    : delLastByte-libc f
// fonction : comme delLastByte, mais uniquement avec des flux libc
//            (fopen, fseek, ftell, fread, fwrite, fclose).
//   statut 0 : succès
//   statut 1 : erreur (message précis sur stderr)
//
// fseek et ftell ne sont pas dans le cours : c'est le sujet qui les
// impose. Ce sont les équivalents libc de lseek (voir man 3 fseek).
//
// compilation : gcc -Wall -Wextra -o delLastByte-libc delLastByte-libc.c

#include <stdio.h>      // FILE, fopen, fseek, ftell, fread, fwrite, fclose
#include <stdlib.h>     // malloc, free
#include <string.h>     // strerror
#include <errno.h>      // errno (fopen le met à jour en cas d'échec)

int main(int argc, char *argv[])
{
    // Il faut exactement un argument
    if (argc != 2) {
        fprintf(stderr, "%s: usage: %s f\n", argv[0], argv[0]);
        return 1;
    }
    // Nom significatif pour le chemin
    char *f = argv[1];

    // Vérifie lecture ET écriture sans rien modifier : le mode "r+"
    // ouvre en lecture/écriture, sans troncature et sans création
    // (cours diapo 42). Échoue si f n'existe pas, si un des droits
    // manque, ou si f est un répertoire.
    FILE *fp = fopen(f, "r+");
    if (fp == NULL) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for reading and writing: %s\n",
                argv[0], f, strerror(errno));
        return 1;
    }
    // Ce flux ne servait qu'au test
    fclose(fp);

    // a. Ouvre f en lecture seule ("r")
    fp = fopen(f, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for reading: %s\n",
                argv[0], f, strerror(errno));
        return 1;
    }

    // b. Taille : fseek place le curseur à la fin (renvoie 0 si ok),
    //    puis ftell renvoie la position courante = la taille t.
    //    fseek échoue si f n'est pas un fichier régulier (ex : FIFO).
    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "%s: Fatal: '%s' is not a regular file: %s\n",
                argv[0], f, strerror(errno));
        fclose(fp);
        return 1;
    }
    long t = ftell(fp);
    if (t == -1) {
        fprintf(stderr, "%s: Fatal: ftell failed on '%s': %s\n",
                argv[0], f, strerror(errno));
        fclose(fp);
        return 1;
    }
    // Fichier vide : il n'y a pas de dernier octet à enlever
    if (t == 0) {
        fprintf(stderr, "%s: Fatal: '%s' is empty\n", argv[0], f);
        fclose(fp);
        return 1;
    }
    // Ramène le curseur au début pour la lecture
    fseek(fp, 0, SEEK_SET);

    // c. Alloue un tableau de t-1 octets (+1 pour éviter malloc(0))
    char *tab = malloc(t);
    if (tab == NULL) {
        fprintf(stderr, "%s: Fatal: out of memory\n", argv[0]);
        fclose(fp);
        return 1;
    }

    // d. Lit d'un bloc 1 élément de t-1 octets.
    //    fread renvoie le nombre d'ÉLÉMENTS lus : 1 si tout est lu,
    //    0 si fin de fichier ou erreur (cours diapo 44)
    if (t - 1 > 0 && fread(tab, t - 1, 1, fp) != 1) {
        fprintf(stderr, "%s: Fatal: cannot read '%s'\n", argv[0], f);
        free(tab);
        fclose(fp);
        return 1;
    }

    // e. Ferme f, puis le rouvre en écriture seule + troncature ("w")
    fclose(fp);
    fp = fopen(f, "w");
    if (fp == NULL) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for writing: %s\n",
                argv[0], f, strerror(errno));
        free(tab);
        return 1;
    }

    // f. Réécrit les t-1 octets (fwrite renvoie aussi un nb d'éléments)
    if (t - 1 > 0 && fwrite(tab, t - 1, 1, fp) != 1) {
        fprintf(stderr, "%s: Fatal: cannot write '%s'\n", argv[0], f);
        free(tab);
        fclose(fp);
        return 1;
    }

    // g. Ferme (fclose écrit le tampon libc avec write) et libère.
    //    fclose renvoie 0 si pas d'erreur : une erreur d'écriture du
    //    tampon (disque plein...) n'apparaît qu'ici.
    free(tab);
    if (fclose(fp) != 0) {
        fprintf(stderr, "%s: Fatal: cannot write '%s': %s\n",
                argv[0], f, strerror(errno));
        return 1;
    }
    return 0;
}
