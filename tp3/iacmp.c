// usage    : iacmp s1 s2 ... sn
// fonction : wrapper de /pub/FISE_OSSE11/shell/iacmp/iacmp (version C de
//            l'exercice 2 du TP2) : configure l'environnement puis se
//            remplace par iacmp en lui transmettant ses arguments.
//   statut : celui de iacmp, ou 1 si execv échoue
//
// compilation : gcc -Wall -Wextra -o iacmp iacmp.c
// test        : ./iacmp 'chat roux' 'chat blanc' 'chat noir'

#include <stdio.h>      // fprintf
#include <stdlib.h>     // setenv
#include <unistd.h>     // execv
#include <string.h>     // strerror
#include <errno.h>      // errno

int main(int argc, char* argv[])
{
    // argc n'est pas utilisé : argv se termine par NULL, execv s'en sert
    (void)argc;

    // Ajoute ou remplace (3e argument = 1) les variables d'environnement.
    // Elles sont transmises au programme lancé par execv (cours diapo 62).
    setenv("IacmpDir",        "/pub/FISE_OSSE11/shell/iacmp",     1);
    setenv("LD_LIBRARY_PATH", "/pub/FISE_OSSE11/shell/iacmp/lib", 1);

    // Remplace ce processus par iacmp (équivalent du "exec" shell).
    // argv est transmis tel quel : argv[1..] sont les chaînes à trier,
    // quel que soit leur nombre, et argv[argc] vaut NULL.
    // execv ne cherche pas dans le PATH : il faut le chemin complet.
    execv("/pub/FISE_OSSE11/shell/iacmp/iacmp", argv);

    // On n'arrive ici QUE si execv a échoué (en cas de succès il n'y a
    // pas de retour, cours diapo 56)
    fprintf(stderr, "%s: execv failed: %s\n",
            argv[0], strerror(errno)
           );
    return 1;
}
