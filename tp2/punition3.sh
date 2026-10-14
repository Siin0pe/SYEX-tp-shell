#!/bin/sh
# usage    : punition3.sh n m word
# fonction : comme punition2.sh, mais avec une seule boucle :
#            on construit la ligne "word word ... word" (m fois),
#            puis on demande à punition1.sh de l'écrire n fois.

# Vérifie qu'il y a exactement 3 arguments
if [ $# -ne 3 ] ; then
    # Message d'erreur sur stderr
    echo "usage: $0 n m word" 1>&2
    # Statut d'erreur
    exit 1
fi

n="$1"          # nombre de lignes
m="$2"          # nombre de mots par ligne
w="$3"          # le mot

# Répertoire où se trouve ce script : $0 sert à retrouver le répertoire
# d'installation (cours diapo 23), donc punition1.sh est trouvé
# quel que soit le CWD
dir=$(dirname "$0")

# str = la ligne en construction, vide au départ ; i = compteur
str= ; i=0
# Répète m fois
while [ $i -lt "$m" ] ; do
    # Premier mot : pas d'espace devant
    if [ $i -eq 0 ] ; then
        str="$w"
    # Mots suivants : on concatène un espace puis le mot
    else
        str="$str $w"
    fi
    # Incrémente le compteur
    i=$((i+1))
done

# Délègue l'écriture des n lignes à punition1.sh
# (guillemets : "$str" reste UN SEUL argument malgré ses espaces)
sh "$dir/punition1.sh" "$n" "$str"
