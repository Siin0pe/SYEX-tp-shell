// usage    : 1234-signal
// fonction : partiel 2026, exercice 3, question 5. Comme 1234-pipe.c, et
//            en plus fils1 se met en pause après avoir créé petit-fils,
//            jusqu'à ce que fils2 lui envoie SIGUSR1 (après "22 ").
//            "333 " et "4444 " sortent donc toujours après "22 ".
//            Les ajouts sont repérés par Q5.
//
// compilation : gcc -Wall -Wextra -o 1234-signal 1234-signal.c
// test        : for i in $(seq 200) ; do ./1234-signal ; done | sort | uniq -c

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // fork, write, read, pipe, close, pause
#include <signal.h>     // signal, kill, SIGUSR1
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

// Q5 (d) : mis à 1 par le handler quand fils1 reçoit SIGUSR1
volatile sig_atomic_t recu = 0;

// Q5 (d) : sans handler, l'action par défaut de SIGUSR1 TUE fils1
void handler(int sig) {
    (void)sig;
    recu = 1;
}

int main() {
    // Q5 (d) : handler installé dans le père, AVANT les fork : fils1 en
    // hérite, et il est en place avant que fils2 puisse envoyer le signal
    if (signal(SIGUSR1, handler) == SIG_ERR) { perror("signal"); exit(1); }

    int p[2];
    if (pipe(p) == -1) { perror("pipe"); exit(1); }

    // Q5 (a) : f1 est créé AVANT fils2 : fils2 hérite d'une copie de f1,
    // qui contient le PID de fils1
    pid_t f1 = fork();
    if (f1 == -1) { perror("fork"); exit(1); }
    if (f1 == 0) {
        close(p[1]);
        write(1, "1 ", 2);
        pid_t pf = fork();
        if (pf == -1) { perror("fork"); exit(1); }
        if (pf == 0) {
            char c;
            read(p[0], &c, 1);
            close(p[0]);
            write(1, "4444 ", 5);
            exit(0);
        }
        close(p[0]);
        // Q5 (b) : pause jusqu'au signal de fils2. La boucle sur "recu"
        // évite de bloquer si le signal est arrivé AVANT pause (sinon
        // pause attendrait un second signal qui ne viendra jamais).
        // Il reste une fenêtre minuscule entre le test et pause ; la
        // version sans aucune course utilise sigprocmask + sigsuspend.
        while (!recu)
            pause();
        write(1, "333 ", 4);
        waitpid(pf, NULL, 0);
        exit(0);
    }

    pid_t f2 = fork();
    if (f2 == -1) { perror("fork"); exit(1); }
    if (f2 == 0) {
        close(p[0]);
        write(1, "22 ", 3);
        // Q5 (c) : prévient fils1 juste après l'écriture
        kill(f1, SIGUSR1);
        write(p[1], "x", 1);
        close(p[1]);
        exit(0);
    }

    close(p[0]);
    close(p[1]);

    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);
    write(1, "\n", 1);
    return 0;
}
