// usage    : readpasswd1
// fonction : exercice 6, questions 1 et 2 : lit un mot de passe d'au plus
//            15 caractères sur l'entrée standard et l'affiche.
//            Le mot de passe s'affiche en clair pendant la saisie
//            (corrigé dans readpasswd.c).
//
// compilation : gcc -Wall -Wextra -o readpasswd1 readpasswd1.c
// test        : ./readpasswd1     puis taper un mot de passe et <ENTER>
//               echo 0123456789abcdefghij | ./readpasswd1

#include <stdio.h>      // printf, fprintf
#include <unistd.h>     // read

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
        // Saut de ligne : fin de la saisie
        if (c == '\n' || c == '\r')
            break;
        // On ne range que les 15 premiers caractères
        if (n < 15)
            pass[n] = c;
        // Mais on les compte tous
        n += 1;
    }

    // Termine la chaîne : pass[15] si n >= 16, pass[n] sinon
    if (n >= 16)
        pass[15] = '\0';
    else
        pass[n] = '\0';
    return n;
}

int main()
{
    // 16 octets : 15 caractères + '\0'
    char pass[16];

    // Invite sur stdout ; pas de '\n' donc on force l'écriture du tampon
    printf("password: ");
    fflush(stdout);

    // Lecture
    int n = readpassword(pass);

    // Résultat
    printf("%d caracteres lus, mot de passe = '%s'\n", n, pass);
    return 0;
}
