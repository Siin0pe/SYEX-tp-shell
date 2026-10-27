#!/bin/sh
# usage    : count_by_pattern.sh [pattern ...]
# fonction : partiel 2025, exercice 2. Pour chaque motif, affiche le
#            nombre de fichiers réguliers du répertoire courant (et de ses
#            sous-répertoires) dont le nom correspond au motif.
#            Aucun argument : n'affiche rien.

for pattern in "$@" ; do
    # -name "$pattern" : le motif est passé tel quel à find (guillemets :
    # le shell ne doit pas développer '*.c' lui-même)
    n=$(find . -type f -name "$pattern" | wc -l)
    # $((n)) enlève les éventuels espaces que certains wc mettent devant
    echo "'$pattern': $((n)) found"
done
