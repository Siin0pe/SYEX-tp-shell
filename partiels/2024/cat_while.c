// usage    : cat_while
// fonction : partiel 2024, exercice 4. Équivalent C de
//              cat | while read line ; do
//                  if [ "$line" = "q" ] ; then break ;
//                  else echo "got: '$line'" ; fi
//              done
//            left_child joue cat, right_child joue la boucle while.
//            Comme la commande shell, il ne s'arrête qu'après la
//            DEUXIÈME ligne saisie après "q" (voir README, Q4.5).
//            Flux noyau. Ajouts au squelette repérés par Q1 / Q2 / Q3.
//
// compilation : gcc -Wall -Wextra -o cat_while cat_while.c
// test        : ./cat_while   puis taper  Bonjour, q, q

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int pipefd[2];

void left_child() {
    /* close the read end of pipefd, since this child will not use it */
    // Q1
    close(pipefd[0]);

    /* while we can read a character on the standard input,
     * get it and write it on the write end of pipefd */
    // Q1 : read renvoie 0 en fin de fichier (Ctrl-D), -1 en cas d'erreur
    char c;
    while (read(0, &c, 1) == 1)
        write(pipefd[1], &c, 1);
    close(pipefd[1]);

    exit(0);
}

void right_child() {
    /* close the write end of pipefd, since this child will not use it */
    // Q2
    close(pipefd[1]);

    char line[1024];
    int index = 0;
    while (1) {
        /* get a line from the read end of pipefd,
         * or abort if the maximum size of 1024 is reached */
        index = 0;
        do {
            /* read a character from the read end of pipefd */
            // Q2 : fin de fichier (plus aucun écrivain) : on s'arrête,
            // comme "read line" qui échoue et termine le while du shell
            if (read(pipefd[0], &line[index], 1) != 1)
                exit(0);

            index++;
            // (le squelette teste "index > 1024", ce qui laisse écrire
            // line[1024] au tour suivant : ">=" est la bonne borne)
            if (index >= 1024) {
                fprintf(stderr, "error: input is too long\n");
                exit(1);
            }
        } while (line[index-1] != '\n');

        /* check whether we should stop reading or print the line */
        if (index == 2 && line[0] == 'q') {
            break;
        } else {
            /* write the expected output on the standard output */
            // Q2 : la ligne sans son '\n' (index-1 caractères)
            write(1, "got: '", 6);
            write(1, line, index - 1);
            write(1, "'\n", 2);
        }
    }
    close(pipefd[0]);
    exit(0);
}

int main() {
    /* initialize pipefd */
    // Q3
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    /* create/run the left and right children */
    // Q3
    pid_t left = fork();
    if (left == -1) { perror("fork"); exit(1); }
    if (left == 0)
        left_child();
    pid_t right = fork();
    if (right == -1) { perror("fork"); exit(1); }
    if (right == 0)
        right_child();

    /* close both ends of pipefd, since the main program will not use them */
    // Q3
    close(pipefd[0]);
    close(pipefd[1]);

    /* wait the termination of the two children */
    // Q3
    waitpid(left, NULL, 0);
    waitpid(right, NULL, 0);

    return 0;
}
