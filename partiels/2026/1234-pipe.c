// usage    : 1234-pipe
// fonction : partiel 2026, exercice 3, question 3. Comme 1234.c, mais
//            petit-fils attend de lire "x" dans un pipe avant d'écrire,
//            et fils2 écrit ce "x" après avoir écrit "22 ".
//            "4444 " sort donc toujours après "22 ".
//            Les ajouts sont repérés par Q3.
//
// compilation : gcc -Wall -Wextra -o 1234-pipe 1234-pipe.c
// test        : for i in $(seq 200) ; do ./1234-pipe ; done | sort | uniq -c

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, write, read, pipe, close
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

int main() {
    // Q3 (a)(b) : le pipe est créé dans le père AVANT les fork : fils2 en
    // hérite directement, petit-fils en hérite via fils1
    int p[2];
    if (pipe(p) == -1) { perror("pipe"); exit(1); }

    pid_t f1 = fork();
    if (f1 == -1) { perror("fork"); exit(1); }
    if (f1 == 0) {
        // Q3 (e) : ni fils1 ni petit-fils n'écrivent dans le pipe
        close(p[1]);
        write(1, "1 ", 2);
        pid_t pf = fork();
        if (pf == -1) { perror("fork"); exit(1); }
        if (pf == 0) {
            // Q3 (d) : petit-fils attend le "x" de fils2 (read bloque
            // tant que le pipe est vide)
            char c;
            read(p[0], &c, 1);
            close(p[0]);
            write(1, "4444 ", 5);
            exit(0);
        }
        // Q3 (e) : fils1 ne lit pas : il ferme p[0] dès que petit-fils
        // en a hérité
        close(p[0]);
        write(1, "333 ", 4);
        waitpid(pf, NULL, 0);
        exit(0);
    }

    pid_t f2 = fork();
    if (f2 == -1) { perror("fork"); exit(1); }
    if (f2 == 0) {
        // Q3 (c) : fils2 ne lit pas ; il écrit "x" après "22 "
        close(p[0]);
        write(1, "22 ", 3);
        write(p[1], "x", 1);
        close(p[1]);
        exit(0);
    }

    // Q3 (e) : le père n'utilise pas le pipe : il ferme les deux bouts
    // dès que ses deux fils sont créés
    close(p[0]);
    close(p[1]);

    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);
    write(1, "\n", 1);
    return 0;
}
