// usage    : lire3exec
// fonction : appelle lire() deux fois, puis se remplace par le programme
//            ./lire (qui fait un 3e lire()).
//   statut : celui de ./lire, ou 1 si execl échoue
//
// compilation : gcc -Wall -Wextra -o lire3exec /pub/FISE_OSSE11/syscall/lire.c lire3exec.c
// test        : ./lire3exec < /pub/FISE_OSSE11/syscall/data.in > data.out
// (l'exécutable ./lire doit être dans le répertoire courant)

#include <stdio.h>      // fprintf
#include <unistd.h>     // execl
#include <string.h>     // strerror
#include <errno.h>      // errno
#include "/pub/FISE_OSSE11/syscall/lire.h"

int main(int argc, char *argv[])
{
    // Pas d'argument attendu
    (void)argc;

    // Deux lectures/écritures de 10 octets sur les flux 0 et 1
    lire();
    lire();

    // Remplace le processus par ./lire. execl prend la liste des
    // arguments un par un : argv[0] du nouveau programme, puis NULL
    // pour terminer la liste. Les flux 0, 1, 2 sont conservés.
    execl("./lire", "lire", (char *)NULL);

    // On n'arrive ici que si execl a échoué
    fprintf(stderr, "%s: execl failed: %s\n", argv[0], strerror(errno));
    return 1;
}
