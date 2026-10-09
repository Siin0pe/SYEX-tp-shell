#!/bin/sh
# usage    : extract.sh
# fonction : prépare /tmp/correction à partir de /tmp/etudiants.txt et
#            des rendus /tmp/xxxx.tar.gz (exercice 9.1, rattrapage 2024-2025)

# Répertoire de travail. Il est mis dans une variable dès le départ,
# ce qui rend la question 2 triviale (voir extract2.sh).
d=/tmp

# --- 1. Crée le répertoire correction, ou erreur s'il existe déjà ---
if [ -e "$d/correction" ] ; then
    # Erreur sur stderr
    echo "error: '$d/correction' already exists" 1>&2
    # Statut 1
    exit 1
fi
# Crée le répertoire. Si mkdir échoue (droits...), || déclenche exit.
mkdir "$d/correction" || exit 2

# --- 2 et 3. Un répertoire par étudiant, et on y range son rendu ---
# Lit etudiants.txt ligne par ligne (redirection après done : pas de
# sous-processus, et chaque read lit la ligne suivante, diapo 97)
while read u ; do
    # Ignore les lignes vides
    if [ -z "$u" ] ; then
        # Passe à la ligne suivante
        continue
    fi
    # Crée /tmp/correction/xxxx
    mkdir "$d/correction/$u"
    # Le rendu existe-t-il ?
    if [ -f "$d/$u.tar.gz" ] ; then
        # Oui : on le déplace dans le répertoire de l'étudiant
        mv "$d/$u.tar.gz" "$d/correction/$u/"
    else
        # Non : message d'erreur avec le nom de l'étudiant
        echo "error: no submission for '$u'" 1>&2
    fi
done < "$d/etudiants.txt"

# --- 4. Rendus restants = fichiers qui ne devraient pas être là ---
# L'expansion "$d"/*.tar.gz liste les .tar.gz restants
for f in "$d"/*.tar.gz ; do
    # Si aucun fichier ne correspond, le motif est gardé tel quel
    # (cours diapo 46) : on vérifie donc que le fichier existe
    if [ ! -e "$f" ] ; then
        # Rien à signaler
        continue
    fi
    # Erreur avec le nom du fichier en trop
    echo "error: '$f' should not be here" 1>&2
done
