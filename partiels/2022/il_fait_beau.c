// usage    : il_fait_beau
// fonction : partiel 2022, exercice 4. Le père crée 3 fils :
//              fils 1 écrit "beau", fils 2 "il", fils 3 "fait".
//            Pour que la sortie soit toujours "il fait beau", chaque fils
//            attend son tour dans un pipe (un "jeton") :
//              fils 2 écrit, puis passe le jeton à fils 3 (pipe a) ;
//              fils 3 attend le jeton, écrit, puis le passe à fils 1 (pipe b) ;
//              fils 1 attend le jeton, écrit.
//            Flux noyau uniquement.
//
// compilation : gcc -Wall -Wextra -o il_fait_beau il_fait_beau.c
// test        : for i in $(seq 500) ; do ./il_fait_beau ; done | sort | uniq -c

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, pipe, read, write, close
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait

// Pipes créés par le père avant les fork : a (fils 2 -> fils 3) et
// b (fils 3 -> fils 1)
int a[2], b[2];

// Ferme les 4 bouts sauf ceux dont le fils a besoin : sans cela un read
// pourrait ne jamais voir la fin de fichier si un fils mourait
void ferme_sauf(int garde1, int garde2) {
    int fds[4] = { a[0], a[1], b[0], b[1] };
    for (int i = 0; i < 4; i++)
        if (fds[i] != garde1 && fds[i] != garde2)
            close(fds[i]);
}

// Attend le jeton sur fd (read bloque tant que le pipe est vide)
void attend(int fd) {
    char c;
    read(fd, &c, 1);
    close(fd);
}

// Donne le jeton sur fd
void donne(int fd) {
    write(fd, "x", 1);
    close(fd);
}

int main() {
    if (pipe(a) == -1 || pipe(b) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fils 1 : "beau", après fils 3
    pid_t p = fork();
    if (p == -1) { perror("fork"); exit(1); }
    if (p == 0) {
        ferme_sauf(b[0], -1);
        attend(b[0]);
        write(1, "beau\n", 5);
        exit(0);
    }

    // Fils 2 : "il", en premier
    p = fork();
    if (p == -1) { perror("fork"); exit(1); }
    if (p == 0) {
        ferme_sauf(a[1], -1);
        write(1, "il ", 3);
        donne(a[1]);
        exit(0);
    }

    // Fils 3 : "fait", après fils 2
    p = fork();
    if (p == -1) { perror("fork"); exit(1); }
    if (p == 0) {
        ferme_sauf(a[0], b[1]);
        attend(a[0]);
        write(1, "fait ", 5);
        donne(b[1]);
        exit(0);
    }

    // Père : n'utilise aucun pipe ; attend ses trois fils
    ferme_sauf(-1, -1);
    for (int i = 0; i < 3; i++)
        wait(NULL);
    return 0;
}
