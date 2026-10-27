// usage    : tester exec input.txt
// fonction : partiel 2025, exercice 4, questions 1 à 3. Lance exec avec
//            input.txt comme entrée standard ; si exec ne s'est pas
//            terminé au bout de 60 s, affiche "FAIL: timeout".
//            La réponse de exec s'affiche directement sur la sortie
//            standard. Ajouts au squelette repérés par Q1 / Q2 / Q3.
//   statut 0 : exec terminé à temps, ou timeout (voir alarm_handler)
//   statut 1 : mauvais usage, ou échec dans le fils (open, dup2, execl)
//   statut 2 : échec de signal
//   statut 3 : échec de fork
//
// compilation : gcc -Wall -Wextra -o tester tester.c
// test        : echo 'whatever' > input.txt ; ./tester /bin/cat input.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>


void alarm_handler(int signum) {
    /* print some error message and terminate the program */
    assert(signum == SIGALRM);
    printf("FAIL: timeout\n");
    exit(0);
}

// Q3 : handler vide. Son seul rôle est d'exister : un signal dont
// l'action est SIG_DFL et qui est ignoré par défaut (cas de SIGCHLD)
// ne réveille pas pause(). Avec un handler, pause() rend la main.
void chld_handler(int signum) {
    (void)signum;
}

void child(const char* execfile, const char* inputfile) {
    /* get read-only access to the content of the file 'inputfile' */
    // Q2
    int fd = open(inputfile, O_RDONLY);
    if (fd == -1) {
        perror("[open]");
        exit(1);
    }

    /* call dup2 in order to use the content of 'inputfile' as the standard input */
    // Q2 : le descripteur 0 désigne maintenant inputfile ; fd ne sert plus
    if (dup2(fd, 0) == -1) {
        perror("[dup2]");
        exit(1);
    }
    close(fd);

    /* call execl in order to run the executable file 'execfile' */
    // Q2 : par convention argv[0] = nom du programme ; liste terminée par NULL
    execl(execfile, execfile, (char*)NULL);
    // On n'arrive ici que si execl a échoué
    perror("[execl]");

    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <exec> <input.txt>\n", argv[0]);
        exit(1);
    }

    /* attach the handler 'alarm_handler' to the signal SIGALRM */
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("[signal]");
        exit(2);
    }
    // Q3 : installé AVANT le fork, sinon un fils très rapide pourrait se
    // terminer avant que le handler soit en place
    if (signal(SIGCHLD, chld_handler) == SIG_ERR) {
        perror("[signal]");
        exit(2);
    }

    /* create a child process and call the function 'child'
     * with the approriate arguments */
    // Q1
    pid_t pid = fork();
    if (pid == -1) {
        perror("[fork]");
        exit(3);
    }
    if (pid == 0)
        child(argv[1], argv[2]);   // ne revient jamais (exit ou exec)

    /* set up an alarm for 60s from now */
    alarm(60);

    pause(); /* waiting for either SIGALRM or SIGCHLD */

    /* at this point, we know that the child process has terminated */
    /* clean up allocated resources and end the program */
    // Q1 : annule l'alarme, puis récupère le fils (sinon il reste zombie
    // jusqu'à la fin du père)
    alarm(0);
    waitpid(pid, NULL, 0);

    return 0;
}
