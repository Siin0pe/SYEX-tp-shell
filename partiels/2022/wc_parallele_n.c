// usage    : wc_parallele_n file
// fonction : partiel 2022, exercice 5, question 2. tp-wc-parallele.c
//            généralisé à N fils (ici N = 3) : le fichier est découpé en
//            N tranches, chaque fils compte les '\n' de sa tranche et
//            envoie le résultat au père par un pipe ; le père additionne.
//            Changements par rapport au listing repérés par "N".
//
// compilation : gcc -Wall -Wextra -o wc_parallele_n wc_parallele_n.c
// test        : ./wc_parallele_n /etc/services ; wc -l < /etc/services

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

// N : nombre de fils
#define N 3

int f2p[2];

void fils(char *fn, int deb, int fin) {
    char *tmp = malloc(fin - deb + 1);
    int fd = open(fn, O_RDONLY);
    lseek(fd, deb, SEEK_SET);
    int lu = read(fd, tmp, fin - deb);
    int cnt = 0;
    for (int i = 0; i < lu; i++)
        if (tmp[i] == '\n')
            cnt += 1;
    printf("%5d: cnt=%d\n", getpid(), cnt);
    write(f2p[1], &cnt, sizeof(cnt));
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        exit(1);
    }
    struct stat info;
    if (stat(argv[1], &info) < 0) {
        fprintf(stderr, "%s: can not stat %s file : %s\n",
                argv[0], argv[1], strerror(errno));
        exit(1);
    }
    // R_OK (et non O_RDONLY, qui vaut 0 = F_OK : simple test d'existence)
    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "%s: can't read %s file: %s\n",
                argv[0], argv[1], strerror(errno));
        exit(1);
    }

    int sz = info.st_size;
    pipe(f2p);

    // N : la tranche k va de k*sz/N à (k+1)*sz/N ; la dernière finit
    // exactement en sz
    for (int k = 0; k < N; k++)
        if (fork() == 0)
            fils(argv[1], k * sz / N, (k + 1) * sz / N);

    // N : une lecture par fils
    int s = 0;
    for (int k = 0; k < N; k++) {
        int x;
        read(f2p[0], &x, sizeof(x));
        s += x;
    }
    printf("%d\n", s);
    // N : une attente par fils
    for (int k = 0; k < N; k++)
        wait(NULL);
    return 0;
}
