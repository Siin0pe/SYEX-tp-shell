#!/bin/sh
# usage    : filter_by_keywords.sh dirname keyword1 [keyword2 ...]
# fonction : partiel 2026, exercice 1 (version corrigée de la figure 1).
#            Affiche les fichiers .txt situés directement dans dirname
#            (pas dans les sous-répertoires) dont les 5 premières lignes
#            contiennent chacun des mots-clés.
#   statut 0 : succès
#   statut 1 : mauvais usage

# check file keyword : vrai (0) si une des 5 premières lignes de file
# contient keyword, faux (1) sinon.
# grep -q : ne rien afficher, juste le statut ; -F : keyword est une chaîne
# fixe (pas une regex, donc pas d'injection) ; -e : keyword peut commencer
# par '-'.
check() {
    head -n 5 "$1" | grep -q -F -e "$2"
}

# Au moins 2 arguments : le répertoire et un mot-clé
if [ $# -lt 2 ] ; then
    echo "usage: $0 <dirname> <keyword1> [<keyword2> ...]" >&2
    exit 1
fi
# Le premier argument doit être un répertoire
if [ ! -d "$1" ] ; then
    echo "$0: '$1' is not a directory" >&2
    exit 1
fi

# Nomme le répertoire, puis le retire : "$@" ne contient plus que les mots-clés
dirname="$1"
shift

# -maxdepth 1 : seulement les fichiers directement dans dirname
find "$dirname" -maxdepth 1 -type f -name '*.txt' |
while IFS= read -r file ; do
    # Remis à "no" pour CHAQUE fichier
    skip=no
    for keyword in "$@" ; do
        # Un mot-clé absent suffit à écarter le fichier
        if ! check "$file" "$keyword" ; then
            skip=yes
            break
        fi
    done
    if [ "$skip" = "no" ] ; then
        echo "$file"
    fi
done
