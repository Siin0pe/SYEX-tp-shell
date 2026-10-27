# Entraînement shell : exercices type partiel (avec corrigés)

Exercices faits en séance (séries 1 et 2), puis variantes dans le style des partiels 2022-2026 (séries 3 à 6).
Les corrigés sont repliés : essaie d'abord, puis clique sur « Corrigé ». Tous les scripts et commandes des corrigés ont été testés sous Linux (dash et GNU coreutils).
Fiche des commandes : `cheatsheet_commandes.md`. Corrigés des vrais partiels : `2022/` à `2026/`.

**Les détails qui coûtent des points** (erreurs vues en séance) :
- oublier le **nom du fichier** (`grep absent` au lieu de `grep absent notes.txt`) ;
- ignorer une partie de la consigne (« **sans doublons** », « le **mot** », « **fichiers** » et pas « entrées ») ;
- `sed 'Np'` sans **`-n`** ;
- `ls` quand il faut `find` (fichiers cachés, `-type f`, `-maxdepth`) ;
- `"*.txt"` : un **`*` entre guillemets** n'est plus un joker ;
- `wc -l f` affiche aussi le nom → **`wc -l < f`** pour un calcul ;
- `$x=...` au lieu de **`x=...`** ;
- **tester le script** avant de rendre (`sh script.sh args`).

---

## Série 1 (faite en séance)

### 1. Commandes
**1.1** Afficher le nombre de lignes de `notes.txt` qui contiennent le mot `absent`.
**1.2** Afficher les 3 dernières lignes de `notes.txt`, triées par ordre alphabétique.
**1.3** Lister les fichiers réguliers `.log` de `/var/tmp` (et sous-répertoires) de plus de 1 Mo.
**1.4** Afficher le premier champ (séparateur `:`) de chaque ligne de `/etc/passwd`, sans doublons.

<details><summary>Corrigé</summary>

