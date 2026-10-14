// usage    : delLastByte-v2 f
// fonction : comme delLastByte, mais la taille est obtenue avec fstat
//            (demandé par le sujet) au lieu de lseek.
//            Uniquement des flux noyau (open, fstat, read, write, close).
//   statut 0 : succès
//   statut 1 : erreur (message précis sur stderr)
//
// Les vérifications sur f (existe, lisible, inscriptible, régulier) se
// font avec les échecs des appels système eux-mêmes : en cas d'échec ils
// renvoient -1 et errno dit pourquoi (cours diapos 17-18).
//
// compilation : gcc -Wall -Wextra -o delLastByte-v2 delLastByte-v2.c

#include <stdio.h>      // fprintf (messages d'erreur sur stderr)
#include <stdlib.h>     // malloc, free
#include <string.h>     // strerror
#include <errno.h>      // errno
#include <unistd.h>     // read, write, close
#include <fcntl.h>      // open, O_RDONLY, O_WRONLY, O_TRUNC
#include <sys/stat.h>   // fstat, struct stat, S_ISREG

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

    // b. Taille avec fstat. Comme scanf, on lui passe l'ADRESSE d'une
    //    variable (&st) qu'il remplit (voir man 2 fstat, EXAMPLES).
    struct stat st;
    if (fstat(fd, &st) == -1) {
        fprintf(stderr, "%s: Fatal: fstat failed on '%s': %s\n",
                argv[0], f, strerror(errno));
        close(fd);
        return 1;
    }
    // st.st_mode donne aussi le type : S_ISREG vrai si fichier régulier
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s: Fatal: '%s' is not a regular file\n", argv[0], f);
        close(fd);
        return 1;
    }
    // La taille en octets
    off_t t = st.st_size;
    // Fichier vide : il n'y a pas de dernier octet à enlever
    if (t == 0) {
        fprintf(stderr, "%s: Fatal: '%s' is empty\n", argv[0], f);
        close(fd);
        return 1;
    }
    // Pas de lseek : le curseur est toujours au début

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
