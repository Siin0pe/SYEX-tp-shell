#!/bin/sh
# usage    : existants.sh arg1 ... argk
# fonction : partiel 2023, exercice 3. Affiche sur la sortie standard
#            ceux des arguments qui correspondent à un fichier existant.

for f in "$@" ; do
    # -e : existe (fichier, répertoire...) ; -f pour les seuls fichiers réguliers
    if [ -e "$f" ] ; then
        printf '%s\n' "$f"
    fi
done
