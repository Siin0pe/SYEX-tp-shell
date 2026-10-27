#!/bin/bash
# usage    : premiere_ligne.sh n f
# fonction : partiel 2022, exercice 3. Affiche n fois la première ligne
#            du fichier f.
#   statut 0 : succès
#   statut 1 : mauvais usage (nombre d'arguments, n invalide, f illisible)

if [ $# -ne 2 ] ; then
    echo "usage: $0 n f" >&2
    exit 1
fi
n="$1"
f="$2"

# n doit être un entier positif ou nul : vide ou caractère non chiffre = erreur
case "$n" in
    '' | *[!0-9]*)
        echo "$0: '$n' is not a non-negative integer" >&2
        exit 1 ;;
esac
# f doit être un fichier régulier lisible
if [ ! -f "$f" ] || [ ! -r "$f" ] ; then
    echo "$0: cannot read file '$f'" >&2
    exit 1
fi

# Première ligne lue une seule fois
ligne=$(head -n 1 "$f")
i=0
while [ $i -lt $n ] ; do
    printf '%s\n' "$ligne"
    i=$((i+1))
done
