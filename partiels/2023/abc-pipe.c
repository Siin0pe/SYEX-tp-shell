// usage    : abc-pipe
// fonction : partiel 2023, exercice 4, question 3. Comme abc.c, et en plus
//            le père récupère le PID du petit-fils par un pipe.
//            Ajouts repérés par Q3.
//
// compilation : gcc -Wall -Wextra -o abc-pipe abc-pipe.c

#include <stdio.h>      // perror, printf
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, write, read, pipe, close
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

int main() {
    // Q3 : pipe créé AVANT le fork : le fils en hérite
    int p[2];
    if (pipe(p) == -1) { perror("pipe"); exit(1); }

    pid_t f = fork();
    if (f == -1) { perror("fork"); exit(1); }
    if (f == 0) {
        // Q3 : le fils ne lit pas
        close(p[0]);
        pid_t pf = fork();
        if (pf == -1) { perror("fork"); exit(1); }
        if (pf == 0) {
            // Q3 : le petit-fils n'utilise pas le pipe
            close(p[1]);
            write(1, "ccc ", 4);
            exit(0);
        }
        // Q3 : fork a renvoyé au fils le PID du petit-fils : il l'envoie
        write(p[1], &pf, sizeof(pf));
        close(p[1]);
        write(1, "bbb ", 4);
        waitpid(pf, NULL, 0);
        exit(0);
    }
    // Q3 : le père n'écrit pas
    close(p[1]);
    write(1, "aaa ", 4);

    // Q3 : read bloque jusqu'à ce que le fils ait écrit le PID
    pid_t pf;
    if (read(p[0], &pf, sizeof(pf)) != sizeof(pf)) {
        perror("read");
        exit(1);
    }
    close(p[0]);

    waitpid(f, NULL, 0);
    write(1, "\n", 1);
    printf("PID du petit-fils : %d\n", (int)pf);
    return 0;
}
