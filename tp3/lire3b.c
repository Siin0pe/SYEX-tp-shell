// usage    : lire3b infile outfile
// fonction : comme lire3a, mais le 1er lire() utilise les flux d'origine ;
//            les dup2 sont faits seulement après, donc le 2e lire() et
//            ./lire travaillent sur infile et outfile.
//   statut : celui de ./lire, ou 1 en cas d'erreur
//
// compilation : gcc -Wall -Wextra -o lire3b /pub/FISE_OSSE11/syscall/lire.c lire3b.c
// test        : echo 111111111 | ./lire3b /pub/FISE_OSSE11/syscall/data.in data.out

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
    // Ouvre argv[2] en écriture + création si besoin + troncature
    int fout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fout == -1) {
        fprintf(stderr, "%s: cannot open '%s' for writing: %s\n",
                argv[0], argv[2], strerror(errno));
        return 1;
    }

    // 1er lire() : flux 0 et 1 d'origine (stdin et stdout du shell)
    lire();

    // Redirige ensuite 0 sur argv[1] et 1 sur argv[2]
    dup2(fin, 0);
    dup2(fout, 1);
    // fin et fout ne servent plus
    close(fin);
    close(fout);

    // 2e lire() : sur les fichiers
    lire();
    // 3e lire() dans ./lire : hérite des descripteurs redirigés
    execl("./lire", "lire", (char *)NULL);

    // On n'arrive ici que si execl a échoué
    fprintf(stderr, "%s: execl failed: %s\n", argv[0], strerror(errno));
    return 1;
}
