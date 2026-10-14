#!/bin/sh
# usage    : extract2.sh [dir]
# fonction : comme extract.sh, mais le répertoire de travail est passé en
#            argument (/tmp par défaut). Exercice 9.2.
# Modifications par rapport à extract.sh : vérifier les arguments, puis
# d=${1:-/tmp} au lieu de d=/tmp. Tout le reste utilise déjà $d.

# 0 ou 1 argument seulement
if [ $# -gt 1 ] ; then
    # Erreur sur stderr
    echo "error: usage: extract2.sh [dir]" 1>&2
    # Statut 1
    exit 1
fi

# ${1:-/tmp} : vaut $1 s'il est défini et non vide, /tmp sinon (diapo 40)
d="${1:-/tmp}"

# Le répertoire doit exister
if [ ! -d "$d" ] ; then
    # Erreur sur stderr
    echo "error: '$d' is not a directory" 1>&2
    # Statut 1
    exit 1
fi

# La liste des étudiants doit être lisible
if [ ! -r "$d/etudiants.txt" ] ; then
    # Erreur sur stderr
    echo "error: '$d/etudiants.txt' is not readable" 1>&2
    # Statut 1
    exit 1
fi

# --- 1. Crée le répertoire correction, ou erreur s'il existe déjà ---
if [ -e "$d/correction" ] ; then
    # Erreur sur stderr
    echo "error: '$d/correction' already exists" 1>&2
    # Statut 1
    exit 1
fi
# Création, avec arrêt si elle échoue
mkdir "$d/correction" || exit 2

# --- 2 et 3. Un répertoire par étudiant, et on y range son rendu ---
# Lit la liste ligne par ligne
while read u ; do
    # Ignore les lignes vides
    if [ -z "$u" ] ; then
        # Ligne suivante
        continue
    fi
    # Répertoire de l'étudiant
    mkdir "$d/correction/$u"
    # Le rendu existe-t-il ?
    if [ -f "$d/$u.tar.gz" ] ; then
        # Oui : on le déplace
        mv "$d/$u.tar.gz" "$d/correction/$u/"
    else
        # Non : erreur avec le nom de l'étudiant
        echo "error: no submission for '$u'" 1>&2
    fi
done < "$d/etudiants.txt"

# --- 4. Signale les rendus restants ---
# Parcourt les .tar.gz encore présents
for f in "$d"/*.tar.gz ; do
    # Motif sans correspondance : il est gardé tel quel, on l'ignore
    if [ ! -e "$f" ] ; then
        # Rien à signaler
        continue
    fi
    # Erreur avec le nom du fichier
    echo "error: '$f' should not be here" 1>&2
done
