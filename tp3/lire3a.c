// usage    : lire3a infile outfile
// fonction : comme lire3exec, mais les 3 appels à lire() lisent infile
//            et écrivent dans outfile : le flux 0 est redirigé sur infile
//            et le flux 1 sur outfile avec dup2.
//   statut : celui de ./lire, ou 1 en cas d'erreur
//
// compilation : gcc -Wall -Wextra -o lire3a /pub/FISE_OSSE11/syscall/lire.c lire3a.c
// test        : ./lire3a /pub/FISE_OSSE11/syscall/data.in data.out

#include <stdio.h>      // fprintf
#include <unistd.h>     // execl, dup2, close
#include <fcntl.h>      // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h>   // S_IRUSR, S_IWUSR
#include <string.h>     // strerror
#include <errno.h>      // errno
#include "/pub/FISE_OSSE11/syscall/lire.h"

int main(int argc, char *argv[])
{
    // Il faut exactement 2 arguments
    if (argc != 3) {
        fprintf(stderr, "%s: usage: %s infile outfile\n", argv[0], argv[0]);
        return 1;
    }

    // Ouvre argv[1] en lecture
    int fin = open(argv[1], O_RDONLY);
    if (fin == -1) {
        fprintf(stderr, "%s: cannot open '%s' for reading: %s\n",
                argv[0], argv[1], strerror(errno));
        return 1;
    }
    // Ouvre argv[2] en écriture + création si besoin + troncature.
    // 3e argument : droits du fichier s'il est créé (rw pour le propriétaire)
    int fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fout == -1) {
        fprintf(stderr, "%s: cannot open '%s' for writing: %s\n",
                argv[0], argv[2], strerror(errno));
        return 1;
    }

    // Le descripteur 0 accède maintenant au même flux que fin : le
    // read(0, ...) de lire() lit argv[1] (cours diapo 30)
    dup2(fin, 0);
    // Le descripteur 1 accède au même flux que fout : write(1, ...) écrit
    // dans argv[2]
    dup2(fout, 1);
    // fin et fout ne servent plus (les flux restent ouverts via 0 et 1)
    close(fin);
    close(fout);

    // Les 3 lire() travaillent sur les fichiers
    lire();
    lire();
    // Le programme lancé hérite des descripteurs 0 et 1 redirigés
    execl("./lire", "lire", (char *)NULL);

    // On n'arrive ici que si execl a échoué
    fprintf(stderr, "%s: execl failed: %s\n", argv[0], strerror(errno));
    return 1;
}
