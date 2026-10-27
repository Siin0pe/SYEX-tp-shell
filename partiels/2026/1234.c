// usage    : 1234
// fonction : partiel 2026, exercice 3, question 1. Arbre de processus
//                père -> fils1 -> petit-fils
//                     -> fils2
//            Écritures avec des flux noyau (write).
//
// compilation : gcc -Wall -Wextra -o 1234 1234.c
// test        : for i in $(seq 200) ; do ./1234 ; done | sort | uniq -c

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, write
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

int main() {
    // Crée fils1
    pid_t f1 = fork();
    if (f1 == -1) { perror("fork"); exit(1); }
    if (f1 == 0) {
        // fils1 : "1 ", crée petit-fils, "333 ", attend petit-fils
        write(1, "1 ", 2);
        pid_t pf = fork();
        if (pf == -1) { perror("fork"); exit(1); }
        if (pf == 0) {
            // petit-fils : "4444 "
            write(1, "4444 ", 5);
            exit(0);
        }
        write(1, "333 ", 4);
        waitpid(pf, NULL, 0);
        exit(0);
    }

    // Crée fils2
    pid_t f2 = fork();
    if (f2 == -1) { perror("fork"); exit(1); }
    if (f2 == 0) {
        // fils2 : "22 "
        write(1, "22 ", 3);
        exit(0);
    }

    // père : attend ses deux fils, puis "\n"
    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);
    write(1, "\n", 1);
    return 0;
}
