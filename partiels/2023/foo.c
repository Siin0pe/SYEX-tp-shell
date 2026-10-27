// Programme foo de la figure 2 du partiel 2023, recopié tel quel
// (seuls ces commentaires sont ajoutés) pour pouvoir le tester.
// Voir foo2.c pour la version corrigée.
//
// compilation : gcc -o foo foo.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipefd[2];

int do_the_thing(const char* f) {
    int res = 0;
    char c;
    int fd = open(f, O_RDONLY);
    if (fd != -1) {
        while (read(fd, &c, 1) == 1) res = res + 1;
        close(fd);
    }
    return write(pipefd[1], &res, sizeof(int));
}

int main(int argc, char* argv[]) {
    int fd_in, nb = 0, res = 0;
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

    while (!feof(stdin) && scanf("%s", buf) == 1) {
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(3);
        } else if (pid == 0) {
            do_the_thing(buf);
            exit(0);
        }
        nb = nb + 1;
    }

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
    printf("res = %d\n", res);

    if (argc == 2) close(fd_in);
    return 0;
}
