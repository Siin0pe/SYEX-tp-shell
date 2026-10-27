// usage    : tester2 exec input.txt
// fonction : partiel 2025, exercice 4, questions 4 à 6. Comme tester.c,
//            mais :
//              Q4 : waitpid à la place de pause (pas de course avec
//                   SIGCHLD, plus besoin de chld_handler) ;
//              Q5 : la sortie de exec passe par un pipe ; le père la lit
//                   et la compare à "42\n" ;
//              Q6 : un échec de execl est transmis au père par le statut
//                   de sortie du fils (127, comme le shell).
//            En cas de timeout, le fils est tué avant de quitter.
//   statut 0 : PASS
//   statut 1 : mauvais usage, ou FAIL (mauvaise réponse, timeout, exec)
//   statut 2 : échec de signal, pipe, fork
//
// compilation : gcc -Wall -Wextra -o tester2 tester2.c
// test        : echo 42 > input.txt ; ./tester2 /bin/cat input.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>

// Q6 : statut réservé à "execl a échoué" (même convention que le shell)
#define EXEC_FAILED 127

// PID du fils, global pour que alarm_handler puisse le tuer
pid_t pid = -1;

void alarm_handler(int signum) {
    assert(signum == SIGALRM);
    printf("FAIL: timeout\n");
    // Sans ce kill, le fils continuerait de tourner après la fin du père.
    // (Seul le fils est tué : si exec est un script qui a lui-même lancé
    // des processus, ceux-ci survivent ; il faudrait un groupe de
    // processus, setpgid + kill(-pid, ...), hors programme.)
    if (pid > 0)
        kill(pid, SIGKILL);
    exit(1);
}

// Q5 : out = bout d'écriture du pipe, qui deviendra la sortie standard
void child(const char* execfile, const char* inputfile, int out) {
    int fd = open(inputfile, O_RDONLY);
    if (fd == -1) {
        perror("[open]");
        exit(EXEC_FAILED);
    }
    if (dup2(fd, 0) == -1 || dup2(out, 1) == -1) {
        perror("[dup2]");
        exit(EXEC_FAILED);
    }
    close(fd);
    close(out);

    execl(execfile, execfile, (char*)NULL);
    perror("[execl]");
    // Q6 : statut distinctif, récupéré par le père avec waitpid
    exit(EXEC_FAILED);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <exec> <input.txt>\n", argv[0]);
        exit(1);
    }
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("[signal]");
        exit(2);
    }

    // Q5 : pipe fils -> père, créé avant le fork pour être hérité
    int p[2];
    if (pipe(p) == -1) {
        perror("[pipe]");
        exit(2);
    }

    pid = fork();
    if (pid == -1) {
        perror("[fork]");
        exit(2);
    }
    if (pid == 0) {
        close(p[0]);                    // le fils ne lit pas le pipe
        child(argv[1], argv[2], p[1]);
    }
    // Le père n'écrit pas : indispensable pour voir la fin de fichier
    close(p[1]);

    alarm(60);

    // Q5 : lit TOUTE la sortie du fils (jusqu'à EOF = fils terminé ou
    // sortie fermée), AVANT d'attendre le fils : sinon un fils qui écrit
    // plus que la capacité du pipe resterait bloqué
    char buf[64];
    size_t len = 0;
    char c;
    while (read(p[0], &c, 1) == 1) {
        if (len < sizeof(buf) - 1)
            buf[len] = c;
        len++;
    }
    close(p[0]);
    buf[len < sizeof(buf) ? len : sizeof(buf) - 1] = '\0';

    // Q4 : waitpid bloque jusqu'à la fin du fils, même si elle a eu lieu
    // avant cet appel (le fils est alors zombie) : aucune course possible
    int status;
    waitpid(pid, &status, 0);
    alarm(0);

    // Q6 : le père sait distinguer un échec de execl
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXEC_FAILED) {
        printf("FAIL: cannot run '%s'\n", argv[1]);
        return 1;
    }
    // Q5 : une unique ligne "42"
    if (len == 3 && strcmp(buf, "42\n") == 0) {
        printf("PASS\n");
        return 0;
    }
    printf("FAIL: wrong answer\n");
    return 1;
}
