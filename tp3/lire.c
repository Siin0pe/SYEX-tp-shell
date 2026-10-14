// usage    : lire
// fonction : écrit "Je suis le programme lire." sur stderr, puis appelle
//            la fonction lire() de /pub/FISE_OSSE11/syscall/lire.c
//            (lit au plus 10 octets sur le flux 0, les écrit sur le flux 1).
//
// compilation : gcc -Wall -Wextra -o lire /pub/FISE_OSSE11/syscall/lire.c lire.c
// test        : ./lire < /pub/FISE_OSSE11/syscall/data.in

# include <stdio.h>
# include "/pub/FISE_OSSE11/syscall/lire.h"

int main() {
    fprintf(stderr, "Je suis le programme lire.\n");
    lire();
    return 0;
}
