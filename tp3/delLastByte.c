// usage    : delLastByte f
// fonction : enlève le dernier octet du fichier régulier f.
//            Uniquement des flux noyau (open, lseek, read, write, close).
//   statut 0 : succès
//   statut 1 : erreur (message précis sur stderr)
//
// Les vérifications sur f (existe, lisible, inscriptible, régulier) se
// font avec les échecs des appels système eux-mêmes : en cas d'échec ils
// renvoient -1 et errno dit pourquoi (cours diapos 17-18).
//
// compilation : gcc -Wall -Wextra -o delLastByte delLastByte.c

#include <stdio.h>      // fprintf (messages d'erreur sur stderr)
#include <stdlib.h>     // malloc, free
#include <string.h>     // strerror
#include <errno.h>      // errno
#include <unistd.h>     // read, write, close, lseek
#include <fcntl.h>      // open, O_RDONLY, O_WRONLY, O_TRUNC

int main(int argc, char *argv[])
{
    // Il faut exactement un argument
    if (argc != 2) {
        // argv[0] = nom du programme
        fprintf(stderr, "%s: usage: %s f\n", argv[0], argv[0]);
        return 1;
    }
    // Nom significatif pour le chemin
    char *f = argv[1];

    // Vérifie que f est inscriptible AVANT de le lire : on ouvre en
    // écriture seule SANS O_TRUNC (le fichier n'est pas modifié).
    //   errno = ENOENT : No such file or directory
    //           EACCES : Permission denied (pas le droit w)
    //           EISDIR : Is a directory
    int fd = open(f, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for writing: %s\n",
                argv[0], f, strerror(errno));
        return 1;
    }
    // Ce descripteur ne servait qu'au test
    close(fd);

    // a. Ouvre f en lecture seule (échoue si pas le droit r)
    fd = open(f, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for reading: %s\n",
                argv[0], f, strerror(errno));
        return 1;
    }

    // b. Taille : lseek place le curseur 0 octet après la fin (SEEK_END)
    //    et renvoie la nouvelle position, c'est-à-dire la taille t.
    //    Échoue (ESPIPE : Illegal seek) si f n'est pas un fichier
    //    régulier, par exemple une FIFO (cours diapo 21).
    off_t t = lseek(fd, 0, SEEK_END);
    if (t == -1) {
        fprintf(stderr, "%s: Fatal: '%s' is not a regular file: %s\n",
                argv[0], f, strerror(errno));
        close(fd);
        return 1;
    }
    // Fichier vide : il n'y a pas de dernier octet à enlever
    if (t == 0) {
        fprintf(stderr, "%s: Fatal: '%s' is empty\n", argv[0], f);
        close(fd);
        return 1;
    }
    // Ramène le curseur au début pour la lecture
    lseek(fd, 0, SEEK_SET);

    // c. Alloue un tableau de t-1 octets (+1 pour que malloc ne reçoive
    //    jamais 0 quand t vaut 1)
    char *tab = malloc(t);
    if (tab == NULL) {
        fprintf(stderr, "%s: Fatal: out of memory\n", argv[0]);
        close(fd);
        return 1;
    }

    // d. Lit d'un seul bloc les t-1 premiers octets.
    //    read renvoie le nombre d'octets lus, ou -1 en cas d'erreur
    int nbl = read(fd, tab, t - 1);
    if (nbl == -1) {
        fprintf(stderr, "%s: Fatal: cannot read '%s': %s\n",
                argv[0], f, strerror(errno));
        free(tab);
        close(fd);
        return 1;
    }
    // Lecture incomplète : on ne vide surtout pas le fichier
    if (nbl != t - 1) {
        fprintf(stderr, "%s: Fatal: short read on '%s'\n", argv[0], f);
        free(tab);
        close(fd);
        return 1;
    }

    // e. Ferme f, puis le rouvre en écriture seule + troncature (vidé)
    close(fd);
    fd = open(f, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        fprintf(stderr, "%s: Fatal: cannot open '%s' for writing: %s\n",
                argv[0], f, strerror(errno));
        free(tab);
        return 1;
    }

    // f. Réécrit les t-1 octets conservés
    int nbe = write(fd, tab, t - 1);
    if (nbe == -1 || nbe != t - 1) {
        fprintf(stderr, "%s: Fatal: cannot write '%s': %s\n",
                argv[0], f, strerror(errno));
        free(tab);
        close(fd);
        return 1;
    }

    // g. Ferme le fichier et libère la mémoire
    close(fd);
    free(tab);
    return 0;
}
