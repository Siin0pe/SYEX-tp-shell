#!/bin/sh
# usage    : myscp.sh src dst
# fonction : partiel 2023, exercice 2 (version corrigée de la figure 1).
#            Wrapper pour scp : exactement un des deux arguments doit
#            désigner un fichier distant (contenir ':'), sinon erreur.
#   statut 1 : mauvais usage
#   statut 2 : src et dst tous deux distants
#   statut 3 : src et dst tous deux locaux
#   sinon    : celui de scp (exec remplace le script)

# check_colon arg : vrai (0) si arg contient ':', faux (1) sinon.
# Variables locales à la fonction : on n'écrase pas src et dst du script.
check_colon() {
    local orig="$1"
    local stripped=$(printf '%s\n' "$1" | sed -e '1s/://')
    # Retirer le premier ':' n'a rien changé : pas de ':'
    if [ "$orig" = "$stripped" ] ; then
        return 1
    fi
    return 0
}

if [ $# -ne 2 ] ; then
    echo >&2 "usage: $0 <src> <dst>"
    exit 1
fi

# giving pretty names to our arguments
src="$1"
dst="$2"

if check_colon "$src" ; then
    if check_colon "$dst" ; then
        echo >&2 "error: $src and $dst are both distant"
        exit 2
    fi
else
    if ! check_colon "$dst" ; then
        echo >&2 "error: $src and $dst are both local"
        exit 3
    fi
fi

exec scp "$src" "$dst"
