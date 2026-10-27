# Cheatsheet shell : une section par commande

Pour chaque commande : son **arborescence** (comment elle se construit), ses **options utiles**, puis des **exemples expliqués**.
Recherche conseillée : Ctrl+Shift+F sur le nom de la commande, ou sur `## sed`, `## find`, etc.
Les notations : `f` = un fichier, `dir` = un répertoire, `motif` = une regex, `[ ]` dans une arborescence = facultatif.

Sommaire : [Rappels](#rappels-qui-servent-partout) · [find](#find) · [grep](#grep) · [sed](#sed) · [cut](#cut) · [head](#head) · [tail](#tail) · [wc](#wc) · [sort](#sort) · [uniq](#uniq) · [tr](#tr) · [stat](#stat) · [ls](#ls) · [cat](#cat) · [echo / printf](#echo--printf) · [cp / mv / rm / mkdir / rmdir](#cp--mv--rm--mkdir--rmdir) · [chmod](#chmod) · [basename / dirname](#basename--dirname) · [seq](#seq) · [tee](#tee) · [test / \[ \]](#test---) · [Calcul : $(( )) et expr](#calcul---et-expr) · [read](#read) · [Script sh : structures](#script-sh--les-structures) · [Checklist script à corriger](#checklist--script-à-corriger)

---

## Rappels qui servent partout

```
commande [options] [arguments]  [redirections]  [| commande suivante ...]
│        │         │             │
│        │         │             ├── > f     stdout dans f (écrase, TRONQUE f avant de lancer la commande)
│        │         │             ├── >> f    stdout à la fin de f
│        │         │             ├── < f     stdin lu depuis f
│        │         │             ├── 2> f    stderr dans f
│        │         │             ├── >&2     stdout vers stderr (= 1>&2) → pour les messages d'erreur
│        │         │             ├── 2>&1    stderr vers stdout (PAS pour afficher une erreur !)
│        │         │             └── 2>/dev/null   jeter les erreurs
│        │         └── fichiers, chaînes... séparés par des blancs
│        └── -x, -xyz (options courtes groupées), --long
└── nom de commande (cherché dans PATH) ou chemin (./script.sh)
```

| Syntaxe | Rôle |
|---|---|
| `a \| b` | stdout de `a` devient stdin de `b` |
| `a ; b` | `a` puis `b`, quoi qu'il arrive |
| `a && b` | `b` seulement si `a` réussit (statut 0) |
| `a \|\| b` | `b` seulement si `a` échoue |
| `$(cmd)` | remplacé par la sortie de `cmd` |
| `$((1+2))` | calcul arithmétique |
| `"..."` | garde les espaces, **remplace** `$var` et `$(...)` |
| `'...'` | tout est littéral, **aucun** remplacement |
| `$?` | statut de la dernière commande (0 = succès = vrai) |

Si une commande prend un fichier, **ne pas l'oublier** : sans fichier, elle lit stdin (le clavier) et semble bloquée.

---

## find

Cherche des fichiers **récursivement** dans une arborescence.

```
find  [chemin...]  [options de profondeur]  [tests...]  [action]
│     │            │                         │           │
│     │            │                         │           ├── -print        afficher (action par défaut)
│     │            │                         │           ├── -exec cmd {} \;   lancer cmd une fois PAR fichier ({} = le fichier)
│     │            │                         │           ├── -exec cmd {} +    lancer cmd avec PLUSIEURS fichiers d'un coup
│     │            │                         │           └── -delete       supprimer
│     │            │                         │
│     │            │                         ├── -name 'motif'   nom (motif shell : * ? [ch]) → TOUJOURS entre quotes
│     │            │                         ├── -iname 'motif'  idem, sans tenir compte de la casse
│     │            │                         ├── -type f / d / l fichier régulier / répertoire / lien
│     │            │                         ├── -size +1M / -10k / +100c   plus de 1 Mo / moins de 10 Ko / plus de 100 octets
│     │            │                         ├── -mtime -7       modifié il y a moins de 7 jours
│     │            │                         ├── -user alice     appartient à alice
│     │            │                         ├── -empty          vide
│     │            │                         ├── ! test          négation
│     │            │                         ├── test1 test2     ET implicite
│     │            │                         └── \( t1 -o t2 \)  OU (parenthèses échappées)
│     │            │
│     │            ├── -maxdepth N   ne pas descendre au-delà de N niveaux (1 = directement dans le dossier)
│     │            └── -mindepth N   ignorer les N premiers niveaux (1 = exclure le dossier lui-même)
│     └── où chercher (. si absent)
└── find
```

**Les expressions sont évaluées de GAUCHE À DROITE** : une action placée avant un test s'exécute avant le test.
Les unités de `-size` : `c` (octets), `k`, **`M`** (majuscule), `G`. Les options `-maxdepth` et `-mindepth` se placent juste après le chemin.

```sh
find . -name '*.c'                          # tous les .c sous le répertoire courant (récursif)
find /tmp/src -type f -name '*.cpp'         # seulement les fichiers réguliers .cpp
find /tmp/src -type f -name '*.cpp' | wc -l # leur NOMBRE
find "$dir" -maxdepth 1 -type f -name '*.txt'   # directement dans $dir, pas dans les sous-dossiers
find ~ -mindepth 1 -maxdepth 1 -type d | wc -l  # nombre de sous-répertoires directs du HOME (cachés compris)
find /var/tmp -type f -name '*.log' -size +1M   # .log de plus d'1 Mo
find . -type f \( -name '*.c' -o -name '*.h' \) # .c OU .h
find . -type f ! -name '*.o'                # tout sauf les .o
find . -name '*.sh' -exec grep -H TODO {} +     # lignes contenant TODO, avec le nom du fichier (-H)
find . -type f -name '*.sh' -exec sed -i -e 's/TODO/DONE/g' {} \;   # remplacement dans chaque fichier
find /tmp -maxdepth 1 -exec stat -c '%U' {} + | sort -u              # propriétaires des fichiers de /tmp
find . -name '*.tmp' -delete                # supprimer les .tmp
```

Pièges : `-name *.c` sans quotes (le shell développe `*.c` avant find) ; `-print` placé avant les tests ; `-size +1m` (le `m` minuscule n'existe pas) ; `-mindepth` confondu avec `-maxdepth`.

---

## grep

Affiche les **lignes** qui contiennent un motif.

```
grep  [options]  [-e] motif  [fichier...]
│     │               │       └── sans fichier : lit stdin
│     │               └── regex (basique). -e utile si le motif commence par -
│     ├── -v   lignes qui NE correspondent PAS
│     ├── -c   NOMBRE de lignes qui correspondent (au lieu des lignes)
│     ├── -l   NOMS des fichiers qui contiennent le motif (chacun une fois)
│     ├── -n   numéro de ligne devant chaque ligne
│     ├── -i   ignorer majuscules/minuscules
│     ├── -w   le motif comme MOT entier (absent ≠ absente)
│     ├── -x   la ligne ENTIÈRE doit correspondre
│     ├── -F   motif = chaîne fixe (. * [ ne sont plus spéciaux)
│     ├── -E   regex étendue (+ ? | ( ) sans backslash)
│     ├── -o   n'afficher que la partie qui correspond
│     ├── -r   récursif dans les répertoires
│     ├── -H   afficher le nom du fichier devant chaque ligne
│     └── -q   rien afficher : seulement le statut (0 = trouvé) → pour les if
└── grep
```

Regex de base : `^` début de ligne, `$` fin, `.` un caractère, `*` 0 ou plus, `[abc]` un des caractères, `[^abc]` aucun d'eux, `\+` 1 ou plus (GNU), `\.` un point littéral.

```sh
grep -w absent notes.txt            # lignes contenant le mot absent
grep -c -w absent notes.txt         # leur nombre
grep -v OK log.txt                  # lignes SANS OK
grep -n '^#' script.sh              # lignes commençant par #, avec leur numéro
grep -i erreur log.txt              # Erreur, ERREUR, erreur...
grep -F '<stdio.h>' prog.c          # chaîne littérale (le . n'est pas spécial)
grep -l Bob /tmp/* 2>/dev/null | wc -l   # NOMBRE de fichiers contenant Bob
grep -r -l TODO .                   # fichiers contenant TODO, récursivement
grep -E '^(abc|def)' f              # lignes commençant par abc ou def
grep -x q f                         # lignes valant exactement q
if grep -q -e TODO readme.md ; then echo 'trouvé' ; fi   # dans un test : -q, et PAS de [ ]
```

---

## sed

Éditeur de flux : applique des commandes à chaque ligne et affiche le résultat.

```
sed  [options]  -e 'script'  [fichier...]
│    │               │
│    │               ├── s/motif/remplacement/[drapeaux]   substitution
│    │               │   ├── g    toutes les occurrences de la ligne (sinon : la première)
│    │               │   ├── p    afficher la ligne si une substitution a eu lieu
│    │               │   └── remplacement : \1 = 1er groupe \( \) capturé, & = tout ce qui correspond
│    │               ├── [adresse]d   supprimer la ligne
│    │               ├── [adresse]p   afficher la ligne (avec -n)
│    │               └── adresse :
│    │                   ├── 5        la ligne 5
│    │                   ├── 2,4      les lignes 2 à 4
│    │                   ├── $        la dernière ligne
│    │                   └── /motif/  les lignes qui correspondent
│    ├── -n   ne PAS afficher automatiquement chaque ligne (on choisit avec p)
│    ├── -i   modifier le fichier EN PLACE (sinon : résultat à l'écran, fichier intact)
│    └── -E   regex étendue (( ) + ? | sans backslash)
└── sed
```

```sh
sed -e 's/Bob/Alice/' f              # 1re occurrence de chaque ligne, affichée à l'écran
sed -e 's/Bob/Alice/g' f             # toutes les occurrences
sed -i -e 's/Bob/Alice/g' f          # idem, en modifiant f
sed -e 's/<stdio\.h>/<cstdio>/g' f   # \. = un vrai point
sed -e 's/x//g' f                    # supprimer tous les x
sed -n '5p' f                        # seulement la ligne 5
sed -n '2,4p' f                      # lignes 2 à 4
sed -n '/erreur/p' f                 # comme grep erreur f
sed -e '1d' f                        # tout sauf la 1re ligne
sed -e '/^#/d' f                     # supprimer les lignes de commentaire
sed -e 's/^/> /' f                   # ajouter "> " en début de chaque ligne
sed -e 's/\(.\).*/\1/'               # garder seulement le 1er caractère de la ligne
sed -e 's/\([A-Z][a-z]\+\).*/\1/'    # couper après le 1er mot qui commence par une majuscule
sed -e 's/:/ /g' /etc/passwd         # remplacer les : par des espaces
```

Piège : `sed ... f > f` **vide** f, car le shell tronque f avant que sed ne le lise. Utiliser `sed -i`.
Le séparateur peut changer : `s|/usr|/opt|g`, pratique quand le motif contient des `/`.

---

## cut

Extrait des **colonnes** de chaque ligne.

```
cut  -d sep -f champs  [fichier...]      par champs (séparateur sep, TAB par défaut)
cut  -c positions      [fichier...]      par caractères
     │
     └── champs/positions :
         ├── 3       le 3e
         ├── 1,4     le 1er et le 4e
         ├── 2-5     du 2e au 5e
         └── 3-      du 3e jusqu'à la fin
```

```sh
cut -d : -f 1 /etc/passwd            # noms des utilisateurs
cut -d : -f 1,6 /etc/passwd          # nom et HOME
cut -d , -f 3 data.csv               # 3e champ d'un CSV
cut -d ' ' -f 2 edt.txt              # 2e colonne, séparateur espace
cut -c 1-10 f                        # 10 premiers caractères de chaque ligne
head -n 10 edt.txt | cut -d ' ' -f 2 # 2e colonne des 10 premières lignes
```

Piège : avec `-d ' '`, deux espaces de suite font un champ vide. `tr -s ' '` les réduit avant.

---

## head

Affiche le **début** d'un fichier.

```
head  [-n N]  [fichier...]
      ├── -n N     les N premières lignes (10 par défaut)
      ├── -n -N    tout SAUF les N dernières lignes (GNU)
      └── -c N     les N premiers octets
```

```sh
head -n 5 projet.c                   # 5 premières lignes
head -n 1 f                          # 1re ligne
head -n 3 f | tail -n 1              # la 3e ligne seule
head -n 7 f | tail -n 5              # lignes 3 à 7
head -n 5 "$file" | grep -q mot      # mot présent dans les 5 premières lignes ?
```

---

## tail

Affiche la **fin** d'un fichier.

```
tail  [-n N]  [fichier...]
      ├── -n N     les N dernières lignes (10 par défaut)
      ├── -n +N    à partir de la ligne N jusqu'à la fin
      ├── -c N     les N derniers octets
      └── -f       suivre le fichier en continu (logs)
```

```sh
tail -n 3 notes.txt                  # 3 dernières lignes
tail -n 3 notes.txt | sort           # 3 dernières lignes, triées
tail -n +2 data.csv                  # tout sauf l'en-tête (1re ligne)
head -n 5 data.txt | tail -n 1       # la 5e ligne seule (ou : sed -n '5p' data.txt)
```

---

## wc

Compte lignes, mots, octets.

```
wc  [options]  [fichier...]
    ├── -l   lignes (en fait : nombre de '\n')
    ├── -w   mots
    ├── -c   octets
    └── -m   caractères (≠ octets en UTF-8 accentué)
```

```sh
wc -l f                              # "42 f"  : nombre ET nom
wc -l < f                            # "42"    : nombre seul (< : wc ne connaît pas le nom)
wc -c < edt.txt                      # taille en octets
ls | wc -l                           # nombre d'entrées du répertoire (sans les cachés)
n=$(grep -c x f)                     # pour compter des lignes qui correspondent, grep -c suffit
```

Piège : `wc -l` compte les **lignes**, pas les caractères (1.1 de 2025).

---

## sort

Trie les lignes.

```
sort  [options]  [fichier...]
      ├── -n       tri NUMÉRIQUE (sinon 10 < 9, car tri alphabétique)
      ├── -r       ordre inverse
      ├── -u       sans doublons
      ├── -t sep   séparateur de champs
      ├── -k N     trier selon le champ N
      └── -f       ignorer la casse
```

```sh
sort f                               # ordre alphabétique
sort -n nombres.txt                  # ordre numérique
sort -rn nombres.txt                 # numérique décroissant
sort -u f                            # trié, sans doublons
sort -t : -k 3 -n /etc/passwd        # trié par UID (3e champ)
cut -d : -f 1 /etc/passwd | sort -u  # utilisateurs sans doublons
du -s * | sort -rn | head -n 3       # les 3 plus gros éléments
```

---

## uniq

Supprime les doublons **consécutifs**, d'où l'habitude de trier avant.

```
uniq  [options]  [fichier]
      ├── -c   préfixer chaque ligne par son nombre d'occurrences
      ├── -d   seulement les lignes en double
      └── -u   seulement les lignes uniques
```

```sh
sort f | uniq                        # = sort -u f
sort f | uniq -c                     # compte chaque ligne
sort f | uniq -c | sort -rn          # les lignes les plus fréquentes en premier
for i in $(seq 100) ; do ./prog ; done | sort | uniq -c   # fréquence de chaque sortie d'un programme
```

---

## tr

Remplace ou supprime des **caractères**. Il lit **seulement stdin**, jamais de nom de fichier.

```
tr  [options]  ensemble1  [ensemble2]   < fichier
    ├── tr 'abc' 'xyz'   a→x, b→y, c→z
    ├── -d 'abc'         supprimer ces caractères
    └── -s ' '           réduire les répétitions à une seule
```

```sh
tr 'a-z' 'A-Z' < f                   # en majuscules
tr -d 'x' < f                        # supprimer les x
tr -s ' ' < f                        # plusieurs espaces → un seul
tr ':' '\n' <<< "$PATH"              # (bash) un dossier du PATH par ligne
echo "$PATH" | tr ':' '\n'           # idem en sh
```

---

## stat

Informations sur un fichier.

```
stat  -c 'format'  fichier...
         ├── %s   taille en octets
         ├── %U   nom du propriétaire
         ├── %n   nom du fichier
         ├── %A   droits (-rwxr-xr-x)
         └── %y   date de modification
```

```sh
stat -c '%s' f                       # taille
stat -c '%U' f                       # propriétaire
stat -c '%U' /tmp/* | sort -u        # propriétaires des fichiers de /tmp, sans doublons
stat -c '%s %n' *.c                  # taille et nom de chaque .c
```

---

## ls

```
ls  [options]  [fichier ou répertoire...]
    ├── -l   format long (droits, propriétaire, taille, date)
    ├── -a   inclure les fichiers cachés (.xxx)
    ├── -d   lister le répertoire lui-même, pas son contenu
    ├── -R   récursif
    ├── -t   trier par date (récent d'abord)
    ├── -S   trier par taille
    ├── -r   ordre inverse
    └── -1   un nom par ligne
```

```sh
ls -la ~                             # tout le HOME, cachés compris, en détail
ls -d /tmp/*.[ch]                    # fichiers .h ou .c de /tmp
ls -l | grep '^d'                    # seulement les répertoires (ligne commençant par d)
ls -t | head -n 1                    # le fichier le plus récent
```

Pour compter ou traiter des fichiers dans un script, préférer `find` : `ls` ne voit pas les cachés, et ses sorties sont faites pour être lues par un humain.

---

## cat

```
cat  [fichier...]       concatène et affiche ; sans fichier (ou avec -) : lit stdin
     ├── -n   numéroter les lignes
     └── -A   montrer les caractères invisibles (TAB = ^I, fin de ligne = $)
```

```sh
cat f1 f2 > f3                       # f3 = f1 suivi de f2
cat >> f                             # ajoute ce qu'on tape à f (fin : Ctrl-D)
cat -n script.sh                     # avec numéros de ligne
```

---

## echo / printf

```
echo  [-n]  texte...          affiche le texte + retour à la ligne (-n : sans retour)
printf  'format'  args...     affichage formaté, PAS de retour à la ligne automatique
        ├── %s   chaîne
        ├── %d   entier
        ├── \n   retour à la ligne
        └── \t   tabulation
```

```sh
echo "Bonjour $USER"                 # guillemets doubles : $USER est remplacé
echo 'Bonjour $USER'                 # quotes simples : affiche littéralement $USER
echo "error: fichier absent" >&2     # message d'erreur → sur la SORTIE D'ERREUR
printf '%s\n' "$f"                   # afficher une variable sans surprise (même si elle vaut -n)
printf '%d\t%s\n' 3 ligne            # "3<TAB>ligne"
printf '%s' "$1" | sed -e 's/\(.\).*/\1/'   # 1er caractère de $1
```

---

## cp / mv / rm / mkdir / rmdir

```
cp  [-r] [-i]  source... destination      copier (-r : répertoires ; -i : demander avant d'écraser)
mv  [-i]       source... destination      déplacer / renommer
rm  [-r] [-f] [-i]  fichier...            supprimer (-r : récursif ; -f : sans erreur ni question)
mkdir  [-p]  répertoire...                créer (-p : avec les parents, sans erreur si existe)
rmdir  répertoire...                      supprimer un répertoire VIDE (échoue sinon)
```

```sh
cp "$f" "$f.bak"                     # sauvegarde (guillemets : noms avec espaces)
mv tmp file.txt                      # renommer
mkdir -p ~/sys/tp1                   # crée ~/sys puis ~/sys/tp1
rmdir /tmp/log                       # supprime /tmp/log seulement s'il est vide
rm -r dossier                        # supprime dossier et son contenu
```

---

## chmod

```
chmod  droits  fichier...
       ├── u+x / g-w / o+r / a+r    u=propriétaire g=groupe o=autres a=tous ; + ajoute, - retire
       └── 755 / 644               en octal : r=4 w=2 x=1, un chiffre pour u, g, o
```

```sh
chmod u+x script.sh                  # rendre exécutable pour soi → ./script.sh
chmod 644 f                          # rw-r--r--
chmod 755 script.sh                  # rwxr-xr-x
```

---

## basename / dirname

```sh
basename /home/a/notes.txt           # notes.txt
basename /home/a/notes.txt .txt      # notes   (retire aussi le suffixe)
dirname /home/a/notes.txt            # /home/a
dir=$(dirname "$0")                  # répertoire du script en cours
```

---

## seq

```sh
seq 5                                # 1 2 3 4 5 (un par ligne)
seq 1 9                              # 1 à 9
seq 0 2 10                           # 0 2 4 6 8 10 (pas de 2)
for i in $(seq 1 9) ; do echo $i ; done
```

---

## tee

Écrit stdin **à la fois** à l'écran et dans un fichier.

```sh
ls | tee liste.txt                   # affiche ET enregistre
cmd | tee -a log.txt                 # -a : ajoute à la fin
```

---

## test / [ ]

`[` est une **commande** (= `test`) : il faut des espaces partout, et le `]` final. Elle renvoie 0 (vrai) ou 1 (faux).

```
[  expression  ]
   ├── fichiers
   │   ├── -e f   existe              ├── -r f   lisible
   │   ├── -f f   fichier régulier    ├── -w f   modifiable
   │   ├── -d f   répertoire          ├── -x f   exécutable
   │   └── -s f   existe et non vide
   ├── chaînes (toujours entre guillemets)
   │   ├── -z "$s"         vide
   │   ├── -n "$s"         non vide
   │   ├── "$a" = "$b"     égales   (= et PAS == en sh)
   │   └── "$a" != "$b"    différentes
   ├── nombres
   │   └── $a -eq $b   (-ne -lt -le -gt -ge)   (PAS < > : ce sont des redirections)
   └── combinaisons
       ├── ! expr                     négation
       ├── [ e1 ] && [ e2 ]           ET  (recommandé ; -a existe mais est obsolète)
       └── [ e1 ] || [ e2 ]           OU  (recommandé ; -o existe mais est obsolète)
```

```sh
[ $# -lt 2 ]                         # moins de 2 arguments
[ -d "$1" ]                          # $1 est un répertoire
[ ! -f "$f" ]                        # $f n'est PAS un fichier régulier
[ "$skip" = "no" ]                   # comparaison de chaînes
[ -f "$f" ] && [ -r "$f" ]           # fichier régulier ET lisible
if grep -q x f ; then ...            # une COMMANDE se teste directement, sans [ ]
if ! ma_fonction "$x" ; then ...     # idem pour une fonction
```

`[[ ]]` est du **bash**, pas du sh. Dans un script `#!/bin/sh`, on n'utilise que `[ ]`.

---

## Calcul : $(( )) et expr

```sh
i=$((i+1))                           # incrément (PAS $(i+1), qui lance une commande "i+1")
n=$(( $1 * 2 ))                      # opérateurs : + - * / % (entiers seulement)
echo $((7 / 2))                      # 3 (division entière)
echo $((7 % 2))                      # 1 (reste)
expr 3 + 4                           # 7 : ancienne méthode, espaces OBLIGATOIRES
expr 3 \* 4                          # le * doit être échappé
```

---

## read

Lit **une ligne** de stdin dans une ou plusieurs variables.

```
read  [-r]  var1 [var2 ...]
      ├── -r            ne pas interpréter les \ (toujours le mettre)
      ├── IFS= read     garder les espaces en début et fin de ligne
      ├── var = un NOM de variable (read f, PAS read $f)
      └── statut ≠ 0 en fin de fichier → arrête un while
```

```sh
read -r nom                          # lit une ligne au clavier dans $nom
while IFS= read -r ligne ; do        # lit un fichier ligne par ligne
    echo "got: $ligne"
done < f
find . -name '*.txt' | while IFS= read -r file ; do   # traite chaque fichier trouvé
    wc -l < "$file"
done
read -r a b reste <<EOF              # découpe aux blancs : a=1 b=2 reste="3 4"
1 2 3 4
EOF
```

Le `while` placé à droite d'un `|` tourne dans un **sous-shell** : les variables qu'il modifie sont perdues après le `done`.

---

## Script sh : les structures

```sh
#!/bin/sh                            # 1re ligne : chemin ABSOLU de l'interpréteur

# --- Arguments ---
# $0 nom du script   $1 $2 ... arguments   $# leur nombre
# "$@" tous les arguments, chacun intact (TOUJOURS entre guillemets)
# $$ PID du shell (PAS les arguments !)   $? statut de la dernière commande
# shift : enlève $1, décale tout ($2 devient $1)

# --- Variables ---
nom="valeur"                         # PAS d'espace autour de =, PAS de $ à gauche
echo "$nom"                          # $ pour LIRE la valeur, entre guillemets
res=$(head -n 1 "$f")                # résultat d'une commande
count=$((count + 1))                 # calcul

# --- Vérifier les arguments ---
if [ $# -lt 1 ] ; then
    echo "usage: $0 <file> [<file> ...]" >&2
    exit 1
fi

# --- if ---
if [ -f "$f" ] ; then
    echo "fichier"
elif [ -d "$f" ] ; then
    echo "répertoire"
else
    echo "autre"
fi

# --- for ---
for f in "$@" ; do                   # sur les arguments
    echo "$f"
done
for f in *.txt ; do                  # sur des fichiers
    echo "$f"
done
for i in $(seq 1 5) ; do echo $i ; done

# --- while ---
i=1
while [ $i -lt 10 ] ; do
    echo $i
    i=$((i+1))
done                                 # PAS "od"

# --- case ---
case "$1" in
    -*)            echo "option" ;;
    *.txt | *.md)  echo "texte" ;;
    '' | *[!0-9]*) echo "pas un entier" ;;
    *)             echo "autre" ;;
esac

# --- Fonction ---
# Elle reçoit ses propres $1, $2... ; return 0 = VRAI, return 1 = FAUX
is_file() {
    [ -f "$1" ]                      # le statut du test devient celui de la fonction
}
if is_file "$f" ; then ...           # appel SANS [ ] et sans parenthèses

# --- Sortie ---
exit 0                               # statut du script (0 = succès)
exec cmd "$@"                        # remplace le script par cmd (même PID, statut de cmd)
```

Squelette type d'un script de partiel :

```sh
#!/bin/sh
# usage : script.sh dir mot...
if [ $# -lt 2 ] ; then
    echo "usage: $0 <dir> <mot> [<mot> ...]" >&2
    exit 1
fi
if [ ! -d "$1" ] ; then
    echo "$0: '$1' is not a directory" >&2
    exit 1
fi
dir="$1"
shift                                # "$@" = les mots restants
total=0
for f in "$dir"/* ; do
    [ -f "$f" ] || continue          # ignorer ce qui n'est pas un fichier régulier
    n=$(wc -l < "$f")
    echo "$(basename "$f"): $n"
    total=$((total + n))
done
echo "total: $total"
```

---

## Checklist : script à corriger

À passer **ligne par ligne**. Chaque point est déjà tombé au moins une fois en partiel.

| # | Chercher | Erreur → Correction |
|---|---|---|
| 1 | 1re ligne | `#!sh`, `!#/bin/sh` → `#!/bin/sh` |
| 2 | chaque `$var` | sans guillemets → `"$var"` |
| 3 | affectations | `$x = 1`, `x = 1` → `x=1` |
| 4 | `if`, `for`, `while` | `; then` / `; do` manquant ; `done`/`fi` manquant ou mal écrit (`od`) |
| 5 | `[ fonction args ]` | → `if fonction args ; then` |
| 6 | `return 0`/`return 1` | statut **inversé** par rapport à l'usage ? (0 = vrai) |
| 7 | `[ $# < 2 ]` | `<` est une redirection → `-lt` |
| 8 | `==` dans `[ ]` | → `=` |
| 9 | `echo 2>&1 "erreur"`, `echo >2` | → `echo "erreur" >&2` |
| 10 | après un message d'erreur | `exit 1` manquant (ou `exit` en trop si on doit continuer) |
| 11 | `exit(1)` | → `exit 1` |
| 12 | `$$`, `$*`, `$@` | → `"$@"` |
| 13 | `$(i+1)` | → `$((i+1))` |
| 14 | `read $f` | → `read -r f` (un NOM de variable) |
| 15 | `'... $var ...'` | quotes simples : pas de remplacement → `"..."` |
| 16 | `//` commentaire, `print`, `pop` | → `#`, `echo`, `shift` |
| 17 | variable globale réutilisée dans une fonction | → `local` ou autre nom |
| 18 | variable d'état (`skip=no`) | initialisée une seule fois au lieu de **à chaque tour** de boucle |
| 19 | `find ... -mindepth 1` | pour « pas dans les sous-répertoires » → `-maxdepth 1` |
| 20 | `cmd` dernière ligne | arguments oubliés : `exec cmd` → `exec cmd "$@"` |
| 21 | `[[ ]]` dans un script sh | → `[ ]` |
| 22 | `sed "/$2/p"`, `grep $mot` | le mot est vu comme une **regex** → `grep -F -e "$2"` |
