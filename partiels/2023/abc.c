// usage    : abc
// fonction : partiel 2023, exercice 4, question 1. Arbre de processus
//                père -> fils -> petit-fils
//            Écritures avec des flux noyau (write).
//
// compilation : gcc -Wall -Wextra -o abc abc.c
// test        : for i in $(seq 1000) ; do ./abc ; done | sort | uniq -c

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, write
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

int main() {
    pid_t f = fork();
    if (f == -1) { perror("fork"); exit(1); }
    if (f == 0) {
        // fils : crée petit-fils, "bbb ", attend petit-fils
        pid_t pf = fork();
        if (pf == -1) { perror("fork"); exit(1); }
        if (pf == 0) {
            // petit-fils : "ccc "
            write(1, "ccc ", 4);
            exit(0);
        }
        write(1, "bbb ", 4);
        waitpid(pf, NULL, 0);
        exit(0);
    }
    // père : "aaa ", attend fils, "\n"
    write(1, "aaa ", 4);
    waitpid(f, NULL, 0);
    write(1, "\n", 1);
    return 0;
}
