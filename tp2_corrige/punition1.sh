#!/bin/sh
# usage    : punition1.sh n word
# fonction : écrit n lignes contenant le mot word sur stdout
#            (script vu en cours, diapo 76)

# Vérifie qu'il y a exactement 2 arguments ($# = nombre d'arguments)
if [ $# -ne 2 ] ; then
    # Message d'erreur sur stderr (1>&2), $0 = nom d'invocation du script
    echo "usage: $0 n word" 1>&2
    # Quitte avec un statut d'erreur (≠ 0)
    exit 1
fi

# Donne des noms significatifs aux arguments
n="$1"          # nombre de lignes à écrire
word="$2"       # mot à écrire (guillemets : on garde les espaces)

# Compteur de lignes déjà écrites
i=0
# Tant que i < n ("$n" entre guillemets pour éviter une erreur si vide)
while [ $i -lt "$n" ] ; do
    # Écrit le mot sur une ligne
    echo "$word"
    # Incrémente le compteur ($(( )) = expansion arithmétique)
    i=$((i+1))
done
