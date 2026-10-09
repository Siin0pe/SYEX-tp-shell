# s3tool.sh : bibliothèque de fonctions, à SOURCER (pas à exécuter) :
#     . ~/OSSE11/bin/s3tool.sh      (seule syntaxe acceptée par sh)
# Sourcer = le shell courant lit ce fichier, les fonctions définies
# deviennent alors des commandes utilisables (cours diapos 55 et 81).
# Pas de #! : ce fichier n'est pas fait pour être lancé seul.


# usage: s36str a b c              => équivaut à s36str a a b b c c
# usage: s36str k1 s1 k2 s2 k3 s3
# Affiche les chaînes s(i) dans l'ordre des clés k(i).
# Les clés sont comparées de façon lexicographique (\> sur les chaînes).
s36str() {
    # Forme courte à 3 arguments : chaque chaîne est sa propre clé
    if [ $# -eq 3 ] ; then
        # Rappel récursif avec la forme à 6 arguments
        s36str "$1" "$1" "$2" "$2" "$3" "$3"
        # Fin de la fonction, statut 0 (ok)
        return 0
    fi
    # On teste les 6 ordres possibles des clés k1=$1, k2=$3, k3=$5.
    # "! a \> b" signifie "a <= b". -a = ET logique de test.
    # Cas k1 <= k2 <= k3
    if [ ! "$1" \> "$3" -a ! "$3" \> "$5" ] ; then
        echo "$2" ; echo "$4" ; echo "$6"
    # Cas k1 <= k3 <= k2
    elif [ ! "$1" \> "$5" -a ! "$5" \> "$3" ] ; then
        echo "$2" ; echo "$6" ; echo "$4"
    # Cas k2 <= k1 <= k3
    elif [ ! "$3" \> "$1" -a ! "$1" \> "$5" ] ; then
        echo "$4" ; echo "$2" ; echo "$6"
    # Cas k2 <= k3 <= k1
    elif [ ! "$3" \> "$5" -a ! "$5" \> "$1" ] ; then
        echo "$4" ; echo "$6" ; echo "$2"
    # Cas k3 <= k1 <= k2
    elif [ ! "$5" \> "$1" -a ! "$1" \> "$3" ] ; then
        echo "$6" ; echo "$2" ; echo "$4"
    # Dernier cas possible : k3 <= k2 <= k1
    else
        echo "$6" ; echo "$4" ; echo "$2"
    fi
}


# usage: s36int a b c              => équivaut à s36int a a b b c c
# usage: s36int k1 s1 k2 s2 k3 s3
# Comme s36str, mais les clés sont des ENTIERS comparés numériquement
# (-le = inférieur ou égal, cours diapo 70).
s36int() {
    # Forme courte : chaque entier est sa propre clé
    if [ $# -eq 3 ] ; then
        # Rappel avec la forme à 6 arguments
        s36int "$1" "$1" "$2" "$2" "$3" "$3"
        # Fin, statut ok
        return 0
    fi
    # Cas k1 <= k2 <= k3
    if [ "$1" -le "$3" -a "$3" -le "$5" ] ; then
        echo "$2" ; echo "$4" ; echo "$6"
    # Cas k1 <= k3 <= k2
    elif [ "$1" -le "$5" -a "$5" -le "$3" ] ; then
        echo "$2" ; echo "$6" ; echo "$4"
    # Cas k2 <= k1 <= k3
    elif [ "$3" -le "$1" -a "$1" -le "$5" ] ; then
        echo "$4" ; echo "$2" ; echo "$6"
    # Cas k2 <= k3 <= k1
    elif [ "$3" -le "$5" -a "$5" -le "$1" ] ; then
        echo "$4" ; echo "$6" ; echo "$2"
    # Cas k3 <= k1 <= k2
    elif [ "$5" -le "$1" -a "$1" -le "$3" ] ; then
        echo "$6" ; echo "$2" ; echo "$4"
    # Dernier cas : k3 <= k2 <= k1
    else
        echo "$6" ; echo "$4" ; echo "$2"
    fi
}


# usage: isInt str
# Renvoie le statut 0 si str représente un entier, 1 sinon.
isInt() {
    # Copie locale de l'argument (local : visible seulement dans la fonction)
    local n="$1"
    # sed -r (expressions régulières étendues) : sur la ligne 1, supprime la
    # première suite "signe optionnel + au moins un chiffre".
    # S'il reste quelque chose, ce n'était pas un entier.
    local m=$(echo "$n" | sed -r -e '1s/[+-]?[0-9]+//')
    # Statut de la fonction = statut de la dernière commande :
    # 0 si m est vide (entier), 1 sinon
    test -z "$m"
}
