// usage    : readpasswd
// fonction : exercice 6, questions 3 à 6 : lit un mot de passe d'au plus
//            15 caractères sans l'afficher en clair. Le terminal est mis
//            en mode raw sans écho ; un '*' est affiché par caractère, et
//            backspace efface le dernier caractère saisi.
//   statut 0 : succès
//   statut 1 : l'entrée standard n'est pas un terminal, ou erreur
//
// ttyname n'est pas dans le cours : c'est le sujet qui l'impose.
// system est dans le cours (diapo 64).
//
// compilation : gcc -Wall -Wextra -o readpasswd readpasswd.c
// test        : ./readpasswd

#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // system, exit
#include <unistd.h>     // read, write, close, ttyname
#include <fcntl.h>      // open, O_RDWR
#include <string.h>     // strerror
#include <errno.h>      // errno

// Descripteur du terminal ouvert dans main : readpassword y écrit les '*'.
// Variable globale car le prototype imposé ne prend que pass.
int tty;

// Lit au plus 15 caractères sur le flux 0, caractère par caractère, jusqu'à
// un saut de ligne ('\n' ou '\r'). Les 15 premiers sont rangés dans pass
// (zone de 16 octets), suivis de '\0'. Renvoie le nombre n de caractères
// lus, saut de ligne non compris (n peut dépasser 15).
int readpassword(char* pass)
{
    // Nombre de caractères lus
    int n = 0;
    // Caractère lu
    char c;

    // read renvoie 1 si un octet est lu, 0 en fin de fichier, -1 si erreur
    while (read(0, &c, 1) == 1) {
        // Saut de ligne : fin de la saisie. En mode raw, <ENTER> envoie '\r'
        if (c == '\n' || c == '\r')
            break;
        // Q6 : backspace (caractère 127 en mode raw)
        if (c == 127) {
            // Rien à effacer : on ignore le caractère
            if (n == 0)
                continue;
            // Oublie le dernier caractère
            n -= 1;
            // Recule, écrit un espace par-dessus le '*', recule encore
            write(tty, "\b \b", 3);
            continue;
        }
        // On ne range que les 15 premiers caractères
        if (n < 15)
            pass[n] = c;
        // Mais on les compte tous
        n += 1;
        // Q4 : un '*' par caractère reçu
        write(tty, "*", 1);
    }

    // Termine la chaîne : pass[15] si n >= 16, pass[n] sinon
    if (n >= 16)
        pass[15] = '\0';
    else
        pass[n] = '\0';
    return n;
}

int main(int argc, char *argv[])
{
    // Pas d'argument attendu
    (void)argc;
    // 16 octets : 15 caractères + '\0'
    char pass[16];

    // a. Chemin du terminal associé au flux 0 (ex : /dev/pts/3).
    //    NULL si le flux 0 n'est pas un terminal (pipe, fichier...)
    char* ttypath = ttyname(0);
    if (ttypath == NULL) {
        fprintf(stderr, "%s: Fatal: standard input is not a terminal\n", argv[0]);
        exit(1);
    }

    // b. Ouvre le terminal en lecture/écriture
    tty = open(ttypath, O_RDWR);
    if (tty == -1) {
        fprintf(stderr, "%s: Fatal: cannot open '%s': %s\n",
                argv[0], ttypath, strerror(errno));
        exit(1);
    }

    // c. Mode raw (caractères reçus dès la frappe) et sans écho.
    //    stty agit sur son entrée standard, héritée = notre terminal
    system("/bin/stty raw -echo");

    // d. Invite, puis lecture
    write(tty, "password: ", 10);
    int n = readpassword(pass);

    // e. Remet le terminal en mode normal (sinon le shell reste sans écho)
    system("/bin/stty -raw echo");

    // En mode raw '\n' ne ramène pas en début de ligne : on a écrit
    // des '*' sur la ligne de l'invite, on passe à la ligne suivante
    write(tty, "\n", 1);
    close(tty);

    // Résultat (pour tester ; un vrai programme ne l'afficherait pas)
    printf("%d caracteres lus, mot de passe = '%s'\n", n, pass);
    return 0;
}
