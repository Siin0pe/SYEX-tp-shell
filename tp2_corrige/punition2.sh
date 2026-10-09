#!/bin/sh
# usage    : punition2.sh n m word
# fonction : écrit n lignes contenant chacune m fois le mot word,
#            les mots étant séparés par un espace (2 boucles imbriquées)

# Vérifie qu'il y a exactement 3 arguments
if [ $# -ne 3 ] ; then
    # Message d'erreur sur stderr
    echo "usage: $0 n m word" 1>&2
    # Statut d'erreur
    exit 1
fi

n="$1"          # nombre de lignes
m="$2"          # nombre de mots par ligne
word="$3"       # le mot à répéter

# i = numéro de la ligne en cours
i=0
# Boucle externe : une itération par ligne
while [ $i -lt "$n" ] ; do
    # j = numéro du mot en cours dans la ligne
    j=0
    # Boucle interne : une itération par mot
    while [ $j -lt "$m" ] ; do
        # Avant chaque mot sauf le premier, écrit un espace (sans saut de ligne)
        if [ $j -gt 0 ] ; then
            echo -n " "
        fi
        # Écrit le mot sans saut de ligne (-n)
        echo -n "$word"
        # Mot suivant
        j=$((j+1))
    done
    # Termine la ligne (echo sans argument = saut de ligne)
    echo
    # Ligne suivante
    i=$((i+1))
done
