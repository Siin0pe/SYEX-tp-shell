#!/bin/sh
# usage    : punition.sh [[n] m] [word]
# fonction : 0 arg  -> 10 lignes de 3 fois "Je ne bavarde pas en cours !"
#            1 arg  -> 10 lignes de 3 fois word
#            2 args -> 10 lignes de m fois word
#            3 args -> n lignes de m fois word
#            Sans boucle : on délègue à punition3.sh.

# Répertoire du script (pour trouver punition3.sh)
dir=$(dirname "$0")

# Choisit les valeurs de n, m et w selon le nombre d'arguments
if [ $# -eq 0 ] ; then
    # Aucun argument : tout prend sa valeur par défaut
    n=10; m=3; w='Je ne bavarde pas en cours !'
elif [ $# -eq 1 ] ; then
    # 1 argument : c'est le mot
    n=10; m=3; w="$1"
elif [ $# -eq 2 ] ; then
    # 2 arguments : m puis le mot
    n=10; m="$1"; w="$2"
elif [ $# -eq 3 ] ; then
    # 3 arguments : n, m puis le mot
    n="$1"; m="$2"; w="$3"
else
    # Plus de 3 arguments : erreur sur stderr
    echo "usage: $0 [[n] m] [word]" 1>&2
    # Statut d'erreur
    exit 1
fi

# Délègue le travail à punition3.sh (variables entre guillemets)
sh "$dir/punition3.sh" "$n" "$m" "$w"
