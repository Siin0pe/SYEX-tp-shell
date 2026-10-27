#!/bin/sh
# usage    : my_ocamlrun.sh [options] filename [arguments]
# fonction : partiel 2024, exercice 2 (version corrigée de la figure 1).
#            Refuse l'option obsolète -bt, sinon transmet tous ses
#            arguments à ocamlrun.
#   statut 1 : mauvais usage ou option -bt
#   sinon    : celui de ocamlrun (exec remplace le script)

# check arg : vrai (0) si arg est une option, c'est-à-dire si son premier
# caractère est '-' ; faux (1) sinon
check() {
    first_char=$(printf '%s' "$1" | sed -e 's/\(.\).*/\1/')
    if [ "$first_char" = "-" ] ; then
        return 0
    fi
    return 1
}

if [ $# -lt 1 ] ; then
    echo >&2 'error: at least one argument expected'
    exit 1
fi

# Parcourt les options, jusqu'au premier argument qui n'en est pas une
# (filename) ; found=yes si on a bien trouvé ce filename
found=no
for arg in "$@" ; do
    if check "$arg" ; then
        if [ "$arg" = '-bt' ] ; then
            echo >&2 "option '-bt' is not valid anymore, use '-b -t' instead"
            exit 1
        fi
    else
        found=yes
        break
    fi
done

# Que des options : il manque filename
if [ "$found" = no ] ; then
    echo >&2 'error: filename expected'
    exit 1
fi

# Transmet TOUS les arguments, intacts
exec ocamlrun "$@"
