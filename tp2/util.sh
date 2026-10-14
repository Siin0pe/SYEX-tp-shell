# util.sh (exercice 8, examen 2017) : bibliothèque à sourcer avec ". util.sh"

# t str : renvoie le statut 0 si str est un entier, 1 sinon
# (c'est isInt de l'exercice 4, mais sans "local" et en regex basique)
t() {
    # Argument dans n. ATTENTION : n est GLOBALE (pas de local) !
    n="$1"
    # Ligne 1 : supprime "signe optionnel (\?) puis 1 chiffre ou plus (\+)"
    m=$(echo "$n" | sed -e '1s/[+-]\?[0-9]\+//')
    # Statut 0 si plus rien ne reste, donc si c'était un entier
    test -z "$m"
}

# f x : écrit x! (factorielle) sur stdout, de façon récursive
f() {
    # Argument (global lui aussi)
    x=$1
    # Cas de base : x <= 1, alors x! = 1
    if [ $x -le 1 ] ; then
        echo 1
    # Sinon x! = x * (x-1)!
    else
        # n = x - 1
        n=$((x-1))
        # $(f $n) lance f dans un SOUS-PROCESSUS : l'appel récursif ne peut
        # donc pas écraser x et n du niveau courant
        echo $(( x * $(f $n) ))
    fi
}
