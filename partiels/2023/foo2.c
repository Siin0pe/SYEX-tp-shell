// usage    : foo2 [file]
// fonction : partiel 2023, exercice 5, version corrigée de foo.c.
//            Lit des noms de fichiers (séparés par des blancs) sur
//            l'entrée standard, ou dans file ; un fils par nom calcule
//            la taille du fichier (0 s'il ne peut pas être ouvert) et
//            l'envoie au père par un pipe ; le père affiche la somme.
//            Changements repérés par Q2 (taille en un appel système) et
//            Q3 (fermeture des bouts du pipe).
//
// compilation : gcc -Wall -Wextra -o foo2 foo2.c
// test        : ls | ./foo2

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipefd[2];

int do_the_thing(const char* f) {
    int res = 0;
    int fd = open(f, O_RDONLY);
    if (fd != -1) {
        // Q2 : lseek en fin de fichier renvoie la nouvelle position,
        // c'est-à-dire la taille : 1 appel système au lieu d'un read
        // par octet
        off_t size = lseek(fd, 0, SEEK_END);
        if (size != -1)
            res = (int)size;
        close(fd);
    }
    return write(pipefd[1], &res, sizeof(int));
}

int main(int argc, char* argv[]) {
    int fd_in = -1, nb = 0, res = 0;
    char buf[1024];

    if (argc > 2) {
        fprintf(stderr, "usage: %s [file]\n", argv[0]);
        exit(1);
    }
    if (argc == 2) {
        fd_in = open(argv[1], O_RDONLY);
        if (fd_in == -1) { perror("open"); exit(2); }
        dup2(fd_in, 0);
    }
    if (pipe(pipefd) == -1) { perror("pipe"); exit(3); }

    // %1023s : au plus 1023 caractères + '\0', pas de débordement de buf
    while (scanf("%1023s", buf) == 1) {
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(3);
        } else if (pid == 0) {
            // Q3 : le fils ne lit pas le pipe
            close(pipefd[0]);
            do_the_thing(buf);
            // Q3 : fermé dès que l'unique écriture est faite
            close(pipefd[1]);
            // _exit et non exit : exit ferme aussi la copie de stdin du
            // fils, et quand stdin est un fichier ordinaire la libc remet
            // l'offset (PARTAGÉ avec le père depuis le fork) à la position
            // logique du fils : le père relirait alors des noms déjà lus
            _exit(0);
        }
        nb = nb + 1;
    }
    // Q3 : le père n'écrit jamais ; il ferme après le dernier fork
    // (plus tôt, les fils suivants n'en hériteraient pas)
    close(pipefd[1]);

    while (nb > 0) {
        int tmp;
        wait(NULL);
        if (read(pipefd[0], &tmp, sizeof(int)) == -1) {
            perror("read");
            exit(4);
        }
        res = res + tmp;
        nb = nb - 1;
    }
    // Q3 : toutes les valeurs sont lues
    close(pipefd[0]);
    printf("res = %d\n", res);

    if (argc == 2) close(fd_in);
    return 0;
}
