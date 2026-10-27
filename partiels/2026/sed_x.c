// usage    : sed_x file
// fonction : partiel 2026, exercice 2. Simule fidèlement la commande
//                sed -e 's/x//g' file > file
//            Le shell traite la redirection "> file" AVANT de lancer sed :
//            on ouvre donc la sortie (ce qui tronque le fichier) avant
//            l'entrée. Résultat : le fichier finit vide. Flux libc.
//   statut 0 : succès
//   statut 1 : mauvais usage, ouverture impossible
//
// compilation : gcc -Wall -Wextra -o sed_x sed_x.c
// test        : printf 'xaxbx\n' > file.txt ; ./sed_x file.txt ; wc -c file.txt

#include <stdio.h>      // FILE, fopen, fclose, fread, fwrite, fprintf
#include <stdlib.h>     // exit
#include <string.h>     // strerror
#include <errno.h>      // errno

// Q2.2 : recopie input dans output en sautant les 'x'
void apply_sed(FILE* input, FILE* output) {
    char ch;
    // fread renvoie 1 tant qu'un caractère a été lu
    while (fread(&ch, 1, 1, input) == 1) {
        if (ch != 'x')
            fwrite(&ch, 1, 1, output);
    }
}

// Q2.3 : ouverture en lecture seule ("r"), échec si le fichier n'existe pas
FILE* open_input(const char* filepath) {
    FILE* f = fopen(filepath, "r");
    if (f == NULL) {
        fprintf(stderr, "Fatal: cannot open '%s' for reading: %s\n",
                filepath, strerror(errno));
        exit(1);
    }
    return f;
}

// Q2.3 : ouverture en écriture seule ("w") : crée le fichier s'il n'existe
// pas, et le TRONQUE (taille ramenée à 0) s'il existe
FILE* open_output(const char* filepath) {
    FILE* f = fopen(filepath, "w");
    if (f == NULL) {
        fprintf(stderr, "Fatal: cannot open '%s' for writing: %s\n",
                filepath, strerror(errno));
        exit(1);
    }
    return f;
}

int main(int argc, char* argv[]) {
    // Exactement un argument
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        exit(1);
    }

    // Q2.4 : même ordre que le shell : la redirection "> file" d'abord,
    // puis sed ouvre file en lecture, puis la substitution
    FILE* output = open_output(argv[1]);
    FILE* input = open_input(argv[1]);
    apply_sed(input, output);

    // Nettoyage
    fclose(input);
    fclose(output);
    return 0;
}
