// usage    : lire3c infile outfile
// fonction : 1er lire() sur les flux d'origine, 2e lire() sur infile et
//            outfile, puis ./lire de nouveau sur les flux d'origine.
//            Les flux 0 et 1 d'origine sont sauvegardés avec dup avant
//            les dup2, puis restaurés avant execl.
//   statut : celui de ./lire, ou 1 en cas d'erreur
//
// compilation : gcc -Wall -Wextra -o lire3c /pub/FISE_OSSE11/syscall/lire.c lire3c.c
// test        : { echo 111111111 ; echo 222222222 ; } | ./lire3c /pub/FISE_OSSE11/syscall/data.in data.out

#include <stdio.h>      // fprintf
#include <unistd.h>     // execl, dup, dup2, close
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
    // Ouvre argv[2] en écriture + création si besoin + troncature
    int fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fout == -1) {
        fprintf(stderr, "%s: cannot open '%s' for writing: %s\n",
                argv[0], argv[2], strerror(errno));
        return 1;
    }

    // 1er lire() : flux 0 et 1 d'origine
    lire();

    // Sauvegarde : dup renvoie un NOUVEAU descripteur (le 1er libre) qui
    // accède au même flux que 0 (resp. 1). Les flux d'origine restent
    // ainsi accessibles après les dup2 (cours diapo 30).
    int save0 = dup(0);
    int save1 = dup(1);

    // Redirige 0 sur argv[1] et 1 sur argv[2]
    dup2(fin, 0);
    dup2(fout, 1);
    // fin et fout ne servent plus
    close(fin);
    close(fout);

    // 2e lire() : sur les fichiers
    lire();

    // Restaure les flux d'origine dans 0 et 1 (dup2 ferme d'abord les
    // descripteurs sur les fichiers)
    dup2(save0, 0);
    dup2(save1, 1);
    // Les copies ne servent plus ; inutile de les laisser à ./lire
    close(save0);
    close(save1);

    // 3e lire() dans ./lire : sur les flux d'origine
    execl("./lire", "lire", (char *)NULL);

    // On n'arrive ici que si execl a échoué
    fprintf(stderr, "%s: execl failed: %s\n", argv[0], strerror(errno));
    return 1;
}