```sh
grep -c -w -e absent notes.txt                    # 1.1  -c : compte ; -w : le mot entier
tail -n 3 notes.txt | sort                        # 1.2  d'abord les 3 dernières, ensuite le tri
find /var/tmp -type f -name '*.log' -size +1M     # 1.3  M majuscule ; -print à la fin ou absent
cut -d : -f 1 /etc/passwd | sort -u               # 1.4  sort -u : trié sans doublons
```
Erreurs faites en séance : fichier oublié (1.1, 1.2), `'*.loc'`, `-print` placé **avant** `-size` (tout s'affiche, puisque find évalue de gauche à droite), `+1m` minuscule (1.3), « sans doublons » oublié (1.4).
</details>

### 2. Script à corriger : `backup.sh`
Consigne : au moins un argument. Chaque argument qui est un fichier régulier existant est copié en `<nom>.bak`. Pour les autres, un message d'erreur est affiché sur la sortie d'erreur et le script **continue**. À la fin, il affiche le nombre de fichiers sauvegardés.

```sh
1  #!/bin/sh
2
3  is_file() {
4      if [ -f $1 ] ; then
5          return 1
6      fi
7      return 0
8  }
9
10 if [ $# = 0 ]
11     echo "usage: $0 <file> [<file> ...]"
12 fi
13
14 $count = 0
15 for f in $* ; do
16     if [ is_file "$f" ] ; then
17         cp $f $f.bak
18         count=$(count+1)
19     else
20         echo 2>&1 "error: $f is not a regular file"
21         exit 1
22     fi
23 done
24
25 echo "$count file(s) saved"
```
**2.1** Expliquer `is_file`. **2.2** Lister les problèmes et les corriger.

<details><summary>Corrigé</summary>

**2.1** `[ -f $1 ]` teste si l'argument est un fichier régulier existant. Si oui, la fonction renvoie 1, sinon 0. En shell, **0 = vrai** : la fonction est donc vraie quand l'argument n'est **pas** un fichier, ce qui est l'**inverse** de son nom et de son usage ligne 16. De plus, `$1` n'est pas entre guillemets : un nom avec espace casse le test, et `[ -f ]` (argument vide) est toujours vrai.

**2.2**

| Ligne | Problème | Correctif |
|---|---|---|
| 4 | `$1` sans guillemets | `[ -f "$1" ]` |
| 5-7 | statut inversé | `return 0` si fichier, sinon `return 1` (ou le corps réduit à `[ -f "$1" ]`) |
| 10 | `; then` manquant (`=` marche, mais `-eq` est plus propre) | `if [ $# -eq 0 ] ; then` |
| 11 | usage sur stdout | `>&2` |
| 11-12 | pas d'`exit` | `exit 1` |
| 14 | `$count = 0` | `count=0` |
| 15 | `$*` | `"$@"` |
| 16 | `[ is_file ... ]` | `if is_file "$f" ; then` |
| 17 | variables sans guillemets | `cp "$f" "$f.bak"` |
| 18 | `$( )` lance une commande | `count=$((count+1))` |
| 20 | `2>&1` | `echo "..." >&2` |
| 21 | `exit 1` contredit « continue » | supprimer |

Remarque : la ligne 25 `echo "$count file(s) saved"` est **correcte**, car les guillemets doubles remplacent les variables.

```sh
#!/bin/sh
is_file() {
    [ -f "$1" ]
}

if [ $# -eq 0 ] ; then
    echo "usage: $0 <file> [<file> ...]" >&2
    exit 1
fi

count=0
for f in "$@" ; do
    if is_file "$f" ; then
        cp "$f" "$f.bak"
        count=$((count+1))
    else
        echo "error: $f is not a regular file" >&2
    fi
done

echo "$count file(s) saved"
```
</details>

---

## Série 2 (faite en séance)

### 1. Commandes
**1.1** Remplacer toutes les occurrences de `TODO` par `DONE` dans tous les fichiers `.sh` du répertoire courant et des sous-répertoires.
**1.2** Afficher le nombre de fichiers (cachés compris) directement dans `$HOME`, sans compter les sous-répertoires.
**1.3** Afficher les lignes de `log.txt` qui ne contiennent pas le mot `OK`.
**1.4** Afficher la 5ᵉ ligne de `data.txt`, et seulement elle.

<details><summary>Corrigé</summary>

```sh
find . -type f -name '*.sh' -exec sed -i -e 's/TODO/DONE/g' {} \;   # 1.1  -i : en place ; g : toutes
find "$HOME" -mindepth 1 -maxdepth 1 -type f | wc -l                # 1.2
grep -v -w OK log.txt                                               # 1.3  (ou sed -e '/OK/d' log.txt)
sed -n '5p' data.txt                                                # 1.4  (ou head -n 5 data.txt | tail -n 1)
```
Erreurs faites en séance : `s/TODO/DONE` sans `/g` et sans `-i` (1.1) ; `ls -a $HOME | wc -l` compte `.`, `..` et les répertoires (1.2) ; mauvais mot (1.3) ; `sed '5p'` sans `-n` affiche tout le fichier avec la ligne 5 en double (1.4).
</details>

### 2. Script à écrire : `compte.sh dir ext`
Exactement deux arguments. Si `dir` n'est pas un répertoire : erreur sur stderr, statut 1. Pour chaque fichier régulier directement dans `dir` avec l'extension `ext`, afficher `nom: nb_lignes`, puis `total: N`.
```
sh$ sh compte.sh ~/cours txt
notes.txt: 42
todo.txt: 3
total: 45
```

<details><summary>Corrigé</summary>

```sh
#!/bin/sh
if [ $# -ne 2 ] ; then
    echo "usage: $0 <dir> <ext>" >&2
    exit 1
fi
if [ ! -d "$1" ] ; then
    echo "$0: '$1' is not a directory" >&2
    exit 1
fi

count=0
for f in "$1"/*."$2" ; do
    [ -f "$f" ] || continue          # ignore les répertoires, et le motif resté littéral s'il n'y a aucun fichier
    c=$(wc -l < "$f")                # < : le nombre seul, sans le nom
    echo "$(basename "$f"): $c"
    count=$((count + c))
done
echo "total: $count"
```
Erreurs faites en séance : `-ne 3` (`$#` ne compte pas `$0`) ; `![` sans espace ; `"*.$2"` (le `*` entre guillemets reste littéral, et on ne cherchait pas dans `$1`) ; pas de test `-f` ; `wc -l $f` (affiche aussi le nom) ; `$count=` ; format de sortie sans `basename` ni `:`.
</details>

---

## Série 3 : commandes erronées (style 2025)

Chaque commande contient une ou plusieurs erreurs. Les lister et corriger.

**3.1** Nombre de fichiers `.c` directement dans le répertoire courant : `ls *.c | wc -c`
**3.2** Trier `/etc/passwd` par UID (3ᵉ champ, numérique) : `sort -t : -k 3 /etc/passwd`
**3.3** Supprimer les lignes vides de `f.txt` (en modifiant le fichier) : `sed -e '/^$/p' f.txt > f.txt`
**3.4** Afficher `pair` si `$n` est pair : `if [ $n % 2 = 0 ] ; then echo pair ; fi`
**3.5** Les 3 lignes les plus fréquentes de `mots.txt` : `sort mots.txt | uniq | sort -n | head 3`

<details><summary>Corrigé</summary>

```sh
find . -maxdepth 1 -type f -name '*.c' | wc -l     # 3.1 wc -c compte les OCTETS ; ls *.c échoue s'il n'y en a aucun et inclut les répertoires .c
sort -t : -k 3 -n /etc/passwd                     # 3.2 sans -n : tri alphabétique (1000 avant 2)
sed -i -e '/^$/d' f.txt                           # 3.3 p → d ; "> f.txt" VIDE le fichier avant la lecture : -i
if [ $((n % 2)) -eq 0 ] ; then echo pair ; fi     # 3.4 [ ] ne calcule pas : $(( )) ; nombres : -eq
sort mots.txt | uniq -c | sort -rn | head -n 3    # 3.5 uniq -c pour compter ; -r : décroissant ; head -n 3
```
</details>

---

## Série 4 : expliquer des commandes sed (style 2022)

Donner la sortie de chaque commande sur l'entrée indiquée, et dire en une phrase ce qu'elle fait.

**4.1** `sed -e 's/^\([^:]*\):.*/\1/'` sur `root:x:0:0:root:/root:/bin/bash`
**4.2** `sed -e 's/[0-9]\+/N/g'` sur `TP3 rendu le 12/01 a 8h30`
**4.3** `sed -n '/^#/!p'` sur les trois lignes `#!/bin/sh`, `# commentaire`, `echo ok`
**4.4** `sed -e 's/\(.*\)\.\(.*\)/\2.\1/'` sur `archive.tar.gz`

<details><summary>Corrigé</summary>

| | Sortie | Ce que ça fait |
|---|---|---|
| 4.1 | `root` | `[^:]*` = tout sauf `:` : garde le **1er champ** (comme `cut -d : -f 1`) |
| 4.2 | `TPN rendu le N/N a NhN` | remplace chaque **suite** de chiffres par `N` (`\+` = 1 ou plus, `g` = toutes) |
| 4.3 | `echo ok` | `!p` = afficher les lignes qui **ne** commencent **pas** par `#` (comme `grep -v '^#'`) |
| 4.4 | `gz.archive.tar` | `.*` est **gourmand** : le 1er groupe prend tout jusqu'au **dernier** point. Les deux parties sont échangées |
</details>

---

## Série 5 : script à corriger (style 2023, 2024, 2026)

Le script `count_ext.sh` doit prendre au moins un argument : une extension `ext` suivie de noms de fichiers. Il affiche combien de ces fichiers ont l'extension `ext`.
```
sh$ sh count_ext.sh c main.c util.h test.c
found 2 files with extension c
```

```sh
1  #/bin/sh
2
3  has_ext() {
4      ext=$(echo $1 | sed -e 's/.*\.//')
5      if [ $ext == $2 ] ; then
6          return 1
7      fi
8      return 0
9  }
10
11 if [ $# -lt 1 ] then
12     echo 2>&1 "usage: $0 <ext> [<file> ...]"
13 fi
14
15 ext = $1
16 shift
17 n=0
18 for file in $@ do
19     if [ has_ext $file $ext ] ; then
20         n=$(n+1)
21     fi
22 done
23 echo 'found $n files with extension $ext'
```
**5.1** Expliquer en détail `has_ext`. **5.2** Lister les problèmes et les corriger.

<details><summary>Corrigé</summary>

**5.1** `echo $1 | sed -e 's/.*\.//'` : `.*\.` est gourmand et va jusqu'au **dernier** point, qui est supprimé avec tout ce qui précède. Il reste l'**extension** (`main.c` donne `c`, `a.tar.gz` donne `gz`). S'il n'y a pas de point, rien n'est remplacé et on obtient le nom entier. La fonction compare cette extension à `$2` et renvoie **1** si elles sont égales, 0 sinon. Elle est donc vraie (0) quand le fichier n'a **pas** la bonne extension, ce qui est **inversé**. Piège supplémentaire : elle écrit dans la variable **globale** `ext`, celle du script, et écrase l'extension recherchée avec celle du fichier testé.

**5.2**

| Ligne | Problème | Correctif |
|---|---|---|
| 1 | `#/bin/sh` : il manque le `!` | `#!/bin/sh` |
| 4 | variable globale `ext` écrasée (même nom que ligne 15) | autre nom (`file_ext`) ou `local` |
| 4 | `echo $1` sans guillemets | `printf '%s\n' "$1"` |
| 5 | `==` pas POSIX ; variables sans guillemets | `[ "$file_ext" = "$2" ]` |
| 5-8 | statut inversé | `return 0` si égales, sinon `return 1` |
| 11 | `; then` manquant | `if [ $# -lt 1 ] ; then` |
| 12 | `2>&1` | `>&2` |
| 12-13 | pas d'`exit` | `exit 1` |
| 15 | `ext = $1` (espaces) | `ext="$1"` |
| 18 | `; do` manquant ; `$@` sans guillemets | `for file in "$@" ; do` |
| 19 | `[ has_ext ... ]` | `if has_ext "$file" "$ext" ; then` |
| 20 | `$(n+1)` | `n=$((n+1))` |
| 23 | quotes simples : `$n` et `$ext` pas remplacés | guillemets doubles |

```sh
#!/bin/sh
has_ext() {
    file_ext=$(printf '%s\n' "$1" | sed -e 's/.*\.//')
    [ "$file_ext" = "$2" ]
}

if [ $# -lt 1 ] ; then
    echo "usage: $0 <ext> [<file> ...]" >&2
    exit 1
fi

ext="$1"
shift
n=0
for file in "$@" ; do
    if has_ext "$file" "$ext" ; then
        n=$((n+1))
    fi
done
echo "found $n files with extension $ext"
```
</details>

---

## Série 6 : scripts à écrire (style 2022, 2023, 2025)

**6.1** `cherche.sh mot fichier...` : au moins 2 arguments. Pour chaque fichier : s'il n'est pas un fichier régulier lisible, erreur sur stderr et on **continue**. Sinon, afficher `fichier: N`, où N est le nombre de lignes contenant le **mot** `mot`. Statut final 1 si au moins une erreur, 0 sinon.

**6.2** `renomme.sh dir old new` : renomme chaque fichier régulier `*.old` directement dans `dir` en `*.new` (`a.old` devient `a.new`), en affichant `a.old -> a.new`. À la fin : `N fichier(s) renommé(s)`.

**6.3** `plus_long.sh fichier...` : affiche le nom du fichier qui a le plus de lignes, et son nombre de lignes (`fichier: N`). Les arguments qui ne sont pas des fichiers réguliers sont ignorés.

<details><summary>Corrigé 6.1</summary>

```sh
#!/bin/sh
if [ $# -lt 2 ] ; then
    echo "usage: $0 <mot> <fichier> [<fichier> ...]" >&2
    exit 1
fi

mot="$1"
shift
statut=0
for f in "$@" ; do
    if [ ! -f "$f" ] || [ ! -r "$f" ] ; then
        echo "$0: cannot read '$f'" >&2
        statut=1
        continue
    fi
    # -w : le mot entier ; -F : chaîne fixe, pas une regex ; -e : mot peut commencer par -
    echo "$f: $(grep -c -w -F -e "$mot" "$f")"
done
exit $statut
```
</details>

<details><summary>Corrigé 6.2</summary>

```sh
#!/bin/sh
if [ $# -ne 3 ] ; then
    echo "usage: $0 <dir> <old> <new>" >&2
    exit 1
fi
if [ ! -d "$1" ] ; then
    echo "$0: '$1' is not a directory" >&2
    exit 1
fi

n=0
for f in "$1"/*."$2" ; do
    [ -f "$f" ] || continue
    base=$(basename "$f" ".$2")      # a.old sans le suffixe .old → a
    mv "$f" "$1/$base.$3"
    echo "$base.$2 -> $base.$3"
    n=$((n+1))
done
echo "$n fichier(s) renommé(s)"
```
</details>

<details><summary>Corrigé 6.3</summary>

```sh
#!/bin/sh
max=-1
nom=""
for f in "$@" ; do
    [ -f "$f" ] || continue
    c=$(wc -l < "$f")
    if [ "$c" -gt "$max" ] ; then
        max=$c
        nom="$f"
    fi
done

if [ -z "$nom" ] ; then
    echo "$0: aucun fichier régulier" >&2
    exit 1
fi
echo "$nom: $max"
```
`max=-1` permet de garder aussi un fichier vide (0 ligne) s'il est le seul.
</details>
